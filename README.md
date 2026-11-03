# Sample-Grid-Sort

[![CI](https://github.com/yangdavip/sample-grid-sort/actions/workflows/ci.yml/badge.svg)](https://github.com/yangdavip/sample-grid-sort/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#requirements)

A **sample-driven bucket sort** header-only C++17 library. It draws a small
sample from the input, uses it to build `k` balanced buckets, scatters every
element in O(1) (uniform mode) or O(log k) (quantile mode) and then sorts
each bucket in place.

* 🌱 Header-only — drop `include/` into your project, or `find_package`.
* ⚡️ **~2× faster than `std::sort`** on large, roughly uniform or normal data.
* 🎯 Robust on skewed distributions via `mode::quantile`.
* 🧵 Bucket-independent — trivially parallelisable.
* 🧪 Full test suite + benchmarks included.

> **Not a drop-in replacement for `std::sort`.** Use it when you know your
> data is large and reasonably well-behaved. For unknown/adversarial data,
> `pdqsort` / `std::sort` is a safer default. See [docs/ALGORITHM.md](docs/ALGORITHM.md).

## Requirements

- C++17 compiler (GCC ≥ 7, Clang ≥ 6, MSVC ≥ 2019)
- CMake ≥ 3.16 (only if you use the bundled build)

## Quick start

```cpp
#include <sample_grid_sort/sample_grid_sort.hpp>
#include <vector>

int main() {
    std::vector<int> v = /* ... */;
    sgs::sort(v.begin(), v.end());              // ascending
    sgs::sort(v.begin(), v.end(), std::greater<>{});  // custom compare

    sgs::config cfg;
    cfg.strategy    = sgs::mode::quantile;      // robust to skew
    cfg.max_buckets = 256;
    sgs::sort(v.begin(), v.end(), cfg);
}
```

### Build & test

```bash
cmake -S . -B build -DSGS_BUILD_TESTS=ON -DSGS_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Benchmark

```bash
cmake -S . -B build -DSGS_BUILD_BENCHMARKS=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/benchmarks/benchmark 10000000
```

Expected speedup vs `std::sort` (n = 10⁷, single thread, indicative):

| Distribution | `std::sort` | `sgs::sort` | Speedup |
|---|---|---|---|
| uniform | 1100 ms | 550 ms | 2.0× |
| normal | 1150 ms | 620 ms | 1.85× |
| exponential (quantile mode) | 1100 ms | 700 ms | 1.55× |
| few duplicates | 900 ms | 500 ms | 1.8× |
| already sorted | 200 ms | 250 ms | 0.8× (slower) |

## API

```cpp
namespace sgs {

enum class mode { auto_detect, uniform, quantile };

struct config {
    mode strategy             = mode::auto_detect;
    std::size_t max_buckets   = 8192;
    std::size_t min_buckets   = 16;
    std::size_t bucket_target = 1024;
    std::size_t sample_factor = 4;
    std::size_t min_sample    = 256;
    std::size_t min_input_size = 4096;
};

template <class RandomIt>
void sort(RandomIt first, RandomIt last, config = {});

template <class RandomIt, class Compare>
void sort(RandomIt first, RandomIt last, Compare comp, config = {});

} // namespace sgs
```

See [docs/ALGORITHM.md](docs/ALGORITHM.md) for a full algorithm description and
[examples/basic_usage.cpp](examples/basic_usage.cpp) for runnable code.

## Python reference implementation

A pure-Python implementation lives in [`python/`](python/). Use it for
teaching and correctness cross-checking (not for performance).

```python
from sample_grid_sort import sort
print(sort([3, 1, 4, 1, 5, 9, 2, 6]))
```

## Platform support

| OS      | Compiler        | Tested |
|---------|-----------------|--------|
| Linux   | GCC 7+, Clang 6+| ✅     |
| macOS   | Apple Clang 10+ | ✅     |
| Windows | MSVC 2019+      | ✅     |

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md). Bug reports and PRs welcome.

## License

MIT — see [LICENSE](LICENSE).
