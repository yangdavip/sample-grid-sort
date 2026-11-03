#include <sample_grid_sort/sample_grid_sort.hpp>

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

int main() {
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> d(0, 999);

    // ---- 1. basic use ----
    std::vector<int> v(20);
    for (auto& x : v) x = d(rng);

    std::cout << "Before: ";
    for (int x : v) std::cout << x << ' ';
    std::cout << "\nAfter:  ";
    sgs::sort(v.begin(), v.end());
    for (int x : v) std::cout << x << ' ';
    std::cout << "\n\n";

    // ---- 2. custom compare ----
    std::vector<int> w(20);
    for (auto& x : w) x = d(rng);
    sgs::sort(w.begin(), w.end(), std::greater<>{});
    std::cout << "Descending: ";
    for (int x : w) std::cout << x << ' ';
    std::cout << "\n\n";

    // ---- 3. tune the config ----
    std::vector<double> vd(100000);
    std::normal_distribution<double> nd(0.0, 1.0);
    for (auto& x : vd) x = nd(rng);

    sgs::config cfg;
    cfg.strategy    = sgs::mode::quantile;
    cfg.max_buckets = 256;
    sgs::sort(vd.begin(), vd.end(), cfg);

    std::cout << "Doubles sorted? " << std::boolalpha
              << std::is_sorted(vd.begin(), vd.end()) << '\n';
    return 0;
}
