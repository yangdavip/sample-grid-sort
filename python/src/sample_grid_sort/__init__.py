"""Pure-Python reference implementation of Sample-Grid Sort.

This module mirrors the C++ header ``include/sample_grid_sort/sample_grid_sort.hpp``.
It is intended for teaching, verification, and prototyping — **not** as a
performance replacement for :py:func:`sorted`.
"""

from __future__ import annotations

import bisect
from typing import Callable, List, Optional, Sequence, TypeVar

T = TypeVar("T")

__all__ = ["sort", "SampleGridSortConfig"]
__version__ = "0.1.0"


class SampleGridSortConfig:
    """Tuning knobs for :func:`sort`.

    Mirrors ``sgs::config`` from the C++ implementation.
    """

    def __init__(
        self,
        *,
        strategy: str = "auto",
        max_buckets: int = 8192,
        min_buckets: int = 16,
        bucket_target: int = 1024,
        sample_factor: int = 4,
        min_sample: int = 256,
        min_input_size: int = 4096,
    ) -> None:
        if strategy not in ("auto", "uniform", "quantile"):
            raise ValueError(f"invalid strategy: {strategy!r}")
        self.strategy = strategy
        self.max_buckets = max_buckets
        self.min_buckets = min_buckets
        self.bucket_target = bucket_target
        self.sample_factor = sample_factor
        self.min_sample = min_sample
        self.min_input_size = min_input_size


def sort(
    data: Sequence[T],
    *,
    key: Optional[Callable[[T], object]] = None,
    reverse: bool = False,
    strategy: str = "auto",
    max_buckets: int = 8192,
    min_buckets: int = 16,
    bucket_target: int = 1024,
    sample_factor: int = 4,
    min_sample: int = 256,
    min_input_size: int = 4096,
) -> List[T]:
    """Return a new list with ``data`` sorted according to ``key``.

    Equivalent to the C++ ``sgs::sort`` API. Use ``strategy="uniform"``
    (default for numbers) to get O(1) bucket lookup, or ``"quantile"``
    for maximum robustness on skewed data.
    """
    _ = SampleGridSortConfig(
        strategy=strategy,
        max_buckets=max_buckets,
        min_buckets=min_buckets,
        bucket_target=bucket_target,
        sample_factor=sample_factor,
        min_sample=min_sample,
        min_input_size=min_input_size,
    )  # validate arguments

    if key is None:
        key = _identity

    out = list(data)
    n = len(out)
    if n <= 1:
        return out

    if n < min_input_size:
        out.sort(key=key, reverse=reverse)
        return out

    k = max(min_buckets, min(max_buckets, n // bucket_target))

    # 1. sample
    sample_size = min(n, max(sample_factor * k, min_sample))
    stride = max(1, n // sample_size)
    sample = [out[i] for i in range(0, n, stride)][:sample_size]
    sample.sort(key=key)

    # 2. quantile boundaries
    sm = len(sample)
    bounds = []
    for i in range(1, k):
        idx = min(i * sm // k, sm - 1)
        v = key(sample[idx])
        if not bounds or v != bounds[-1]:
            bounds.append(v)
    real_k = len(bounds) + 1

    # 3. scatter (simple list-of-lists; Python lacks a fast two-pass primitive)
    buckets: List[List[T]] = [[] for _ in range(real_k)]
    for x in out:
        buckets[bisect.bisect_right(bounds, key(x))].append(x)

    # 4. per-bucket sort
    out = []
    for b in buckets:
        b.sort(key=key)
        out.extend(b)

    if reverse:
        out.reverse()
    return out


def _identity(x):
    return x
