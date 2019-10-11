#! /usr/bin/env bash

set -eu
set -o pipefail

test_dir=$1
test=$2

results_dir=$PWD/run_results
mkdir -p $results_dir

./ci-watchdog-stall.sh $test &>> run.log &

echo "running $test" &>> run.log

(
export LD_LIBRARY_PATH=$PWD/third_party/level_zero_linux/lib/level_zero
export SetCommandStreamReceiver=2
export ProductFamilyOverride=skl
cd $test_dir
./$test --logging-level=error 2>&1 | tee $results_dir/$test || true  # why isn't --logging-level=error the default?!
touch $results_dir/$test-done
) &

while ! [[ -e $results_dir/$test-stall || -e $results_dir/$test-done ]]; do
    sleep 0.1
done

if [[ -e $results_dir/$test-stall ]]; then
    echo "$test marked as stalled by watchdog" &>> run.log
else
    echo "$test ran to completion" &>> run.log
fi

exit 0
