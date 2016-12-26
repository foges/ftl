#pragma once

namespace ftl {
namespace impl {

struct nil { };

template <typename T>
impl::nil operator<(const T&, const T&);

template <typename T>
impl::nil operator+(const T&, const T&);

template <typename T>
struct lt_exists {
  enum {
    value = !std::is_same<decltype(*(T*)(0) < *(T*)(0)), nil>::value
  };
};

template <typename T>
struct plus_exists {
  enum {
    value = !std::is_same<decltype(*(T*)(0) + *(T*)(0)), nil>::value
  };
};

template <typename T>
struct bool_exists {
  enum {
    value = std::is_constructible<T, bool>::value
  };
};



}  // namespace impl
}  // namespace ftl

