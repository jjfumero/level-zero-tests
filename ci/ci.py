from typing import Dict, Callable, Sequence, List
import argparse
import docker
from threading import Timer, Event
from concurrent.futures import ThreadPoolExecutor,as_completed
import time
from multiprocessing import Process,Pipe
from managedpool import ManagedPool
import os
import re
import yaml
from shutil import copytree, ignore_patterns
from tempfile import TemporaryDirectory
from abc import ABC, abstractmethod

# TODO: not sure if this actually works, but it's here for the future just in case
lib_search_env = 'PATH' if os.name == 'nt' else 'LD_LIBRARY_PATH'

def vis(obj):
    return [val for key, val in obj.__dict__.items() if not key.startswith('__')]

class Mode:
    AubGen = 'aub'
    Fulsim = 'fulsim'
    Simics = 'simics'
    Native = 'native'

class Platform:
    skl = 'skl'
    ats = 'ats'

def IsListableDirPath(path: str):
    try:
        os.listdir(path)
    except Exception as e:
        raise argparse.ArgumentTypeError(path + ' is not a listable directory: ' + str(e))
    return os.path.abspath(path)

def IsReadableDirPath(path: str):
    if not os.access(path, os.R_OK):
        raise argparse.ArgumentTypeError(path + ' is not readable')
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError(path + ' is not a directory')
    return os.path.abspath(path)

def IsWritableDirPath(path: str):
    if not os.access(path, os.W_OK):
        raise argparse.ArgumentTypeError(path + ' is not writable')
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError(path + ' is not a directory')
    return os.path.abspath(path)

def IsReadableWritableDirPath(path: str):
    if not os.access(path, os.R_OK):
        raise argparse.ArgumentTypeError(path + ' is not readable')
    if not os.access(path, os.W_OK):
        raise argparse.ArgumentTypeError(path + ' is not writable')
    if not os.path.isdir(path):
        raise argparse.ArgumentTypeError(path + ' is not a directory')
    return os.path.abspath(path)

def IsWritableFilePath(path: str):
    try:
        f = open(path, 'w')
    except Exception as e:
        raise argparse.ArgumentTypeError(path + ' cannot be opened for writing: ' + str(e))
    else:
        f.close()
        return os.path.abspath(path)

class CommandTimeout(Exception):
    pass

def run_command_in_container(
  container,
  cmd: Sequence[str],
  workdir: str,
  env: Dict[str,str] = {},
  timeout: int = None,
  stdout = True,
  stderr = True,
  demux = False):
    """Runs a command in a container, optionally with a timeout.

    Raises:
        CommandTimeout: If a timeout occurs.

    Returns:
        A tuple of (exit_code, output). If demux equals True, output will be a
        bytes tuple of (stdout, stderr). Otherwise, output will be a bytes of
        the raw output.
    """
    client = docker.from_env(timeout = None)
    exec_id = client.api.exec_create(
      container = container.id,
      cmd = cmd,
      environment = env,
      workdir = workdir,
      stdout = stdout,
      stderr = stderr)['Id']

    use_process_and_timeout = timeout not in (None, 0)

    if use_process_and_timeout:
        # Start the exec in a process so that the process can be killed in the event
        # of a timeout. Otherwise, it could potentially block forever attached to
        # stdout and stderr.
        output_pipe_rx, output_pipe_tx = Pipe(duplex = False)
        def start_exec():
            client = docker.from_env(timeout = None)
            output_pipe_tx.send(client.api.exec_start(exec_id, demux = demux))
        exec_process = Process(target = start_exec)
        exec_process.start()
    else:
        # No timeout is needed, so no extra process is needed.
        output = client.api.exec_start(exec_id, demux = demux)

    timeout_event = Event()
    if use_process_and_timeout:
        timeout_timer = Timer(timeout, timeout_event.set)
        timeout_timer.start()

    stop_waiting_event = Event()

    def wait_for_exitcode():
        while not stop_waiting_event.is_set():
            exit_code = client.api.exec_inspect(exec_id)['ExitCode']
            if exit_code != None:
                return exit_code
            time.sleep(0.1)
        return None

    completion_targets = {
        'process_exit': wait_for_exitcode,
        'timeout': timeout_event.wait
    }
    with ThreadPoolExecutor(max_workers = len(completion_targets)) as e:
        completion_futures = {
            key: e.submit(target)
            for key, target in completion_targets.items()}

        completed_future = next(as_completed(completion_futures.values()))
        if completed_future == completion_futures['timeout']:
            exec_process.kill()
            stop_waiting_event.set()
            raise CommandTimeout()
        elif completed_future == completion_futures['process_exit']:
            timeout_event.set()
            exit_code = completed_future.result()
            if use_process_and_timeout:
                output = output_pipe_rx.recv()
                exec_process.kill()

    if use_process_and_timeout:
        output_pipe_rx.close()

    return (exit_code, output)

def parse_suites_from_gtest_list_tests(output: bytes) -> Dict[str, str]:
    """Parses output from --gtest_list_tests.

    Parameterized tests are reduced to a single test.

    Args:
        output (bytes): The raw output from executing the gtest binary with
            --gtest_list_tests.

    Returns:
        A dictionary mapping suite names to lists of test names.
    """
    lines = output.decode('utf-8').splitlines()
    suites = {}
    current_suite = None
    for line in lines:
        if line[0] != ' ':  # test suite
            current_suite_name = line.split('.')[0]
            current_suite = []
            suites[current_suite_name] = current_suite
        else:  # test
            # parameterized tests are reduced to a single test for all parameterizations
            parameterized_test_name = line.split()[0]  # e.g., 'GivenXWhenYThenZ/1  # GetParam() = (0)' or just 'GivenXWhenYThenZ' if not parameterized
            test_name = parameterized_test_name.split('/')[0]  # e.g., 'GivenXWhenYThenZ'
            current_suite.append(test_name)
    return suites

def check_if_callable_is_gtest(run: Callable[[Sequence], bytes]) -> bool:
    """Checks if an callable is (runs) a Google Test (gtest) executable.

    Args:
        run (Callable[[Sequence], bytes]): Callable which accepts a sequence of
            arguments to pass to the gtest executable it represents and returns
            the stdout in byte form.

    Returns:
        True if the callable runs a Google Test executable, False otherwise.
    """
    lines = run(['--help']).decode('utf-8').splitlines()
    return lines != [] and lines[0].strip() == 'This program contains tests written using Google Test. You can use the'

def get_suites_from_gtest_callable(run: Callable[[Sequence], bytes]) -> Dict[str, List[str]]:
    """Get suite and gtest data from a googletest.

    Args:
        run (Callable[[Sequence], bytes]): Callable which accepts a sequence of
            arguments to pass to the gtest executable it represents and returns
            the stdout in byte form.

    Returns:
        A dictionary mapping suite names to lists of test names.
    """
    return parse_suites_from_gtest_list_tests(run(['--gtest_list_tests']))

def get_tests(root_dir: str, binary_dir: str, lib_dirs: List[str], runtime_image: str):
    """Discovers and returns information about the tests and how to run them.

    Reads the manifest.txt file in the binary_dir to get all the test
    executables, discovers all the gtest tests from the executables, and merges
    this information with the additional test descriptions and configurations in
    ci_test_config.yml. ci_test_config.yml contains full, partial, and default
    test descriptions and configurations.

    Args:
        root_dir (str): Path to the root of the repository checkout. Used for
            looking up the ci_test_config.yml file.
        binary_dir (str): Path to the test binary install location, containing
            all the test binaries and manifest.txt file.
        lib_dirs (List[str]): Paths to directories containing necessary runtime
            libraries.
        runtime_image (str): Docker image to use for the runtime container when
            retrieving gtest information from the test executables.

    Returns:
        A dictionary mapping test names to a dictionary of the test's
        configuration. The test configuration dictionary can be passed into any
        TestExecutor to execute the test.
    """
    def get_gtest_tests_from_executable(runtime, executable: str):
        def runner(container, executable):
            def run(args):
                tmpdir = os.path.join(binary_dir, '.tmp')
                os.makedirs(tmpdir, exist_ok = True)
                with TemporaryDirectory(dir = tmpdir) as workdir:
                    executable_path = os.path.join(workdir, executable)
                    copytree(
                      src = binary_dir,
                      dst = workdir,
                      ignore = ignore_patterns('*.tmp*'),
                      dirs_exist_ok = True,
                      copy_function = os.link)
                    _, output = run_command_in_container(
                      container = container,
                      cmd = [executable_path, *args],
                      workdir = workdir,
                      env = {
                        'SetCommandStreamReceiver': '1',
                        lib_search_env: os.pathsep.join(lib_dirs)},
                      stderr = False)
                return output
            return run

        def check_if_gtest(runtime, executable: str):
            return check_if_callable_is_gtest(runner(runtime, executable))

        if not check_if_gtest(runtime, executable):
            return []

        suites_dict = get_suites_from_gtest_callable(runner(runtime, executable))
        return [
          '%s.%s' % (suite_name, test_name)
          for suite_name in suites_dict
          for test_name in suites_dict[suite_name]]

    with open(os.path.join(binary_dir, 'manifest.txt'), 'r') as f:
        executables = [
          line.strip()
          for line in f
          if line.strip() != '' and line.strip()[0] != '#']

    with open(os.path.join(root_dir, 'ci', 'ci_test_config.yml'), 'r') as f:
        ci_test_config_values = yaml.full_load(f)
    if ci_test_config_values == None:
        ci_test_config_values = []

    with RuntimeManagedPool(
      size = 1,
      runtime_args = dict(
        image = runtime_image,
        volumes = {
          directory: {'bind': directory, 'mode': 'rw'}
          for directory in [binary_dir, *lib_dirs]})
    ) as pool:
        runtime = pool.get()
        with ThreadPoolExecutor(max_workers = len(executables)) as executor:
            gtest_tests = {
              test_name: {
                'executable': executable,
                'args': ['--gtest_filter=%s*' % test_name]}
              for executable, test_names in executor.map(
                lambda executable: (
                  executable,
                  get_gtest_tests_from_executable(runtime, executable)),
                executables)
              for test_name in test_names}
        pool.put(runtime)

    def match(pattern, string):
        return re.fullmatch(re.escape(pattern).replace(r'\*', '.*'), string)

    tests = {
      **dict.fromkeys([
          pattern
          for patterns in ci_test_config_values
          for pattern in patterns
          if '*' not in pattern],
        {}),
      **gtest_tests}

    for patterns in ci_test_config_values:
        for pattern, new_config in patterns.items():
            for test_name, current_config in tests.items():
                if match(pattern, test_name):
                    current_config.update(new_config)

    return tests

class RuntimeManagedPool(ManagedPool):
    def __init__(self,
      runtime_args: Dict,
      size: int,
      max_workers: int = None):
        client = docker.from_env(timeout = None)
        self.make_runtime = lambda: client.containers.run(
          **runtime_args,
          auto_remove = False,
          stdout = False,
          stderr = False,
          detach = True,
          tty = True)
        super().__init__(size = size, max_workers = max_workers)

    def make(self):
        return self.make_runtime()

    def destroy(self, res):
        res.remove(force = True)

class FulsimRuntimeManagedPool(ManagedPool):
    def __init__(self,
      runtime_args: Dict,
      fulsim_args: Dict,
      fulsim_cmd: List[str],
      size: int,
      max_workers: int = None):
        client = docker.from_env(timeout = None)
        self.make_runtime = lambda: client.containers.run(
          **runtime_args,
          auto_remove = False,
          stdout = False,
          stderr = False,
          detach = True,
          tty = True)
        self.make_fulsim = lambda runtime: client.containers.run(
          **fulsim_args,
          auto_remove = False,
          stdout = False,
          stderr = False,
          network_mode = 'container:' + runtime.id,
          detach = True,
          tty = True)
        self.client = docker.from_env(timeout = None)
        self.fulsim_cmd = fulsim_cmd
        super().__init__(size = size, max_workers = max_workers)

    def make(self):
        runtime = self.make_runtime()
        fulsim = self.make_fulsim(runtime)
        return (fulsim, runtime)

    def prepare(self, res):
        fulsim, runtime = res
        fulsim.exec_run(
          cmd = self.fulsim_cmd,
          detach = True,
          stdout = False,
          stderr = False)
        # wait for the fulsim process in the container to open localhost:4321/tcp
        # TODO: how to make this cross-platform? Maybe write a python
        # program for this...
        runtime.exec_run('bash -c "while ! grep -q 00000000:10E1 /proc/net/tcp; do sleep 0.1; done"')
        return (fulsim, runtime)

    def destroy(self, res):
        fulsim, runtime = res
        with ThreadPoolExecutor(max_workers = 2) as e:
            for c in (fulsim, runtime):
                e.submit(lambda: c.remove(force = True))

class TestExecutor(ABC):
    """Abstract base class for implementing test executors.

    Implementations of this class execute tests independently, concurrently, and
    asynchronously, possibly using dynamic resources (e.g., docker containers).
    Implementation-specific details about how tests should be executed should be
    taken care of during __init__().
    """
    def __del__(self):
        self.shutdown(wait = True, force = True)

    def __enter__(self):
        self.start()
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        self.shutdown(wait = True, force = (exc_type != None))

    @abstractmethod
    def start(self):
        """Start processing submitted tests.

        Creates the resources necessary for test execution. Tests may have been
        submitted prior to calling this method.
        """
        pass

    @abstractmethod
    def shutdown(self, wait: bool = False, force: bool = False):
        """Start cleaning-up runtime resources and the internal executor.

        Signals that no more tests will be submitted for execution and that
        dynamic resources can start to be cleaned-up as they become idle.

        Calls to TestExecutor.submit() made after shutdown should raise
        RuntimeError.

        Setting `force = True` will cause all of the runtime resources to be
        forcibly killed even if tests are still currently running and using
        them.

        Args:
            wait (bool): Block until all tests have finished executing.
            force (bool): Cancel outstanding test execution and force resources
                to be cleaned-up now.
        """
        pass

    @abstractmethod
    def submit(self, context, executable: str, args: List[str], timeout: int):
        """Submits a test for execution.

        Args:
            context: Arbitrary object that is returned as part of the future's
                result.
            executable (str): The name of the test's executable.
            args (List[str]): List of arguments to pass to the executable to run
                the test.
            timeout (int): Maximum runtime in seconds that the test can run
                before being killed.

        Raises:
            RuntimeError: If called after a call to shutdown().

        Returns:
            A Future object representing the test's execution against fulsim.
                The future's result will be a tuple of (context, output: str,
                exit_code: int). If a timeout occurs, exit_code and output will
                be None.
        """
        pass

class AubGenTestExecutor(TestExecutor):
    """Generate AUB traces from test execution.

    Tests are executed in a docker container. AUB traces are generated and
    collected from each test's execution.

    AUB files will only be saved after execution if the test exits with either 0
    or 1 as its exit code. Other exit codes are considered crashes, and
    therefore the AUB is considered to be corrupt.

    Directory arguments supplied during init (like binary_dir) are bind-mounted
    one-to-one from the host into the runtime container.

    Args:
        binary_dir (str): Directory containing the executables to execute.
        lib_dirs (List[str]): Directories containing runtime libraries needed
            for test execution. This will be used to construct the
            `LD_LIBRARY_PATH` environment variable on Linux, and `PATH`
            environment variable on Windows.
        max_workers (int): Maximum number of tests that can be executing at a
            time concurrently. Be careful with setting this to large values so
            as to not exhaust system file descriptor limits.
        platform (str): Product family the tests are being executed against.
        runtime_image (str): The docker image to use for creating runtime
            containers.
        aub_path_fn (Callable): Function invoked on every submit() which given
            (context, executable: str, args: List[str]) as arguments returns a
            string path that the resulting AUB file should be written to. If
            None (default), the AUB file will not be saved after execution.
    """
    def __init__(
      self,
      binary_dir: str,
      lib_dirs: List[str],
      max_workers: int,
      platform: str,
      runtime_image: str,
      aub_path_fn: Callable = None):
        self.shutdown_event = Event()
        self.aub_path_fn = aub_path_fn
        self.executor = ThreadPoolExecutor(max_workers = max_workers)
        self.binary_dir = binary_dir
        self.pool = RuntimeManagedPool(
          size = 1,
          runtime_args = dict(
            image = runtime_image,
            environment = {
              lib_search_env: os.pathsep.join(lib_dirs),
              'ProductFamilyOverride': platform},
            volumes = {
              directory: {'bind': directory, 'mode': 'rw'}
              for directory in [binary_dir, *lib_dirs]},
            shm_size = '8G'))  # TODO: manage this from ci_test_config.yml
        self.runtime_available_event = Event()

    def start(self):
        self.pool.start()
        self.runtime = self.pool.get()
        self.runtime_available_event.set()

    def shutdown(self, wait: bool = False, force: bool = False):
        if not self.shutdown_event.is_set():
            self.shutdown_event.set()
            self.pool.put(self.runtime)
        self.pool.shutdown(wait = wait, force = force)
        self.executor.shutdown(wait = wait)

    def submit(self, context, executable: str, args: List[str], timeout: int = None):
        """Start running a test to collect its trace.

        Returns:
            A Future object representing the test's execution. The future's
                result will be a tuple of (context, output: str, exit_code:
                int). If the executable exits with exit code 0 but no AUB file
                is generated, or if more than one AUB file is generated, a
                RuntimeError exception containing (context, reason: str, output:
                str) will be raised as the future's result. If a timeout occurs,
                exit_code and output will be None.
        """
        if self.shutdown_event.is_set():
            raise RuntimeError('Call to submit() after shutdown()')

        def run_test():
            self.runtime_available_event.wait()

            tmpdir = os.path.join(self.binary_dir, '.tmp')
            os.makedirs(tmpdir, exist_ok = True)
            with TemporaryDirectory(dir = tmpdir) as workdir:
                copytree(
                  src = self.binary_dir,
                  dst = workdir,
                  ignore = ignore_patterns('*.tmp*'),
                  dirs_exist_ok = True,
                  copy_function = os.link)
                original_contents = os.listdir(workdir)

                cmd = [os.path.join(workdir, executable), *args]

                try:
                    exit_code, output = run_command_in_container(
                    container = self.runtime,
                    cmd = cmd,
                    workdir = workdir,
                    env = dict(
                        CreateMultipleRootDevices = '2',
                        SetCommandStreamReceiver = '1'),
                    timeout = timeout)
                except CommandTimeout:
                    output = None
                    exit_code = None
                else:
                    output = output.decode('utf-8')

                    aub_files = [
                      f for f in os.listdir(workdir)
                      if f not in original_contents
                      and f.split('.')[-1] == 'aub']

                    if len(aub_files) > 1:
                        raise RuntimeError(
                          context,
                          '%s produced more than one AUB file (%d)' % (
                            ' '.join(cmd),
                            len(aub_files)),
                          output)

                    aub_file = None
                    if exit_code in (0, 1) and len(aub_files) == 1:
                        aub_file = aub_files[0]

                    if self.aub_path_fn != None and aub_file != None:
                        os.rename(
                          src = os.path.join(workdir, aub_file),
                          dst = self.aub_path_fn(context, executable, args))

            return context, output, exit_code

        return self.executor.submit(fn = run_test)

class FulsimTestExecutor(TestExecutor):
    """Execute tests against fulsim asynchronously and independently.

    Tests are executed in docker containers. Execution occurs with a pair of
    containers: runtime and fulsim. The runtime container is a container
    suitable for executing the test's executable, and the fulsim container
    contains an instance of fulsim that can be invoked by an `AubLoad` shell
    command. The two containers share a network namespace, to enable the test
    running in the runtime container to establish a socket with the fulsim
    process running in the other container.

    Directory arguments supplied during init (like binary_dir) are bind-mounted
    one-to-one from the host into the runtime container.

    Args:
        binary_dir (str): Directory containing the executables to execute.
        lib_dirs (List[str]): Directories containing runtime libraries needed
            for test execution. This will be used to construct the
            `LD_LIBRARY_PATH` environment variable on Linux, and `PATH`
            environment variable on Windows.
        max_container_pairs (int): Maximum number of (fulsim, runtime) container
            pairs to have created at a time. This is effectively the maximum
            level of concurrency that the executor can work at.
        platform (str): Product family the tests are being executed against.
        runtime_image (str): The docker image to use for creating runtime
            containers.
        fulsim_image (str): The docker image to use for creating fulsim
            containers.
        fulsim_cmd (List[str]): The command to execute in the fulsim container
            to start fulsim.
    """
    def __init__(
      self,
      binary_dir: str,
      lib_dirs: List[str],
      max_container_pairs: int,
      platform: str,
      runtime_image: str,
      fulsim_image: str,
      fulsim_cmd: List[str]):
        self.shutdown_event = Event()
        self.executor = ThreadPoolExecutor(max_workers = max_container_pairs)
        self.binary_dir = binary_dir
        self.pool = FulsimRuntimeManagedPool(
          size = max_container_pairs,
          runtime_args = dict(
            image = runtime_image,
            environment = {
              lib_search_env: os.pathsep.join(lib_dirs),
              'ProductFamilyOverride': platform},
            volumes = {
              directory: {'bind': directory, 'mode': 'rw'}
              for directory in [binary_dir, *lib_dirs]}),
          fulsim_args = dict(
            image = fulsim_image),
          fulsim_cmd = fulsim_cmd)

    def start(self):
        self.pool.start()

    def shutdown(self, wait: bool = False, force: bool = False):
        self.shutdown_event.set()
        self.pool.shutdown(wait = wait, force = force)
        self.executor.shutdown(wait = wait)

    def submit(self, context, executable: str, args: List[str], timeout: int = None):
        """Start running a test against fulsim.

        Each test submitted will be run against its own fresh instance of
        fulsim.

        Returns:
            A Future object representing the test's execution against fulsim.
                The future's result will be a tuple of (context, output: str,
                exit_code: int). If a timeout occurs, exit_code and output will
                be None.
        """
        if self.shutdown_event.is_set():
            raise RuntimeError('Call to submit() after shutdown()')

        def run_test():
            fulsim, runtime = self.pool.get()

            try:
                exit_code, output = run_command_in_container(
                  container = runtime,
                  cmd = [os.path.join(self.binary_dir, executable), *args],
                  workdir = self.binary_dir,
                  env = dict(
                    CreateMultipleRootDevices = '2',
                    SetCommandStreamReceiver = '2'),
                  timeout = timeout)
            except CommandTimeout:
                output = None
                exit_code = None
                self.pool.discard((fulsim, runtime))
            else:
                self.pool.put((fulsim, runtime))
                output = output.decode('utf-8')
            return context, output, exit_code

        return self.executor.submit(fn = run_test)
