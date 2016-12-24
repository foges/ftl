#pragma once

#include <utility>

#include <ftl/seq.h>

namespace ftl {
namespace impl {

template <typename Iter, typename... Iters>
class zip_iterator {
public:
  using value_type = std::tuple<const typename Iter::value_type&,
                                const typename Iters::value_type&...>;

  zip_iterator(const Iter &head, const Iters&... tail)
      : head_(head),
        tail_(tail...) { }

  value_type operator*() const {
    return std::tuple_cat(std::tuple<const typename Iter::value_type&>(*head_),
                          *tail_);
  }

  zip_iterator& operator++() {
    ++head_;
    ++tail_;
    return *this;
  }

  bool operator==(const zip_iterator &rhs) const {
    return head_ == rhs.head_ && tail_ == rhs.tail_;
  }

  bool operator!=(const zip_iterator &rhs) const {
    return !(*this == rhs);
  }

private:
  Iter head_;
  zip_iterator<Iters...> tail_;
};

template <typename Iter>
class zip_iterator<Iter> {
public:
  using value_type = std::tuple<const typename Iter::value_type&>;

  zip_iterator(const Iter &head) : head_(head) { }

  value_type operator*() const {
    return value_type(*head_);
  }

  zip_iterator& operator++() { ++head_; return *this; }

  bool operator==(const zip_iterator &rhs) const { return head_ == rhs.head_; }

  bool operator!=(const zip_iterator &rhs) const { return !(*this == rhs); }

private:
  Iter head_;
};

}  // namespace impl

/* WARNING: Undefined behavior if iterator lengths are different.
 */
template <typename... Seqs>
seq<impl::zip_iterator<typename Seqs::iterator...>>
zip(const Seqs&... seqs) {
  return make_seq(
      impl::zip_iterator<typename Seqs::iterator...>(std::begin(seqs)...),
      impl::zip_iterator<typename Seqs::iterator...>(std::end(seqs)...));
}

}  // namespace ftl

