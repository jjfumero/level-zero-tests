#! /usr/bin/env python3

from ci import (
  AubGenTestExecutor,
  get_tests,
  IsReadableDirPath,
  IsWritableDirPath,
  IsReadableWritableDirPath,
  Mode,
  Platform,
  vis)
import argparse
from concurrent.futures import as_completed
import os

platform_max_str_len = max(len(platform) for platform in vis(Platform))

def print_progress(
  completed_num: int,
  total_num: int,
  platform: str,
  status: str,
  fail_str: str,
  max_fail_lines = 32):
    progress_str = '[%s/%s]' % (
      format(completed_num, '>%d' % len(str(total_num))),
      str(total_num))
    platform_str = format(
      platform,
      '<%d' % platform_max_str_len)
    full_str = '%s %s %s' % (progress_str, platform_str, status)

    print(full_str)

    if fail_str not in (None, ''):
        lines = fail_str.splitlines()
        if len(lines) > max_fail_lines:
            lines = [*lines[:max_fail_lines-1], '(output truncated)']
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
    parser.add_argument('output_dir', metavar = 'output-dir', type = IsWritableDirPath, help = 'Output directory for the generated AUB files. Subdirectories will be created for each platform.')
    parser.add_argument('--parallelism', '-n', type = int, default = 32, help = 'Number of runtime containers to use for parallel execution.')
    args = parser.parse_args()

    level_zero_lib_dir = get_level_zero_lib_dir(args.root_dir)

    tests = get_tests(
      root_dir = args.root_dir,
      binary_dir = args.binary_dir,
      lib_dirs = [level_zero_lib_dir],
      runtime_image = args.runtime_image)

    def get_aub_path(context):
        test_name, platform = context
        aub_name = ''.join(x if x.isalnum() else '_' for x in test_name) + '.aub'
        return os.path.join(args.output_dir, platform, aub_name)

    executors = {
      platform: AubGenTestExecutor(
        binary_dir = args.binary_dir,
        lib_dirs = [level_zero_lib_dir],
        max_workers = args.parallelism,
        platform = platform,
        runtime_image = args.runtime_image,
        aub_path_fn = lambda context, executable, args: get_aub_path(context))
      for platform in vis(Platform)}

    for executor in executors.values():
        executor.start()

    execution_futures = [
      executor.submit(
        context = (test_name, platform),
        executable = config['executable'],
        args = config['args'])
      for test_name, config in tests.items()
      for platform in config['publish-aubs']
      for exec_platform, executor in executors.items()
      if exec_platform == platform]

    for platform in vis(Platform):
        os.mkdir(os.path.join(args.output_dir, platform))

    completed_num = 0
    total_num = len(execution_futures)
    for future in as_completed(execution_futures):
        completed_num += 1
        fail_str = None
        try:
            (test_name, platform), output, exit_code = future.result()
        except RuntimeError as e:
            (test_name, platform), reason, output, exit_code = e.args
            status = reason
            fail_str = output
        else:
            if exit_code == None:
                status = '%s timed out' % test_name
            elif exit_code != 0:
                status = '%s failed with exit code %d' % (test_name, exit_code)
                fail_str = output
            else:
                status = get_aub_path((test_name, platform))

        print_progress(
          completed_num = completed_num,
          total_num = total_num,
          platform = platform,
          status = status,
          fail_str = fail_str)

    for executor in executors.values():
        executor.shutdown(wait = True)
