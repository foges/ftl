#include <algorithm>
#include <iostream>

#include <ftl/ftl.h>

// Find the sum of all the multiples of 3 or 5 below 1000.
int problem1() {
  return ftl::unfold(0, [](let x){ return x + 1; })
      .take_while([](let x){ return x < 1000; })
      .filter_lazy([](let x){ return x % 3 == 0 || x % 5 == 0; })
      .sum();
}

// By considering the terms in the Fibonacci sequence whose values do not exceed
// four million, find the sum of the even-valued terms.
int problem2() {
  return ftl::unfold(std::make_tuple(1, 1), [](let x){
          return std::make_tuple(std::get<1>(x),
                                 std::get<0>(x) + std::get<1>(x));
      })
      .map_lazy([](let x){ return std::get<0>(x); })
      .take_while([](let x){ return x < 4'000'000; })
      .filter_lazy([](let x){ return x % 2 == 0; })
      .sum();
}

int main() {
  std::cout << "Problem 1: " << problem1() << std::endl;
  std::cout << "Problem 2: " << problem2() << std::endl;
}
