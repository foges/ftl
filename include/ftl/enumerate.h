#pragma once

#include <ftl/seq.h>

namespace ftl {
namespace impl {

template <typename Iter>
class enum_iterator {
public:
  using value_type = std::tuple<size_t, const typename Iter::value_type&>;

  enum_iterator(const Iter &it) : idx_(0), it_(it) { }

  value_type operator*() const {
    return value_type(idx_, *it_);
  }

  enum_iterator& operator++() {
    ++it_;
    ++idx_;
    return *this;
  }

  bool operator==(const enum_iterator<Iter> &rhs) const {
    return it_ == rhs.it_;
  }

  bool operator!=(const enum_iterator<Iter> &rhs) const {
    return !(*this == rhs);
  }

private:
  size_t idx_;
  Iter it_;
};

}  // namespace impl

template <typename Iter>
auto enumerate(const seq<Iter> &sequence) {
  return make_seq(
      impl::enum_iterator<typename seq<Iter>::iterator>(sequence.begin()),
      impl::enum_iterator<typename seq<Iter>::iterator>(sequence.end()));
}

}  // namespace ftl

