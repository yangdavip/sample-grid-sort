#include <sample_grid_sort/sample_grid_sort.hpp>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

namespace {

using Clock = std::chrono::high_resolution_clock;

template <typename Fn>
double time_ms(Fn fn, std::vector<int> data, int reps = 3) {
    double best = std::numeric_limits<double>::infinity();
    for (int r = 0; r < reps; ++r) {
        auto copy = data;
        auto t0 = Clock::now();
        fn(copy);
        auto t1 = Clock::now();
        double dt = std::chrono::duration<double, std::milli>(t1 - t0).count();
        best = std::min(best, dt);
    }
    return best;
}

struct Case { std::string name; std::vector<int> data; };

std::vector<Case> make_cases(std::size_t n, std::mt19937_64& rng) {
    std::vector<Case> cases;
    {
        std::vector<int> v(n);
        std::uniform_int_distribution<int> d(0, 1'000'000'000);
        for (auto& x : v) x = d(rng);
        cases.push_back({"uniform", std::move(v)});
    }
    {
        std::vector<int> v(n);
        std::normal_distribution<double> d(5e8, 1e8);
        for (auto& x : v) x = static_cast<int>(d(rng));
        cases.push_back({"normal", std::move(v)});
    }
    {
        std::vector<int> v(n);
        std::exponential_distribution<double> d(1.0 / 1e8);
        for (auto& x : v) x = static_cast<int>(d(rng));
        cases.push_back({"exponential", std::move(v)});
    }
    {
        std::vector<int> v(n);
        std::uniform_int_distribution<int> d(0, 99);
        for (auto& x : v) x = d(rng);
        cases.push_back({"few_duplicates", std::move(v)});
    }
    {
        std::vector<int> v(n);
        std::uniform_int_distribution<int> d(0, 1'000'000'000);
        for (auto& x : v) x = d(rng);
        std::sort(v.begin(), v.end());
        cases.push_back({"sorted", std::move(v)});
    }
    {
        std::vector<int> v(n);
        std::uniform_int_distribution<int> d(0, 1'000'000'000);
        for (auto& x : v) x = d(rng);
        std::sort(v.begin(), v.end());
        std::reverse(v.begin(), v.end());
        cases.push_back({"reverse", std::move(v)});
    }
    return cases;
}

}  // namespace

int main(int argc, char** argv) {
    std::size_t n = 1'000'000;
    if (argc > 1) n = static_cast<std::size_t>(std::stoull(argv[1]));

    std::cout << "=== Sample-Grid-Sort benchmark | n = " << n << " ===\n";
    std::cout << std::left
              << std::setw(18) << "case"
              << std::setw(14) << "std::sort"
              << std::setw(14) << "sgs::sort"
              << std::setw(12) << "speedup"
              << "ok?\n";
    std::cout << std::string(62, '-') << "\n";

    std::mt19937_64 rng(42);
    auto cases = make_cases(n, rng);

    for (auto& c : cases) {
        auto data = c.data;
        double t_std = time_ms([](auto& v) { std::sort(v.begin(), v.end()); }, data);
        double t_sgs = time_ms([](auto& v) { sgs::sort(v.begin(), v.end()); }, data);

        auto check = data;
        sgs::sort(check.begin(), check.end());
        bool ok = std::is_sorted(check.begin(), check.end());

        std::cout << std::left
                  << std::setw(18) << c.name
                  << std::setw(14) << std::fixed << std::setprecision(2) << t_std
                  << std::setw(14) << t_sgs
                  << std::setw(12) << std::setprecision(3) << (t_std / t_sgs)
                  << (ok ? "yes" : "NO") << "\n";
    }
    return 0;
}
