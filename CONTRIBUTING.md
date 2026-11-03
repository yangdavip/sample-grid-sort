# Contributing

Thanks for your interest!

## Building

```bash
cmake -S . -B build -DSGS_BUILD_TESTS=ON -DSGS_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Before opening a PR

1. Format with `clang-format -i` (config in `.clang-format`).
2. Add a test for any non-trivial behaviour change.
3. Ensure `ctest` passes on at least one compiler (CI covers the rest).
4. If the change affects performance, include a before/after snippet
   from `benchmarks/benchmark`.

## Reporting a bug

Include:

- Compiler + version + C++ standard (`-std=c++17` / `-std=c++20`)
- OS
- Minimal reproducible snippet
- Expected vs actual behaviour

## Design notes

- Correctness of `sgs::sort` must **never** depend on the sampling being accurate.
- The two strategies (`uniform`, `quantile`) must produce identical output.
- Prefer readability over micro-optimisation unless the benchmark justifies it.
