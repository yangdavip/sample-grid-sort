# Sample-Grid-Sort — Algorithm Description

Sample-Grid-Sort is a **sample-driven bucket sort**. Its distinguishing idea
is to *estimate the data distribution from a small sample first*, then use
that estimate to partition the full input into roughly balanced buckets.

## Pipeline

```
input array A (n elements)
      │
      ▼
[1] sample m elements                 (m ≈ max(4k, 256))
      │
      ▼
[2] derive k-1 bucket boundaries      (k = clamp(n / 1024, 16, 8192))
      │
      ▼
[3] Pass 1: count per bucket          (O(n))
      │
      ▼
[4] prefix sum → offsets
      │
      ▼
[5] Pass 2: scatter into contiguous buffer  (O(n))
      │
      ▼
[6] sort each bucket in place         (Σ O(gᵢ log gᵢ))
      │
      ▼
output sorted array A'
```

## Two bucket-selection strategies

| Strategy | Boundaries | Lookup | When to use |
|---|---|---|---|
| `uniform` | equal-width over `[min, max]` | **O(1)** via `(x - lo) / w` | arithmetic types, near-uniform data |
| `quantile`| sample quantiles | O(log k) via `upper_bound` | any totally-ordered type, skewed data |

`mode::auto_detect` (default) picks `uniform` for arithmetic `T` and
`quantile` otherwise.

## Why `uniform` matters

If bucket lookup is O(1), the total comparison count is:

```
n · O(1)   +   k · (n/k)·log(n/k)    =   n · log(n / k)
```

If you instead use binary search to locate the bucket, the count becomes:

```
n · log(k) + k · (n/k)·log(n/k)      =   n · log(n)     (no gain!)
```

This is why `uniform` mode is preferred whenever the value type is arithmetic.

## Robustness

- `n < min_input_size`  → defer to `std::sort`.
- All-equal input        → early return.
- Degenerate sample      → boundaries deduplicated; `real_k` may be < `k`.
- Skewed distribution    → switch to `quantile` mode (or set
  `config::strategy = mode::quantile`).

## Complexity

| Phase | Cost |
|---|---|
| Sample + sort | O(m log m), m ≈ 4k |
| Two-pass scatter | O(n) |
| Per-bucket sort | Σ O(gᵢ log gᵢ) |
| Ideal (balanced buckets) | **O(n log(n/k))** |
| Worst (single bucket) | O(n log n) — same as `std::sort` |

Correctness does **not** depend on the sampling being accurate; only the
performance does.
