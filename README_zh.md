# Sample-Grid-Sort · 抽样分格排序

[![CI](https://github.com/yangdavip/sample-grid-sort/actions/workflows/ci.yml/badge.svg)](https://github.com/yangdavip/sample-grid-sort/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](#requirements)

一个**样本驱动的桶排序** C++17 头文件库。先抽样估计数据分布，再据此把输入切成 `k` 个均衡的桶，把每个元素 O(1) 或 O(log k) 归格，最后仅对桶内元素排序。

* 🌱 头文件库 —— 复制 `include/` 即可使用
* ⚡️ 大规模、分布较均匀数据上**约为 `std::sort` 的 2 倍**
* 🎯 `mode::quantile` 对偏斜/重尾数据依然稳健
* 🧵 桶间完全独立，易并行
* 🧪 完整测试与基准

> **不是 `std::sort` 的通用替代品**。大规模、分布规律的数据才是它的主场；对抗性数据建议回退到 `pdqsort`/`std::sort`。

## 快速上手

```cpp
#include <sample_grid_sort/sample_grid_sort.hpp>
#include <vector>

std::vector<int> v = /* ... */;
sgs::sort(v.begin(), v.end());

sgs::config cfg;
cfg.strategy    = sgs::mode::quantile;
cfg.max_buckets = 256;
sgs::sort(v.begin(), v.end(), cfg);
```

## 构建 / 测试

```bash
cmake -S . -B build -DSGS_BUILD_TESTS=ON -DSGS_BUILD_EXAMPLES=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## 性能（n = 10⁷，单线程，参考值）

| 数据分布 | `std::sort` | `sgs::sort` | 加速比 |
|---|---|---|---|
| 均匀 | 1100 ms | 550 ms | 2.0× |
| 正态 | 1150 ms | 620 ms | 1.85× |
| 指数（quantile） | 1100 ms | 700 ms | 1.55× |
| 大量重复 | 900 ms | 500 ms | 1.8× |
| 已排序 | 200 ms | 250 ms | 0.8×（更慢） |

算法细节见 [docs/ALGORITHM.md](docs/ALGORITHM.md)。

## 许可证

MIT —— 见 [LICENSE](LICENSE)。
