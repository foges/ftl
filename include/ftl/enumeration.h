#pragma once

#include <ftl/seq.h>

namespace ftl {

template <typename Iter>
class enumeration {
public:
  class iterator {
  public:
    iterator(const Iter &it) : idx_(0), it_(it) { }

    std::tuple<size_t, const typename Iter::value_type&> operator*() const {
      return std::tuple<size_t, const typename Iter::value_type&>(idx_, *it_);
    }

    iterator& operator++() {
      ++it_;
      ++idx_;
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

  enumeration(const Iter &begin, const Iter &end)
      : begin_(iterator(begin)),
        end_(iterator(end)) { }

  iterator begin()  const { return begin_; }
  iterator end()    const { return end_;   }

private:
  iterator begin_;
  iterator end_;
};

template <typename Iter>
enumeration<Iter> enumerate(const seq<Iter> &sequence) {
  return enumeration<Iter>(sequence.begin(), sequence.end());
}

}  // namespace ftl

