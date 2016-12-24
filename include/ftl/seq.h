#pragma once

#include <algorithm>
#include <memory>
#include <vector>

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

  template <typename Func>
  auto map(Func f) const {
    auto res = std::make_shared<std::vector<decltype(f(*begin_))>>();

    for (const auto &val : *this) {
      res->emplace_back(f(val));
    }

    return seq<decltype(res->begin())>(res->begin(), res->end(), res);
  }

  template <typename Func>
  auto map_lazy(Func f) const {
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
  auto max_element(Func cmp) const {
    auto max_el = begin_;
    for (auto it = begin_; it != end_; ++it) {
      if (cmp(*max_el, *it)) {
        max_el = it;
      }
    }
    return max_el;
  }

  template <typename T=Iter>
  typename std::enable_if<impl::plus_exists<typename T::value_type>::value,
                          value_type>::type
  sum() const {
    return reduce(value_type(), [](const value_type &acc, const value_type &x){
        return acc + x;
    });
  }

  template <typename Func>
  auto filter(Func f) const {
    auto res = std::make_shared<std::vector<value_type>>();

    for (const auto &val : *this) {
      if (f(val)) {
        res->emplace_back(val);
      }
    }
    return seq<decltype(res->begin())>(res->begin(), res->end(), res);
  }

  template <typename Func>
  auto filter_lazy(Func f) const {
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

  template <typename T=std::vector<typename Iter::value_type>>
  auto split(const typename Iter::value_type &separator) const {
    auto res = std::make_shared<std::vector<T>>();
    Iter last_it = begin_;
    for (auto it = begin_; it != end_; ++it) {
      if (*it == separator) {
        res->emplace_back(last_it, it);
        last_it = it;
        ++last_it;
      }
    }
    res->emplace_back(last_it, end_);
    return seq<typename std::vector<T>::iterator>(res->begin(), res->end(), res);
  }

  template <typename Res=std::vector<typename Iter::value_type>>
  auto split_lazy(const typename Iter::value_type &separator) const {
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

  auto head() const {
    return *begin_;
  }

  auto tail() const {
    for (auto it = begin_;;) {
      const auto val = *it;
      ++it;
      if (it == end_) {
          return val;
      }
    }
    return *end_;
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

