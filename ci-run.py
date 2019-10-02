#! /usr/bin/env python3

import argparse
import os
from typing import Dict,List,TextIO,Iterable
import yaml
import docker
from concurrent.futures import ThreadPoolExecutor,as_completed,wait
from threading import Event,Thread,Semaphore
from functools import partial
import time
from queue import Queue,Empty
import xunitparser
import junitparser
import html

# TODO: not sure if this actually works, but it's here for the future just in case
lib_search_env = 'PATH' if os.name == 'nt' else 'LD_LIBRARY_PATH'

class TestCase:
    name = None
    config = None
    suite = None
    exit_code = None
    result = None
    failures = []
    exit_code = None
    def __init__(self, suite: 'TestSuite', name: str, config: Dict):
        self.name = name
        self.config = config
        self.suite = suite
        self.fullname = suite.name + '.' + name

class TestCaseAlreadyExistsInSuite(Exception):
    pass

class TestSuite:
    name = None
    executable_name = None
    test_cases = None
    def __init__(self, name: str, executable_name: str):
        self.name = name
        self.executable_name = executable_name
        self.test_cases = dict()
    def addcase(self, case: TestCase):
        if case.name in self.test_cases:
            raise TestCaseAlreadyExistsInSuite()
        self.test_cases[case.name] = case

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

def IsWritableFilePath(path: str):
    try:
        f = open(path, 'w')
    except Exception as e:
        raise argparse.ArgumentTypeError(path + ' cannot be opened for writing: ' + str(e))
    else:
        f.close()
        return os.path.abspath(path)

class TestConfig:
    def __init__(self, stream: TextIO):
        d = yaml.full_load(stream)
        if d == None:
            raise Exception('Test configuration has no entries ("default" required)')
        self.test_config_dict = d
    def get(self, suite: str, case: str):
        try:
            return self.test_config_dict[suite][case]
        except KeyError:
            return self.test_config_dict['default']

def make_filter(case: TestCase):
    return case.fullname + '*'

def get_suites(binary_dir: str, level_zero_lib_dir: str, runtime_image: str):
    def get_suites_from_binary(runtime_container, test_binary: str):
        test_binary_path = os.path.join(binary_dir, test_binary)

        _, output = runtime_container.exec_run(
          cmd = [test_binary_path, '--gtest_list_tests', '--logging-level=error'],
          workdir = binary_dir,
          stderr = False,
          environment = {
            'SetCommandStreamReceiver': '1',
            lib_search_env: level_zero_lib_dir},
        )
        output = output.decode('utf-8').splitlines()

        # parameterized cases are reduced to a single case for all parameterizations
        test_suites = []
        current_suite = None
        for line in output:
            if line[0] != ' ':  # test suite
                current_suite = TestSuite(
                  name = line.split('.')[0],
                  executable_name = test_binary_path)
                test_suites.append(current_suite)
            else:  # test case
                parameterized_case_name = line.split()[0]  # e.g., 'GivenXWhenYThenZ/1  # GetParam() = (0)' or just 'GivenXWhenYThenZ' if not parameterized
                case_name = parameterized_case_name.split('/')[0]  # e.g., 'GivenXWhenYThenZ'
                config = test_config.get(current_suite.name, case_name)
                case = TestCase(current_suite, case_name, config)
                try:
                    current_suite.addcase(case)
                except TestCaseAlreadyExistsInSuite:
                    del case

        return test_suites

    test_binaries = [
      filename for filename in os.listdir(binary_dir)
      if os.access(os.path.join(binary_dir, filename), os.X_OK) and
      os.path.isfile(os.path.join(binary_dir, filename))]

    client = docker.from_env(timeout = None)
    runtime = client.containers.run(
      image = runtime_image,
      volumes = {
        binary_dir: {'bind': binary_dir, 'mode': 'rw'},
        level_zero_lib_dir: {'bind': level_zero_lib_dir, 'mode': 'ro'}},
      detach = True,
      remove = True,
      tty = True)

    executor = ThreadPoolExecutor(max_workers = len(test_binaries))
    futures = [
      executor.submit(get_suites_from_binary,
        runtime_container = runtime,
        test_binary = binary)
      for binary in test_binaries]
    executor.shutdown(wait = False)

    results = [future.result() for future in as_completed(futures)]
    test_suites = [suite for suites in results for suite in suites]
    runtime.stop()

    return test_suites

def run_case_in_container(
  binary_dir: str,
  level_zero_lib_dir: str,
  log_dir: str,
  case: TestCase,
  runtime_container,
  env_vars: Dict):
    case_report_file_path = os.path.join(
      log_dir,
      ''.join(x if x.isalnum() else '_' for x in case.fullname) + '.xml')

    client = docker.from_env(timeout = None)
    exec_id = client.api.exec_create(
      container = runtime_container.id,
      cmd = [
        os.path.join(binary_dir, case.suite.executable_name),
        '--gtest_filter=' + make_filter(case),
        '--gtest_output=xml:' + case_report_file_path],
      environment = {
        **env_vars,
        lib_search_env: level_zero_lib_dir},
      workdir = binary_dir,
      stdout = False,
      stderr = False)['Id']
    client.api.exec_start(exec_id)

    def wait_for_exitcode():
        while True:
            exit_code = client.api.exec_inspect(exec_id)['ExitCode']
            if exit_code != None:
                return exit_code
            time.sleep(0.1)

    timeout = case.config['timeout']
    timeout_case_complete_notifier = Event()
    def simple_timeout():
        elapsed = 0
        while (not timeout_case_complete_notifier.is_set()) and elapsed < timeout:
            time.sleep(0.1)
            elapsed += 0.1

    completion_targets = {
      'process_exit': wait_for_exitcode,
      'stall_detected': simple_timeout
    }
    completion_executor = ThreadPoolExecutor(max_workers = len(completion_targets))
    completion_futures = {
      key: completion_executor.submit(target)
      for key, target in completion_targets.items()}
    completion_executor.shutdown(wait = False)

    result = 'PASS'
    failures = []
    exit_code = None
    completed_future = next(as_completed(completion_futures.values()))
    if completed_future == completion_futures['stall_detected']:
        result = 'STALL'
    elif completed_future == completion_futures['process_exit']:
        timeout_case_complete_notifier.set()
        exit_code = completed_future.result()
        # exit_code in (0, 1) indicates that the process ran to normal
        # completion (pass or fail). Any other code is an unexpected process
        # exit (crash).
        if exit_code == 1:
            result = 'FAIL'
            _, tr = xunitparser.parse(case_report_file_path)
            failures = [fail_str for _, fail_str in tr.failures]
        elif exit_code != 0:
            result = 'CRASH'

    return (result, failures, exit_code)

def run_cases_in_docker_fulsim(
  binary_dir: str,
  level_zero_lib_dir: str,
  log_dir: str,
  test_cases: List[TestCase]):
    client = docker.from_env(timeout = None)

    num_cases = len(test_cases)
    num_container_pairs = min(args.parallelism, num_cases)
    container_limit_semaphore = Semaphore(num_container_pairs)
    unready_queue = Queue(maxsize = num_container_pairs)
    recycle_queue = Queue(maxsize = num_container_pairs)
    ready_queue = Queue(maxsize = num_container_pairs)
    readiness_limit_semaphore = Semaphore(num_cases)
    drain_queue = Queue(maxsize = num_container_pairs)
    drain_progress_semaphore = Semaphore(0)
    num_service_threads = num_container_pairs

    def service_queues():
        def prepare(pair):
            fulsim, runtime = pair
            fulsim.exec_run(
              cmd = ["AubLoad", "-device", "skl.2.a0", "-socket", "tcp"],
              detach = True,
              stdout = False,
              stderr = False)
            # wait for the fulsim process in the container to open localhost:4321/tcp
            # TODO: how to make this cross-platform? Maybe write a python
            # program for this...
            runtime.exec_run('bash -c "while ! grep -q 00000000:10E1 /proc/net/tcp; do sleep 0.1; done"')
            return (fulsim, runtime)

        def make():
            runtime = client.containers.run(
              image = args.runtime_image,
              tty = True,
              volumes = {
                binary_dir: {'bind': binary_dir, 'mode': 'rw'},
                level_zero_lib_dir: {'bind': level_zero_lib_dir, 'mode': 'ro'},
                log_dir: {'bind': log_dir, 'mode': 'rw'}},
              detach = True,
              auto_remove = False,
              stdout = False,
              stderr = False)
            fulsim = client.containers.run(
              image = args.fulsim_image,
              network_mode = 'container:' + runtime.id,
              tty = True,
              detach = True,
              auto_remove = True,
              stdout = False,
              stderr = False)
            return prepare((fulsim, runtime))

        def remove(containers):
            with ThreadPoolExecutor(max_workers = len(containers)) as executor:
                for c in containers:
                    executor.submit(lambda: c.remove(force = True))

        def recycle(pair):
            remove(pair)
            return make()

        def service():
            while True:
                try:
                    ready_queue.put(
                      prepare(unready_queue.get(timeout = 0.1)))
                    return
                except Empty:
                    pass
                if container_limit_semaphore.acquire(blocking = False):
                    ready_queue.put(make())
                    drain_progress_semaphore.release()
                    return
                try:
                    ready_queue.put(
                      recycle(recycle_queue.get_nowait()))
                    return
                except Empty:
                    pass

        while readiness_limit_semaphore.acquire(blocking = False):
            service()
        while drain_progress_semaphore.acquire(blocking = False):
            remove(drain_queue.get())

    service_threads = [
      Thread(target = service_queues)
      for _ in range(num_service_threads)]
    for thread in service_threads:
        thread.start()

    with ThreadPoolExecutor(max_workers = num_cases) as executor:
        def run_case(case: TestCase):
            fulsim, runtime = ready_queue.get()
            return (
              (case, fulsim, runtime),
              run_case_in_container(
                binary_dir = binary_dir,
                level_zero_lib_dir = level_zero_lib_dir,
                log_dir = args.log_dir,
                case = case,
                env_vars = {
                  'SetCommandStreamReceiver': '2',
                  # 'ProductFamilyOverride': 'skl',  # TODO: this broke recently
                  'CreateMultipleRootDevices': '2'},
                runtime_container = runtime))
        test_futures = [executor.submit(partial(run_case, case)) for case in test_cases]

        num_completed = 0
        for completed_future in as_completed(test_futures):
            (case, fulsim, runtime), (result, failures, exit_code) = completed_future.result()
            if (num_completed < num_cases - num_container_pairs):
                if result == 'STALL':
                    recycle_queue.put((fulsim, runtime))
                else:
                    unready_queue.put((fulsim, runtime))
            else:
                drain_queue.put((fulsim, runtime))
            num_completed += 1
            case.result = result
            case.failures = failures
            case.exit_code = exit_code
            print('[' + str(num_completed) + '/' + str(num_cases) + '] [' + result + ']: ' + case.fullname)

    for thread in service_threads:
        thread.join()

def make_junit_case(case: TestCase):
    junit_case = junitparser.TestCase(case.fullname)
    if case.result == 'FAIL':
        junit_case.result = junitparser.Failure(
          message = html.unescape(case.failures[0]),
          type_ = case.result)
    elif case.result == 'STALL':
        timeout = case.config['timeout']
        junit_case.result = junitparser.Failure(
          message = 'Test did not complete after %d seconds' % timeout,
          type_ = case.result)
    elif case.result == 'CRASH':
        junit_case.result = junitparser.Failure(
          message = 'Test crashed with exit code %d' % case.exit_code,
          type_ = case.result)
    return junit_case

def make_junit_suite(suite: TestSuite, junit_cases: List[junitparser.TestCase]):
    junit_suite = junitparser.TestSuite(suite.name)
    for junit_case in junit_cases:
        junit_suite.add_testcase(junit_case)
    return junit_suite

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('environment', type = str, choices = ('docker-fulsim',), help = 'The type of execution environment in which the test cases will be run.')
    parser.add_argument('binary_dir', metavar = 'binary-dir', type = IsListableDirPath, help = 'Directory containing gtest binaries and SPVs.')
    parser.add_argument('level_zero_lib_dir', metavar = 'level-zero-lib-dir', type = IsListableDirPath, help = 'Directory containing liblevel_zero.so.')
    parser.add_argument('log_dir', metavar = 'log-dir', type = IsWritableDirPath, help = 'Directory to which the logs for each test case will be written')
    parser.add_argument('test_config', metavar = 'test-config', type = argparse.FileType('r'), help = 'Test yml configuration file.')
    parser.add_argument('--runtime-image', type = str, help = 'Name of the docker image to use for the runtime container.')
    parser.add_argument('--parallelism', '-n', type = int, default = 8, help = 'Number of runtime containers to use for parallel execution.')
    parser.add_argument('--fulsim-image', type = str, help = 'Name of the docker image to use for the fulsim container.')
    parser.add_argument('--output-junit-xml', type = IsWritableFilePath, help = 'JUnit XML file in which to store the test results.')
    args = parser.parse_args()

    test_config = TestConfig(args.test_config)

    print('Determining test cases to execute')
    test_suites = get_suites(
      binary_dir = args.binary_dir,
      level_zero_lib_dir = args.level_zero_lib_dir,
      runtime_image = args.runtime_image)
    test_cases = [
      case
      for suite in test_suites
      for case in suite.test_cases.values()]

    print('Executing cases')
    if args.environment == 'docker-fulsim':
        run_cases_in_docker_fulsim(
          binary_dir = args.binary_dir,
          level_zero_lib_dir = args.level_zero_lib_dir,
          log_dir = args.log_dir,
          test_cases = test_cases)

    if args.output_junit_xml != None:
        junit_suites = [
          make_junit_suite(
            suite,
            junit_cases = [
              make_junit_case(case)
              for case in suite.test_cases.values()])
          for suite in test_suites]
        junit_xml = junitparser.JUnitXml()
        for junit_suite in junit_suites:
            junit_xml.add_testsuite(junit_suite)
        junit_xml.update_statistics()
        junit_xml.write(args.output_junit_xml)
