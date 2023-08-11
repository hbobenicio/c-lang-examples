# perf-notes

## Perf Setup

```
sudo sysctl -w kernel.perf_event_paranoid=1  # or any other value from [-1, 4]
```

or just...

create `/etc/sysctl.d/foo.conf` with this:
```
kernel.perf_event_paranoid=1
```

after that reload sysctl with:
```
sysctl --system
```

## Perf Notes

- `-a` means "system-wide"
- Flamegraph requires `-g`
- `--call-graph` implies `-g`
- `--call-graph=fp` recommends `-fno-omit-frame-pointer` (some compilers can use the register where the fp is stored as a general purpose register)
- Always note the amount of samplings after perf record. Fast programs may require some artificial looping to let perf sample enough data

## Perf Execution

```
sudo perf record -v -e cycles,instructions,cache-references,cache-misses,bus-cycles --call-graph=fp ./http-server
perf script | ~/repos/github/brendangregg/FlameGraph/stackcollapse-perf.pl > out.perf-folded
cat out.perf-folded | ~/repos/github/brendangregg/FlameGraph/flamegraph.pl > flamegraph.svg
```

## References
- https://superuser.com/questions/980632/run-perf-without-root-rights/980757
- https://unix.stackexchange.com/questions/519070/security-implications-of-changing-perf-event-paranoid

