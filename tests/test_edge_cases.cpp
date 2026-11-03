#include <sample_grid_sort/sample_grid_sort.hpp>

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

namespace {

int g_checks = 0;
int g_failures = 0;

#define CHECK(cond)                                                       \
    do {                                                                  \
        ++g_checks;                                                       \
        if (!(cond)) {                                                    \
            ++g_failures;                                                 \
            std::cerr << "FAIL " #cond " @ " << __FILE__ << ":"           \
                      << __LINE__ << "\n";                                \
        }                                                                 \
    } while (0)

void test_empty() {
    std::vector<int> v;
    sgs::sort(v.begin(), v.end());
    CHECK(v.empty());
}

void test_one_element() {
    std::vector<int> v{42};
    sgs::sort(v.begin(), v.end());
    CHECK(v == std::vector<int>{42});
}

void test_two_elements() {
    {
        std::vector<int> v{2, 1};
        sgs::sort(v.begin(), v.end());
        CHECK((v == std::vector<int>{1, 2}));
    }
    {
        std::vector<int> v{1, 2};
        sgs::sort(v.begin(), v.end());
        CHECK((v == std::vector<int>{1, 2}));
    }
    {
        std::vector<int> v{5, 5};
        sgs::sort(v.begin(), v.end());
        CHECK((v == std::vector<int>{5, 5}));
    }
}

void test_all_equal() {
    std::vector<int> v(100000, 7);
    sgs::sort(v.begin(), v.end());
    CHECK(std::all_of(v.begin(), v.end(), [](int x) { return x == 7; }));
}

void test_extreme_int64() {
    std::vector<std::int64_t> v{
        std::numeric_limits<std::int64_t>::max(),
        std::numeric_limits<std::int64_t>::min(),
        0, 1, -1,
        std::numeric_limits<std::int64_t>::max() - 1,
        std::numeric_limits<std::int64_t>::min() + 1,
    };
    auto expected = v;
    std::sort(expected.begin(), expected.end());

    // quantile mode is exact for any range
    sgs::config cfg;
    cfg.strategy = sgs::mode::quantile;
    cfg.min_input_size = 1;   // force algorithm even for small input
    sgs::sort(v.begin(), v.end(), cfg);
    CHECK(v == expected);
}

void test_extreme_floats() {
    std::vector<float> v{
        std::numeric_limits<float>::lowest(),
        std::numeric_limits<float>::max(),
        -1.0f, 0.0f, 1.0f,
        -std::numeric_limits<float>::infinity(),
        std::numeric_limits<float>::infinity(),
    };
    auto expected = v;
    std::sort(expected.begin(), expected.end());

    sgs::config cfg;
    cfg.strategy = sgs::mode::quantile;
    cfg.min_input_size = 1;
    sgs::sort(v.begin(), v.end(), cfg);
    CHECK(v == expected);
}

void test_below_threshold_fallback() {
    // n < min_input_size -> std::sort
    std::mt19937 rng(1);
    std::vector<int> v(100);
    for (auto& x : v) x = static_cast<int>(rng() % 1000);
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    sgs::sort(v.begin(), v.end());
    CHECK(v == expected);
}

void test_negative_range() {
    std::mt19937_64 rng(5);
    std::uniform_int_distribution<int> d(-1000000, -1);
    std::vector<int> v(50000);
    for (auto& x : v) x = d(rng);
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    sgs::sort(v.begin(), v.end());
    CHECK(v == expected);
}

void test_large_input() {
    std::mt19937_64 rng(123);
    std::uniform_int_distribution<int> d(0, 1'000'000);
    std::vector<int> v(1'000'000);
    for (auto& x : v) x = d(rng);
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    sgs::sort(v.begin(), v.end());
    CHECK(v == expected);
}

}  // namespace

int main() {
    test_empty();
    test_one_element();
    test_two_elements();
    test_all_equal();
    test_extreme_int64();
    test_extreme_floats();
    test_below_threshold_fallback();
    test_negative_range();
    test_large_input();

    std::cout << "checks: " << (g_checks - g_failures) << "/" << g_checks << "\n";
    if (g_failures) { std::cerr << "FAILURES: " << g_failures << "\n"; return 1; }
    std::cout << "all edge-case tests passed\n";
    return 0;
}
