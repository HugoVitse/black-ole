perf record -F 99 -g -- ./build/black++ole
perf script > perf.unfold
/usr/bin/stackcollapse-perf.pl perf.unfold > perf.folded
/usr/bin/flamegraph.pl perf.folded > flamegraph.svg
firefox flamegraph.svg
