#include <assert.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <set>

#include <ftl/ftl.h>

#include "timestamp.h"

// -----------------------------------------------------------------------------
// -- Helper Functions
// -----------------------------------------------------------------------------
template <typename T>
T sqrt_int(T num) {
  return static_cast<T>(std::sqrt(num + 0.99));
}

uint64_t smallest_prime_factor(uint64_t num) {
  return ftl::range(2llu, sqrt_int(num) + 1)
      .filter([num](let p){ return (num % p) == 0; })
      .head()
      .value_or(num);
}

uint64_t largest_prime_factor(uint64_t num) {
  let p = smallest_prime_factor(num);
  return p == num ? p : largest_prime_factor(num / p);
}

uint64_t is_prime(uint64_t num) {
  return num == smallest_prime_factor(num);
};

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

// Find the sum of all the multiples of 3 or 5 below 1000.
int problem1() {
  return ftl::range(1000)
      .filter([](let x){ return x % 3 == 0 || x % 5 == 0; })
      .sum();
}

// By considering the terms in the Fibonacci sequence whose values do not exceed
// four million, find the sum of the even-valued terms.
int problem2() {
  return ftl::unfold(std::make_tuple(1, 1), [](let x){
          return std::make_tuple(std::get<1>(x),
                                 std::get<0>(x) + std::get<1>(x));
      })
      .map([](let x){ return std::get<0>(x); })
      .take_while([](let x){ return x < 4'000'000; })
      .filter([](let x){ return x % 2 == 0; })
      .sum();
}

// What is the largest prime factor of the number 600851475143
uint64_t problem3() {
  return largest_prime_factor(600851475143llu);
}

// Find the difference between the sum of the squares of the first one hundred
// natural numbers and the square of the sum.
uint64_t problem6() {
  let n = 100;
  let square = [](let x){ return x * x; };
  let sum_squares = ftl::range(1, n + 1).map(square).sum();
  let square_sum = square(ftl::range(1, n + 1).sum());
  return square_sum - sum_squares;
}

// What is the 10'001st prime number?
uint64_t problem7() {
  return ftl::iota(2llu).filter(is_prime).take(10'001).tail().value();
}

// There exists exactly one Pythagorean triplet for which a + b + c = 1000.
// Find the product abc.
uint32_t problem9() {
  let n = 1000;
  return ftl::range(1, n / 2).map([](let a){
      return ftl::range(1, a).map([a](let b){
          return std::make_tuple(a, b);
      });
  })
  .flat_map([n](let ab){
      return std::tuple_cat(ab,
          std::make_tuple(n - std::get<0>(ab) - std::get<1>(ab)));
  })
  .filter([](let abc) {
      let a = std::get<0>(abc);
      let b = std::get<1>(abc);
      let c = std::get<2>(abc);
      return a * a + b * b ==  c * c;
  })
  .map([](let abc){
      return std::get<0>(abc) * std::get<1>(abc) * std::get<2>(abc);
  })
  .head()
  .value();
}

// Find the sum of all the primes below two million.
uint64_t problem10() {
  return ftl::range(2, 2'000'000).filter(is_prime).sum<uint64_t>();
}

// What is the value of the first triangle number to have over five hundred
// divisors?
uint64_t problem12() {
  let num_divisors = [](let n) {
      let sqrtn = sqrt_int(n);
      return ftl::range(1, sqrtn)
          .filter([n](let i) { return n % i == 0; })
          .map([](let _) { return 1; })
          .sum() * 2 + (sqrtn * sqrtn == n ? 1 : 0);
  };
  return ftl::iota(1)
     .map([](let x) { return x * (x + 1) / 2; })
     .filter([&num_divisors](let x){ return num_divisors(x) > 500; })
     .head().value();
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
        .map([](let _){ return 1; })
        .sum();
  };
  return std::get<0>(ftl::range(1llu, 1'000'000llu)
      .map([chain_length](let i){
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
      let sqrtn = sqrt_int(n);
      let sum_divisors = ftl::range(1, sqrtn + 1)
          .filter([n](let i){ return n % i == 0; })
          .map([n](let i){ return i == 1 || i * i == n ? i : i + n / i; })
          .sum();
      return sum_divisors > n;
  };

  let is_abundant = ftl::memoize<decltype(is_abundant_impl), int>(
      is_abundant_impl);

  let abundants = ftl::range(1, max_num + 1).filter(is_abundant).eval();

  let is_sum_abundants = [&is_abundant, &abundants](let n){
      return abundants
          .filter([n](let k){ return k < n; })
          .filter([n, &is_abundant](let k){ return is_abundant(n - k); })
          .any();
  };

  return ftl::range(1, max_num + 1)
      .filter([&is_sum_abundants](let i){ return !is_sum_abundants(i); })
      .sum();
}

template <typename Func>
void do_run(const std::string &name, const Func &f) {
  uint64_t t_start = timestamp_ms();
  let res = f();
  uint64_t t_stop = timestamp_ms();
  std::cout << "| "
            << std::setw(10) << std::left << name << " | "
            << std::setw(12) << std::right << res << " | "
            << std::setw(5) << t_stop - t_start << "ms"
            << " |"
            << std::endl;
}

#define STRINGIFY(x) #x
#define TOSTRING(x)  STRINGIFY(x)
#define RUN(f) do_run(TOSTRING(f), (f))

int main() {
  const char* spacing = "---------------------------------------";
  const char* title =   "| Project Euler examples              |";

  std::cout << spacing << std::endl
            << title << std::endl
            << spacing << std::endl;

  RUN(problem1);
  RUN(problem2);
  RUN(problem3);
  RUN(problem6);
  RUN(problem7);
  RUN(problem9);
  RUN(problem10);
  RUN(problem12);
  RUN(problem14);
  RUN(problem23);

  std::cout << spacing << std::endl;
}

