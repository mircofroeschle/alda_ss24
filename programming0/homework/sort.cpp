#include "fstream"
#include "list.hpp"
#include <algorithm>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

std::vector<std::pair<size_t, uint64_t>>
count_number_of_compares(size_t min_n, size_t max_n, uint64_t repeats) {
  std::vector<std::pair<size_t, uint64_t>> results;

  std::mt19937_64 gen(0x123456789);

  for (size_t n = min_n; n <= max_n; n *= 2) {
    std::cout << "n=" << n << std::endl;

    std::vector<int> values(n);
    std::iota(values.begin(), values.end(), 0);

    for (size_t rep = 0; rep < repeats; ++rep) {
      std::shuffle(values.begin(), values.end(), gen);

      List list;
      for (auto &&x : values) {
        list.push_back(x);
      }

      const auto compares = list.sort();
      if (!list.is_sorted()) {
        std::cout << "Liste ist nicht sortiert\n";
        return {};
      }

      results.emplace_back(n, compares);
    }
  }

  return results;
}

int main() {
  constexpr size_t min_n = 1 << 5;
  constexpr size_t max_n = 1 << 20;
  constexpr size_t repeats = 30;

  const auto results = count_number_of_compares(min_n, max_n, repeats);

  std::ofstream output;
  output.open("compares.csv");
  output << "num_items,num_compares\n";

  for (auto x : results) {
    output << x.first << "," << x.second << "\n";
  }

  return 0;
}
