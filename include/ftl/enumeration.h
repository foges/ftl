#pragma once

#include <ftl/seq.h>

namespace ftl {

template <typename Iter>
class enumeration {
public:
  class iterator {
  public:
    iterator(Iter it) : idx_(0), it_(it) { }

    std::tuple<size_t, const typename Iter::value_type&> operator*() const {
      return std::tuple<size_t, const typename Iter::value_type&>(idx_, *it_);
    }

    iterator& operator++() {
      ++it_; ++idx_;
      return *this;
    }

    bool operator==(const iterator &rhs) const {
      return it_ == rhs.it_;
    }

    bool operator!=(const iterator &rhs) const {
      return !(*this == rhs);
    }
  private:
    size_t idx_;
    Iter it_;
  };

  using const_iterator = iterator;

  enumeration(const Iter begin, const Iter end) : begin_(begin), end_(end) { }

  iterator begin()  const { return iterator(begin_); }
  iterator end()    const { return iterator(end_);   }
  iterator cbegin() const { return iterator(begin_); }
  iterator cend()   const { return iterator(end_);   }

private:
  const Iter begin_;
  const Iter end_;
};

template <typename Iter>
enumeration<Iter> enumerate(const Iter begin, const Iter end) {
  return enumeration<Iter>(begin, end);
}

template <typename Iter>
enumeration<Iter> enumerate(const seq<Iter> &sequence) {
  return enumerate(sequence.begin(), sequence.end());
}

}  // namespace ftl

