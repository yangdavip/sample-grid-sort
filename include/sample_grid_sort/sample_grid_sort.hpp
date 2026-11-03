// Sample-Grid-Sort: a sample-driven bucket sort.
//
// SPDX-License-Identifier: MIT
//
#ifndef SAMPLE_GRID_SORT_HPP
#define SAMPLE_GRID_SORT_HPP

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>
#include <vector>

namespace sgs {

/// Bucket-selection strategy.
enum class mode {
    /// Auto-select: uniform for arithmetic types, quantile otherwise.
    auto_detect,
    /// Equal-width buckets. O(1) bucket lookup per element.
    /// Requires an arithmetic value type.
    uniform,
    /// Quantile-based buckets. O(log k) bucket lookup per element.
    /// Works with any totally-ordered type.
    quantile,
};

/// Tunable parameters for `sgs::sort`.
struct config {
    mode strategy            = mode::auto_detect;
    std::size_t max_buckets  = 8192;
    std::size_t min_buckets  = 16;
    std::size_t bucket_target = 1024;   ///< target avg bucket size
    std::size_t sample_factor = 4;      ///< m = sample_factor * k
    std::size_t min_sample    = 256;
    std::size_t min_input_size = 4096;  ///< below this, defer to std::sort
};

namespace detail {

// -------------------------------------------------------------
//  Equal-width bucket sort (arithmetic types only)
// -------------------------------------------------------------
template <typename RandomIt, typename Compare>
void sort_uniform(RandomIt first, RandomIt last, Compare comp, std::size_t k) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    const std::size_t n = static_cast<std::size_t>(std::distance(first, last));
    if (n <= 1) return;

    auto mm = std::minmax_element(first, last, comp);
    const T& mn = *mm.first;
    const T& mx = *mm.second;
    if (!comp(mn, mx) && !comp(mx, mn)) return;  // all equal

    const long double lo    = static_cast<long double>(mn);
    const long double hi    = static_cast<long double>(mx);
    const long double inv_w = static_cast<long double>(k) / (hi - lo);

    auto bucket_of = [&](const T& x) -> std::size_t {
        long double p = (static_cast<long double>(x) - lo) * inv_w;
        if (p < 0.0L) return 0;
        if (p >= static_cast<long double>(k)) return k - 1;
        return static_cast<std::size_t>(p);
    };

    std::vector<std::size_t> cnt(k, 0);
    for (auto it = first; it != last; ++it) ++cnt[bucket_of(*it)];

    std::vector<std::size_t> off(k);
    std::size_t acc = 0;
    for (std::size_t i = 0; i < k; ++i) { off[i] = acc; acc += cnt[i]; }

    std::vector<std::size_t> cursor = off;
    std::vector<T> buf(n);
    for (auto it = first; it != last; ++it) {
        buf[cursor[bucket_of(*it)]++] = std::move(*it);
    }

    for (std::size_t i = 0; i < k; ++i) {
        if (cnt[i] > 1) {
            std::sort(buf.begin() + static_cast<std::ptrdiff_t>(off[i]),
                      buf.begin() + static_cast<std::ptrdiff_t>(off[i] + cnt[i]),
                      comp);
        }
    }
    std::move(buf.begin(), buf.end(), first);
}

// -------------------------------------------------------------
//  Quantile-based bucket sort (any totally-ordered type)
// -------------------------------------------------------------
template <typename RandomIt, typename Compare>
void sort_quantile(RandomIt first, RandomIt last, Compare comp, std::size_t k) {
    using T = typename std::iterator_traits<RandomIt>::value_type;
    const std::size_t n = static_cast<std::size_t>(std::distance(first, last));
    if (n <= 1) return;

    // 1. sample
    const std::size_t m      = std::min<std::size_t>(n, std::max<std::size_t>(4 * k, 256));
    const std::size_t stride = std::max<std::size_t>(1, n / m);

    std::vector<T> sample;
    sample.reserve(m);
    for (std::size_t i = 0; i < n && sample.size() < m; i += stride) {
        sample.push_back(*(first + static_cast<std::ptrdiff_t>(i)));
    }
    std::sort(sample.begin(), sample.end(), comp);

    // 2. quantile boundaries (deduplicated)
    const std::size_t sm = sample.size();
    std::vector<T> bounds;
    bounds.reserve(k - 1);
    for (std::size_t i = 1; i < k; ++i) {
        std::size_t idx = std::min<std::size_t>(i * sm / k, sm - 1);
        const T& v = sample[idx];
        if (bounds.empty() ||
            comp(bounds.back(), v) || comp(v, bounds.back())) {
            bounds.push_back(v);
        }
    }
    const std::size_t real_k = bounds.size() + 1;

    // 3. scatter (two-pass, contiguous buffer)
    std::vector<std::size_t> cnt(real_k, 0);
    for (auto it = first; it != last; ++it) {
        std::size_t p = static_cast<std::size_t>(
            std::upper_bound(bounds.begin(), bounds.end(), *it, comp) - bounds.begin());
        ++cnt[p];
    }

    std::vector<std::size_t> off(real_k);
    std::size_t acc = 0;
    for (std::size_t i = 0; i < real_k; ++i) { off[i] = acc; acc += cnt[i]; }

    std::vector<std::size_t> cursor = off;
    std::vector<T> buf(n);
    for (auto it = first; it != last; ++it) {
        std::size_t p = static_cast<std::size_t>(
            std::upper_bound(bounds.begin(), bounds.end(), *it, comp) - bounds.begin());
        buf[cursor[p]++] = std::move(*it);
    }

    // 4. per-bucket sort
    for (std::size_t i = 0; i < real_k; ++i) {
        if (cnt[i] > 1) {
            std::sort(buf.begin() + static_cast<std::ptrdiff_t>(off[i]),
                      buf.begin() + static_cast<std::ptrdiff_t>(off[i] + cnt[i]),
                      comp);
        }
    }
    std::move(buf.begin(), buf.end(), first);
}

}  // namespace detail

// -------------------------------------------------------------
//  Public API
// -------------------------------------------------------------

/// Sort [first, last) with `comp`, using Sample-Grid-Sort.
template <typename RandomIt, typename Compare,
          typename = std::enable_if_t<
              !std::is_same_v<std::decay_t<Compare>, config>>>
void sort(RandomIt first, RandomIt last, Compare comp, config cfg = {}) {
    const std::size_t n = static_cast<std::size_t>(std::distance(first, last));
    if (n <= 1) return;
    if (n < cfg.min_input_size) {
        std::sort(first, last, comp);
        return;
    }

    const std::size_t k = std::clamp<std::size_t>(
        n / cfg.bucket_target, cfg.min_buckets, cfg.max_buckets);

    using T = typename std::iterator_traits<RandomIt>::value_type;

    if constexpr (std::is_arithmetic_v<T>) {
        if (cfg.strategy == mode::uniform || cfg.strategy == mode::auto_detect) {
            detail::sort_uniform(first, last, comp, k);
            return;
        }
    }
    (void)cfg.strategy;
    detail::sort_quantile(first, last, comp, k);
}

/// Sort [first, last) with the natural ordering.
template <typename RandomIt>
void sort(RandomIt first, RandomIt last, config cfg = {}) {
    sgs::sort(first, last, std::less<>{}, cfg);
}

}  // namespace sgs

#endif  // SAMPLE_GRID_SORT_HPP
