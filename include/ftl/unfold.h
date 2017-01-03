#pragma once

#include <ftl/optional.h>
#include <ftl/seq.h>

namespace ftl {

template <typename T, typename Func>
auto unfold(const T &init, const Func &f) {
  auto lambda = [init, f](const auto &f_next){
    auto acc = ftl::make_optional(init);
    while (acc && f_next(*acc)) {
      acc = f(*acc);
    }
  };
  return seq<decltype(lambda), T>(lambda);
}

// Convenience methods
template <typename T>
auto iota(const T& begin, const T& incr=T(1)) {
  return unfold(begin, [incr](const auto x){ return x + incr; });
}

template <typename T>
auto range(const T& begin, const T& end, const T& incr=T(1)) {
  return unfold(begin, [incr, end](const auto x){
    return x + incr < end ? optional<T>(x + incr) : optional<T>();
  });
}

template <typename T>
auto range(const T& end) {
  return range(T(0), end);
}

}  // namespace flt

