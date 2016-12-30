#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <ftl/optional.h>
#include <ftl/utils.h>

namespace ftl {
namespace impl {

template <typename Iter, typename Func>
class map_iterator {
public:
  using value_type =
      typename std::result_of<Func(typename Iter::value_type)>::type;

  map_iterator(Iter it, Func f) : it_(it), f_(f) { }
  auto operator*() const { return f_(*it_); }
  map_iterator& operator++() { ++it_; return *this; }
  bool operator==(const map_iterator &rhs) const { return it_ == rhs.it_; }
  bool operator!=(const map_iterator &rhs) const { return !(*this == rhs); }
private:
  Iter it_;
  Func f_;
};

template <typename Iter, typename Func>
class flat_map_iterator {
public:
  using iterator_type = typename Iter::value_type::iterator;
  using value_type =
      typename std::result_of<Func(typename iterator_type::value_type)>::type;

  flat_map_iterator(const Iter &it, const Iter &end, const Func &f)
      : it_(it),
        end_(end),
        it_inner_(it_ == end_ ?
            ftl::optional<iterator_type>() :
            ftl::optional<iterator_type>((*it_).begin())),
        f_(f) {
    make_valid_();
  }

  value_type operator*() const {
     return f_(**it_inner_);
  }

  flat_map_iterator& operator++() {
    ++(*it_inner_);
    make_valid_();
    return *this;
  }

  bool operator==(const flat_map_iterator &rhs) const {
    return it_ == rhs.it_;
  }

  bool operator!=(const flat_map_iterator &rhs) const {
    return !(*this == rhs);
  }

private:
  void make_valid_() {
    while (it_ != end_ && *it_inner_ == (*it_).end()) {
      ++it_;
      if (it_ != end_) {
        (*it_inner_).~iterator_type();
        new (&(*it_inner_)) iterator_type((*it_).begin());
      }
    }
  }

  Iter it_;
  Iter end_;
  ftl::optional<iterator_type> it_inner_;
  Func f_;
};

template <typename Iter, typename Func>
class filter_iterator {
public:
  using value_type = typename Iter::value_type;

  filter_iterator(Iter it, Iter end, Func f) : it_(it), end_(end), f_(f) {
    make_valid_();
  }
  auto operator*() const { return *it_; }
  filter_iterator& operator++() { ++it_; make_valid_(); return *this; }
  bool operator==(const filter_iterator &rhs) const { return it_ == rhs.it_; }
  bool operator!=(const filter_iterator &rhs) const { return !(*this == rhs); }
private:
  void make_valid_() { while (it_ != end_ && !f_(*it_)) { ++it_; } }
  Iter it_;
  Iter end_;
  Func f_;
};

template <typename Iter, typename Res>
class split_iterator {
public:
  using value_type = Res;

  split_iterator(Iter it, Iter end, const typename Iter::value_type &separator)
      : next_it_(it), it_(it), end_(end), separator_(separator) {
    make_valid_();
  }
  auto operator*() const { return value_type(it_, next_it_); }
  split_iterator& operator++() {
    if (next_it_ != end_) {
      ++next_it_;
    }
    it_ = next_it_;
    make_valid_();
    return *this;
  }
  bool operator==(const split_iterator &rhs) const { return it_ == rhs.it_; }
  bool operator!=(const split_iterator &rhs) const { return !(*this == rhs); }
private:
  void make_valid_() {
    while (next_it_ != end_ && *next_it_ != separator_) { ++next_it_; }
  }
  Iter next_it_;
  Iter it_;
  Iter end_;
  typename Iter::value_type separator_;
};

template <typename Iter, typename Func>
class take_while_iterator {
public:
  using value_type = typename Iter::value_type;

  take_while_iterator(Iter it, bool is_end, const Func &f)
      : it_(it), is_end_(is_end), f_(f) { }
  auto operator*() const { return *it_; }
  take_while_iterator& operator++() {
    ++it_;
    is_end_ = is_end_ || !f_(*it_);
    return *this;
  }
  bool operator==(const take_while_iterator &rhs) const {
    return it_ == rhs.it_ || (is_end_ && rhs.is_end_);
  }
  bool operator!=(const take_while_iterator &rhs) const {
    return !(*this == rhs);
  }
private:
  Iter it_;
  bool is_end_;
  Func f_;
};

template <typename Iter>
class take_iterator {
public:
  using value_type = typename Iter::value_type;

  take_iterator(Iter it, size_t idx) : it_(it), idx_(idx) { }
  auto operator*() const { return *it_; }
  take_iterator& operator++() {
    ++it_; ++idx_;
    return *this;
  }
  bool operator==(const take_iterator &rhs) const {
    return it_ == rhs.it_ || idx_ == rhs.idx_;
  }
  bool operator!=(const take_iterator &rhs) const {
    return !(*this == rhs);
  }
private:
  Iter it_;
  size_t idx_;
};


}  // namespace impl

template <typename Iter, typename Data=std::vector<typename Iter::value_type>>
class seq {
public:
  using value_type = typename Iter::value_type;
  using iterator = Iter;

  seq(const Iter begin,
      const Iter end,
      const std::shared_ptr<const Data> &data)
    : begin_(begin),
      end_(end),
      data_(data) { }

  seq(const Iter begin, const Iter end) : seq(begin, end, nullptr) { }

  bool empty() const { return begin_ == end_; }

  const Iter begin()  const { return begin_; }
  const Iter end()    const { return end_;   }

  auto eval() {
    auto res = std::make_shared<std::vector<decltype(*begin_)>>();
    for (const auto &val : *this) {
      res->emplace_back(val);
    }

    return seq<decltype(res->begin())>(res->begin(), res->end(), res);
  }

  template <typename Func>
  auto flat_map(Func f) const {
    return seq<impl::flat_map_iterator<Iter, Func>, Data>(
        impl::flat_map_iterator<Iter, Func>(begin_, end_, f),
        impl::flat_map_iterator<Iter, Func>(end_, end_, f), data_);
  }

  template <typename Func>
  auto map(Func f) const {
    return seq<impl::map_iterator<Iter, Func>, Data>(
        impl::map_iterator<Iter, Func>(begin_, f),
        impl::map_iterator<Iter, Func>(end_, f), data_);
  }

  template <typename T, typename Func>
  T reduce(T init, Func f) const {
    for (const auto &val : *this) {
      init = f(init, val);
    }
    return init;
  }

  template <typename Func>
  auto max(Func cmp) const {
    ftl::optional<value_type> max_val;
    for (auto it = begin_; it != end_; ++it) {
      if (!max_val || cmp(*max_val, *it)) {
        max_val = *it;
      }
    }
    return max_val;
  }

  template <typename T=value_type>
  typename std::enable_if<impl::lt_exists<T>::value, ftl::optional<T>>::type
  max() const {
    return max([](const T &x, const T &y){ return x < y; });
  }

  template <typename T=value_type>
  typename std::enable_if<impl::plus_exists<T>::value, T>::type
  sum() const {
    return reduce(T(), [](const T &acc, const value_type &x){
        return acc + static_cast<T>(x);
    });
  }

  template <typename Func>
  auto filter(Func f) const {
    return seq<impl::filter_iterator<Iter, Func>, Data>(
        impl::filter_iterator<Iter, Func>(begin_, end_, f),
        impl::filter_iterator<Iter, Func>(end_, end_, f), data_);
  }

  template <typename Func>
  auto sorted(Func cmp) const {
    auto res = std::make_shared<std::vector<typename Iter::value_type>>();
    *res = std::vector<typename Iter::value_type>(begin_, end_);
    std::sort(res->begin(), res->end(), cmp);
    return seq<Iter>(res->begin(), res->end(), res);
  }

  template <typename T=Iter>
  typename std::enable_if<impl::lt_exists<typename T::value_type>::value,
                          seq<Iter>>::type
  sorted() const {
    return sorted([](const auto &x, const auto &y) { return x < y; });
  }

  template <typename Res=std::vector<typename Iter::value_type>>
  auto split(const typename Iter::value_type &separator) const {
    return seq<impl::split_iterator<Iter, Res>, Data>(
        impl::split_iterator<Iter, Res>(begin_, end_, separator),
        impl::split_iterator<Iter, Res>(end_, end_, separator), data_);
  }

  template <typename Func>
  auto take_while(const Func &f) {
    return seq<impl::take_while_iterator<Iter, Func>, Data>(
        impl::take_while_iterator<Iter, Func>(begin_, false, f),
        impl::take_while_iterator<Iter, Func>(end_, true, f), data_);
  }

  auto take(size_t num) {
    return seq<impl::take_iterator<Iter>, Data>(
        impl::take_iterator<Iter>(begin_, 0),
        impl::take_iterator<Iter>(end_, num), data_);
  }

  ftl::optional<value_type> head() const {
    if (!empty()) {
      return *begin_;
    } else {
      return ftl::optional<value_type>();
    }
  }

  ftl::optional<value_type> tail() const {
    for (auto it = begin_;;) {
      Iter last_it(it);
      ++it;
      if (it == end_) {
          return *last_it;
      }
    }
    return ftl::optional<value_type>();
  }

  template <typename Func>
  bool any(const Func &f) const {
    for (const auto &val : *this) {
      if (f(val)) {
        return true;
      }
    }
    return false;
  }

  template <typename T=typename Iter::value_type>
  typename std::enable_if<impl::bool_exists<T>::value, bool>::type
  any() const {
    return any([](const auto &x) { return static_cast<bool>(x); });
  }

  template <typename Func>
  bool all(const Func &f) const {
    for (const auto &val : *this) {
      if (!f(val)) {
        return false;
      }
    }
    return true;
  }

  template <typename T=typename Iter::value_type>
  typename std::enable_if<impl::bool_exists<T>::value, bool>::type
  all() const {
    return all([](const auto &x) { return static_cast<bool>(x); });
  }

private:
  Iter begin_;
  Iter end_;

  std::shared_ptr<const Data> data_;
};

template <typename Iter>
seq<Iter> make_seq(const Iter begin, const Iter end) {
  return seq<Iter>(begin, end);
}

}  // namespace ftl

