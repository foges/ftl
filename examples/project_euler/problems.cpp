#include <assert.h>
#include <algorithm>
#include <iostream>

#include <ftl/ftl.h>

// Find the sum of all the multiples of 3 or 5 below 1000.
int problem1() {
  return ftl::iota(0)
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

// What is the largest prime factor of the number 600851475143>
uint64_t smallest_prime_factor(int64_t num) {
  return ftl::iota(2llu)
      .filter_lazy([num](let p){ return (num % p) == 0; })
      .head();
}

uint64_t largest_prime_factor(uint64_t num) {
  let p = smallest_prime_factor(num);
  return p == num ? p : largest_prime_factor(num / p);
}

uint64_t problem3() {
  return largest_prime_factor(600851475143llu);
}

int main() {
  std::cout << "Problem 1: " << problem1() << std::endl;
  std::cout << "Problem 2: " << problem2() << std::endl;
  std::cout << "Problem 3: " << problem3() << std::endl;
}
