#!/bin/bash   

fuzid = `pidof fuzzy-database`
sudo perf record -F 9999 -p $fuzid -g -- sleep 30
sudo perf script | ./flamegraph/stackcollapse-perf.pl > out.perf-folded
sudo ./flamegraph/flamegraph.pl out.perf-folded > perf-kernal.svg 