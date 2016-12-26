#include <assert.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>

#include <ftl/ftl.h>

#include "timestamp.h"

// Find the sum of all the multiples of 3 or 5 below 1000.
int problem1() {
  return ftl::range(1000)
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

// Find the difference between the sum of the squares of the first one hundred
// natural numbers and the square of the sum.
uint64_t problem6() {
  let n = 100;
  let square = [](let x){ return x * x; };
  let sum_squares = ftl::range(1, n + 1).map_lazy(square).sum();
  let square_sum = square(ftl::range(1, n + 1).sum());
  return square_sum - sum_squares;
}

// What is the value of the first triangle number to have over five hundred
// divisors?
uint64_t problem12() {
  let num_divisors = [](let n) {
      let sqrtn = static_cast<uint64_t>(std::sqrt(n + 0.99));
      return ftl::range(1llu, sqrtn)
          .filter_lazy([n](let i) { return n % i == 0; })
          .map_lazy([](let _) { return 1llu; })
          .sum() * 2 + (sqrtn * sqrtn == 1 ? 1 : 0);
  };
  return ftl::iota(1llu)
     .map_lazy([](let x) { return x * (x + 1) / 2; })
     .filter_lazy([&num_divisors](let x){ return num_divisors(x) > 500llu; })
     .head();
}

uint64_t problem14() {
  let chain_length = [](let n){
    return ftl::unfold(n,
        [](let i){
            if (i == 1) {
              return ftl::optional<uint64_t>();
            } else if (i % 2 == 0) {
              return ftl::make_optional(i / 2);
            } else {
              return ftl::make_optional(3 * i + 1);
            }
        })
        .map_lazy([](let _){ return 1; })
        .sum();
  };
  return std::get<0>(ftl::range(1llu, 1'000'000llu)
      .map_lazy([chain_length](let i){
        return std::make_tuple(i, chain_length(i));
      })
      .reduce(std::make_tuple(0llu, 0), [](let acc, let val){
        if (std::get<1>(acc) < std::get<1>(val)) {
          return val;
        } else {
          return acc;
        }
      }));
}

// Find the sum of all the positive integers which cannot be written as the sum
// of two abundant numbers.
uint64_t problem23() {
  let max_num = 28123;

  let is_abundant_impl = [](let n){
      let sqrtn = static_cast<int>(std::sqrt(n + 0.99));
      let sum_divisors = ftl::range(1, sqrtn + 1)
          .filter_lazy([n](let i){ return n % i == 0; })
          .map_lazy([n](let i){ return i == 1 || i * i == n ? i : i + n / i; })
          .sum();
      return sum_divisors > n;
  };

  let is_abundant = ftl::memoize<decltype(is_abundant_impl), int>(
      is_abundant_impl);

  let abundants = ftl::range(1, max_num + 1).filter(is_abundant);

  let is_sum_abundants = [&is_abundant, &abundants](let n){
      return abundants
          .filter_lazy([n](let k){ return k < n; })
          .filter_lazy([n, &is_abundant](let k){ return is_abundant(n - k); })
          .any();
  };

  return ftl::range(1, max_num + 1)
      .filter_lazy([&is_sum_abundants](let i){ return !is_sum_abundants(i); })
      .sum();
}

template <typename Func>
void run(int num, const Func &f) {
  uint64_t t_start = timestamp_ms();
  let res = f();
  uint64_t t_stop = timestamp_ms();
  std::cout << "Problem " << std::setw(3) << num << " | "
            << std::setw(10) << res << " | "
            << std::setw(5) << t_stop - t_start << "ms"
            << std::endl;
}

int main() {
  const char* spacing = "----------------------------------";

  std::cout << spacing << std::endl
            << "| Project Euler examples         |" << std::endl
            << spacing << std::endl;

  run(1,  problem1);
  run(2,  problem2);
  run(3,  problem3);
  run(6,  problem6);
  run(12, problem12);
  run(14, problem14);
  run(23, problem23);

  std::cout << spacing << std::endl;
}

