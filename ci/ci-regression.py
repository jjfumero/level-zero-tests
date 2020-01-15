#! /usr/bin/env python3

from pylzt import AubGenTestExecutor, FulsimTestExecutor, get_tests
import argparse
from concurrent.futures import as_completed
import os
import junitparser

def vis(obj):
    return [val for key, val in obj.__dict__.items() if not key.startswith('__')]

class TestResult:
    Pass = 'PASS'
    Fail = 'FAIL'
    Stall = 'STALL'
    Crash = 'CRASH'

class Mode:
    AubGen = 'aub'
    Fulsim = 'fulsim'
    Simics = 'simics'
    Native = 'native'

class Platform:
    skl = 'skl'
    ats = 'ats'

def IsReadableDirPath(path: str):
    if not os.access(path, os.R_OK):
        raise argparse.ArgumentTypeError(path + ' is not readable')
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
result_max_str_len = max(len(str(result)) for result in vis(TestResult))

mode_platform_max_str_len = max(
  len('%s-%s' % (mode, platform))
  for mode in vis(Mode)
  for platform in vis(Platform))

def print_test_result(
  completed_num: int,
  total_num: int,
  result: TestResult,
  fail_str: str,
  exit_code: int,
  mode: str,
  platform: str,
  test_name: str,
  max_fail_lines = 32):
    progress_str = '[%s/%s]' % (
      format(completed_num, '>%d' % len(str(total_num))),
      str(total_num))
    result_str = format(
      '%s%s' % (
        str(result),
        ' (%d)' % exit_code if result != TestResult.Stall else ''),
      '<%d' % (result_max_str_len + 6))
    mode_platform_str = format(
      '%s-%s' % (mode, platform),
      '<%d' % mode_platform_max_str_len)
    full_str = '%s %s %s %s' % (
      progress_str,
      result_str,
      mode_platform_str,
      test_name)

    print(full_str)

    if fail_str not in (None, ''):
        lines = fail_str.splitlines()
        if len(lines) > max_fail_lines:
            lines = ['(output truncated)', *lines[-(max_fail_lines-1):]]
        for line in lines:
            print(format(line, '>%d' % (len(line) + len(progress_str) + 1)))

def get_level_zero_lib_dir(root_dir: str):
    return os.path.join(root_dir, 'third_party/level_zero_%s/lib' %
      ('windows' if os.name == 'nt' else 'linux'))

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('root_dir', metavar = 'root-dir', type = IsReadableWritableDirPath, help = 'Root directory of the level_zero_tests repository.')
    parser.add_argument('binary_dir', metavar = 'binary-dir', type = IsReadableDirPath, help = 'Directory containing manifest.txt, test executables, and binaries.')
    parser.add_argument('runtime_image', metavar = 'runtime-image', type = str, help = 'Docker image to use for runtime containers.')
    parser.add_argument('--parallelism', '-n', type = int, default = 16, help = 'Number of runtime containers to use for parallel execution.')
    parser.add_argument('--fulsim-skl-image', type = str, help = 'Docker image to use for fulsim-skl containers.')
    parser.add_argument('--fulsim-skl-cmd', type = str, help = 'Command to run in the SKL fulsim container to start fulsim.')
    parser.add_argument('--output-junit-xml', type = IsWritableFilePath, help = 'JUnit XML file in which to store the test results.')
    args = parser.parse_args()

    level_zero_lib_dir = get_level_zero_lib_dir(args.root_dir)

    with open(os.path.join(args.binary_dir, 'manifest.txt'), 'r') as f:
        executables = [
          line.strip()
          for line in f
          if line.strip() != '' and line.strip()[0] != '#']

    with open(os.path.join(args.root_dir, 'ci', 'ci_test_config.yml'), 'r') as test_config_io:
        tests = get_tests(
          binary_dir = args.binary_dir,
          runtime_image = args.runtime_image,
          executables = executables,
          test_config_io = test_config_io,
          lib_dirs = [level_zero_lib_dir])

    executors = {
      (Mode.Fulsim, Platform.skl): FulsimTestExecutor(
        binary_dir = args.binary_dir,
        lib_dirs = [level_zero_lib_dir],
        max_container_pairs = args.parallelism,
        platform = Platform.skl,
        runtime_image = args.runtime_image,
        fulsim_image = args.fulsim_skl_image,
        fulsim_cmd = args.fulsim_skl_cmd.split()),
      **{
        (Mode.AubGen, platform): AubGenTestExecutor(
          binary_dir = args.binary_dir,
          lib_dirs = [level_zero_lib_dir],
          max_workers = args.parallelism,
          platform = platform,
          runtime_image = args.runtime_image)
        for platform in vis(Platform)
      }
    }

    for executor in executors.values():
        executor.start()

    execution_futures = [
      executor.submit(
        context = (test_name, platform, mode, config['timeout']),
        executable = config['executable'],
        args = config['args'],
        timeout = config['timeout'])
      for test_name, config in tests.items()
      for mode in config['regression']
      for platform in config['regression'][mode]
      for (exec_mode, exec_platform), executor in executors.items()
      if exec_mode == mode and exec_platform == platform]

    junit_suite = junitparser.TestSuite('regression')

    completed_num = 0
    total_num = len(execution_futures)
    for future in as_completed(execution_futures):
        completed_num += 1

        (test_name, platform, mode, timeout), output, exit_code = future.result()

        if exit_code == None:
            result = TestResult.Stall
        elif exit_code == 0:
            result = TestResult.Pass
        elif exit_code == 1:
            result = TestResult.Fail
        else:
            result = TestResult.Crash

        junit_case = junitparser.TestCase('%s-%s %s' % (mode, platform, test_name))
        if result != TestResult.Pass:
            junit_case.result = junitparser.Failure(
              message = (
                output if result == TestResult.Fail else
                'Test did not complete after %d seconds' % timeout if result == TestResult.Stall else
                'Test crashed with exit code %d' % exit_code),
              type_ = result)
        junit_suite.add_testcase(junit_case)

        print_test_result(
          completed_num = completed_num,
          total_num = total_num,
          result = result,
          fail_str = None if result == TestResult.Pass else output,
          exit_code = exit_code,
          mode = mode,
          platform = platform,
          test_name = test_name)

    if args.output_junit_xml != None:
        junit_xml = junitparser.JUnitXml()
        junit_xml.add_testsuite(junit_suite)
        junit_xml.update_statistics()
        junit_xml.write(args.output_junit_xml)

    for executor in executors.values():
        executor.shutdown(wait = True)
