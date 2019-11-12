from abc import ABC, abstractmethod
import queue
from queue import Queue
from threading import Event, Thread, Semaphore, Barrier, Lock
import time

class Empty(Exception):
    """Indicates that the ManagedPool is empty.

    Raised when ManagedPool.get(wait = False) is called and there are no
    prepared resources in the pool.
    """
    pass

class ManagedPool(ABC):
    """Abstract class for creating pools of resources with managed lifecycles.

    A ManagedPool manages the complete lifecycle of finite resources, including
    creation, recycling, re-use, and destruction. Resources can be requested
    from and returned to the pool.

    When used as a context manager, exiting the context normally is equivalent
    to calling `shutdown(wait = True)`. If the context is exited because of an
    exception, it is equivalent to calling `shutdown(wait = True, force = True)`
    (i.e., all resources will be cleaned-up including ones currently in-use).
    """
    def __init__(self, size: int, max_workers: int = None):
        """Create an instance of a ManagedPool.

        Args:
            size (int): The number of resources that can exist at a time, either
                inside or outside the pool.
            max_workers (int, optional): The maximum number of worker threads to
                use for lifecycle management. If None or not given, it will
                default to the size of the pool.
        """
        self.resource_limit_semaphore = Semaphore(size)
        self.unready_queue = Queue(maxsize = size)
        self.recycle_queue = Queue(maxsize = size)
        self.ready_queue = Queue(maxsize = size)
        self.drain_queue = Queue(maxsize = size)
        self.using_set_lock = Lock()
        self.using_set = set()
        self.shutdown_event = Event()
        self.drain_progress_semaphore = Semaphore(0)

        def make_prepared():
            return self.prepare(self.make())

        def recycle(res):
            self.destroy(res)
            return make_prepared()

        def service():
            try:
                self.ready_queue.put(
                  self.prepare(self.unready_queue.get(timeout = 0.1)))
                return
            except queue.Empty:
                pass
            if self.resource_limit_semaphore.acquire(blocking = False):
                self.ready_queue.put(make_prepared())
                self.drain_progress_semaphore.release()
                return
            try:
                self.ready_queue.put(recycle(self.recycle_queue.get_nowait()))
                return
            except queue.Empty:
                pass

        def drain_single():
            while True:
                for q in (
                  self.unready_queue,
                  self.recycle_queue,
                  self.ready_queue,
                  self.drain_queue):
                    try:
                        self.destroy(q.get_nowait())
                        return
                    except queue.Empty:
                        pass
                time.sleep(0.1)

        def work():
            while not self.shutdown_event.is_set():
                service()
            while self.drain_progress_semaphore.acquire(blocking = False):
                drain_single()

        num_workers = size if max_workers == None else max_workers
        self.workers = [Thread(target = work) for _ in range(num_workers)]

    def __del__(self):
        self.shutdown(wait = True)

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.shutdown(wait = True, force = (exc_type != None))

    def start(self):
        for thread in self.workers:
            thread.start()

    def shutdown(self, wait: bool = False, force: bool = False):
        """Indicates that the ManagedPool should clean up and destroy the pool.

        Args:
            wait (bool): Block until the shutdown completely finishes. All
                workers complete their cleanup tasks and exit. This must be True
                at in at least one shutdown call to ensure all resources are
                freed.
            force (bool): Destroy resources that the caller still posesses
                without waiting for them to be returned by a `put()` or
                `discard()` call.
        """
        if force:
            with self.using_set_lock:
                for res in [res for res in self.using_set]:
                    self.using_set.remove(res)
                    self.drain_queue.put(res)
        self.shutdown_event.set()
        if wait:
            for thread in self.workers:
                thread.join()

    @abstractmethod
    def make(self):
        """Create a new instance of the resource.

        Creates a resource without preparing it.

        This method is abstract for subclasses of ManagedPool to define. It is
        meant to only be called by the ManagedPool's workers, not directly.

        Calls to this method will be matched by calls to `destroy()`.

        Example implementation:
            return MyResource()

        Returns:
            A new instance of a resource.
        """
        pass

    def prepare(self, res):
        """Prepare a resource for use.

        Prepares a resource after it has been made with `make()` or returned
        with `put()`. This is essentially a hook that is called on every
        resource before it is returned to the pool.

        This method may be optionally re-defined by subclasses of ManagedPool.
        It is meant to only be called by the ManagedPool's workers, not
        directly. The default implementation just returns the resource without
        any modification.

        Args:
            res: A resource which has not yet been prepared for use, or which
                has been already used and can be re-prepared.

        Returns:
            A prepared resource.
        """
        return res

    def destroy(self, res):
        """Destroys a resource.

        This method may be optionally re-defined by subclasses of ManagedPool.
        It is meant to only be called by the ManagedPool's workers, not
        directly. The default implementation just calls the resource's
        destructor with `del`.

        This method is the end of the lifecycle for a resource. It will match
        every call to `make()`.

        Args:
            res: A resource to destroy.
        """
        del res

    def get(self, wait: bool = True):
        """Get a prepared resource from the pool.

        All successful calls to `get()` must be matched by a call to `put()` or
        `discard()`.

        It is illegal to call this method after calling shutdown().

        Args:
            wait (bool, optional): Block until a prepared resource is available
                from the pool. Defaults to True.

        Returns:
            A prepared resource from the pool.

        Raises:
            Empty: If `wait` is equal to False and there are no prepared
                resources available in the pool.
            RuntimeError: If called after a call to shutdown().
        """
        if self.shutdown_event.is_set():
            raise RuntimeError('Call to get() after shutdown()')
        try:
            res = self.ready_queue.get(block = wait)
        except queue.Empty:
            raise Empty()
        with self.using_set_lock:
            self.using_set.add(res)
        return res

    def put(self, res):
        """Return a used resource previously acquired by a `get()` call.

        If `shutdown()` has not been called yet, the resource will be prepared
        for use again with `prepare()` before being placed back into the pool.
        Otherwise, the resource will be destroyed asynchronously.

        Args:
            res: The resource to return.
        """
        with self.using_set_lock:
            self.using_set.remove(res)
        if self.shutdown_event.is_set():
            self.drain_queue.put(res)
        else:
            self.unready_queue.put(res)

    def discard(self, res):
        """Discards a resource.

        If called before `shutdown()`, the resource will be recycled. Otherwise,
        the resource will be drained (destroyed).

        Args:
            res: The resource to discard.
        """
        with self.using_set_lock:
            self.using_set.remove(res)
        if self.shutdown_event.is_set():
            self.drain_queue.put(res)
        else:
            self.recycle_queue.put(res)
