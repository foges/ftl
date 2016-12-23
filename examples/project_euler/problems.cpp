#include <algorithm>
#include <iostream>

#include <ftl/ftl.h>

template <typename T>
class range {
public:
  using value_type = T;

  range(T idx) : idx_(idx) { }
  auto operator*() const { return idx_; }
  range& operator++() { ++idx_; return *this; }
  bool operator==(const range<T> &rhs) const { return idx_ == rhs.idx_; }
  bool operator!=(const range<T> &rhs) const { return !(*this == rhs); }
private:
  T idx_;
};

template <typename T>
range<T> make_range(T val) { return range<T>(val); }

// Find the sum of all the multiples of 3 or 5 below 1000.
int problem1() {
  return ftl::make_seq(make_range(1), make_range(1000))
      .filter_lazy([](let x) { return x % 3 == 0 || x % 5 == 0; })
      .sum();
}

int main() {
  std::cout << "Problem 1: " << problem1() << std::endl;
}
