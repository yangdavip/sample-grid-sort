# sample-grid-sort (Python)

Pure-Python **reference** implementation of Sample-Grid-Sort — mirrors the
C++ algorithm for teaching, verification, and quick prototyping.

> Performance is **not** the goal here. Use the C++ implementation if you
> actually need speed.

## Install

```bash
pip install -e python
```

## Usage

```python
from sample_grid_sort import sort

print(sort([3, 1, 4, 1, 5, 9, 2, 6]))
# [1, 1, 2, 3, 4, 5, 6, 9]

# custom key / reverse
records = [{"age": 30}, {"age": 20}, {"age": 40}]
print(sort(records, key=lambda r: r["age"], reverse=True))
```

## API

```python
sort(
    data,
    *,
    key=None,
    reverse=False,
    strategy="auto",     # "auto" | "uniform" | "quantile"
    max_buckets=8192,
    min_buckets=16,
    bucket_target=1024,
    min_input_size=4096,
) -> list
```
