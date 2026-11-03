#include <sample_grid_sort/sample_grid_sort.hpp>

#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
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

template <typename T, typename Gen>
void check_sorted(Gen gen, const char* label, std::size_t n, sgs::config cfg = {}) {
    std::vector<T> data(n);
    for (auto& x : data) x = gen();
    std::vector<T> expected = data;
    std::sort(expected.begin(), expected.end());
    sgs::sort(data.begin(), data.end(), cfg);
    CHECK(data == expected);
    if (data != expected) {
        std::cerr << "  label=" << label << " n=" << n << "\n";
    }
}

void test_int_distributions() {
    std::mt19937_64 rng(42);
    auto uniform_gen = [&] {
        std::uniform_int_distribution<int> d(-1'000'000, 1'000'000);
        return d(rng);
    };
    auto normal_gen = [&] {
        std::normal_distribution<double> d(0.0, 1e6);
        return static_cast<int>(d(rng));
    };
    auto exponential_gen = [&] {
        std::exponential_distribution<double> d(1.0 / 1e6);
        return static_cast<int>(d(rng));
    };
    auto small_dup_gen = [&] { return static_cast<int>(rng() % 100); };

    for (std::size_t n : {0, 1, 2, 5, 100, 4095, 4096, 10000, 100000}) {
        check_sorted<int>(uniform_gen,     "uniform",     n);
        check_sorted<int>(normal_gen,      "normal",      n);
        check_sorted<int>(exponential_gen, "exponential", n);
        check_sorted<int>(small_dup_gen,   "small_dup",   n);
    }
}

void test_already_sorted() {
    std::vector<int> v(100000);
    std::iota(v.begin(), v.end(), 0);
    auto expected = v;
    sgs::sort(v.begin(), v.end());
    CHECK(v == expected);
}

void test_reverse_sorted() {
    std::vector<int> v(100000);
    std::iota(v.begin(), v.end(), 0);
    std::reverse(v.begin(), v.end());
    auto expected = v;
    std::sort(expected.begin(), expected.end());
    sgs::sort(v.begin(), v.end());
    CHECK(v == expected);
}

void test_custom_compare() {
    std::mt19937_64 rng(7);
    std::uniform_int_distribution<int> d(-1000, 1000);
    std::vector<int> v(20000);
    for (auto& x : v) x = d(rng);
    auto expected = v;
    std::sort(expected.begin(), expected.end(), std::greater<>{});
    sgs::sort(v.begin(), v.end(), std::greater<>{});
    CHECK(v == expected);
}

void test_strings() {
    std::mt19937_64 rng(11);
    auto gen = [&] {
        std::string s;
        int len = 1 + static_cast<int>(rng() % 8);
        for (int i = 0; i < len; ++i) s.push_back(static_cast<char>('a' + rng() % 26));
        return s;
    };
    for (std::size_t n : {0, 10, 4095, 10000, 50000}) {
        std::vector<std::string> v(n);
        for (auto& s : v) s = gen();
        auto expected = v;
        std::sort(expected.begin(), expected.end());
        sgs::sort(v.begin(), v.end());
        CHECK(v == expected);
    }
}

void test_quantile_forced_on_ints() {
    std::mt19937_64 rng(99);
    std::uniform_int_distribution<int> d(-100000, 100000);
    std::vector<int> v(50000);
    for (auto& x : v) x = d(rng);
    auto expected = v;
    std::sort(expected.begin(), expected.end());

    sgs::config cfg;
    cfg.strategy = sgs::mode::quantile;
    sgs::sort(v.begin(), v.end(), cfg);
    CHECK(v == expected);
}

}  // namespace

int main() {
    test_int_distributions();
    test_already_sorted();
    test_reverse_sorted();
    test_custom_compare();
    test_strings();
    test_quantile_forced_on_ints();

    std::cout << "checks: " << (g_checks - g_failures) << "/" << g_checks << "\n";
    if (g_failures) { std::cerr << "FAILURES: " << g_failures << "\n"; return 1; }
    std::cout << "all correctness tests passed\n";
    return 0;
}
