#! /usr/bin/env bash

set -eu
set -o pipefail

test=$1

timeout=$(yq r test_config.yml ${test}.timeout)
if [ $timeout = "null" ]; then
    timeout=60
fi

old_size="initial"
while true; do
    new_size=$(stat --printf="%s" run_results/$test)
    if [ $old_size = $new_size ]; then
        echo "stall watchdog: progress stall detected for $test (no new log activity seen in last $timeout seconds)"
        touch run_results/$test-stall
        exit 0
    fi
    old_size=$new_size;
    sleep $timeout
done
