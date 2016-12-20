#pragma once

#include <algorithm>
#include <memory>
#include <vector>

namespace ftl {

template <typename Iter>
class seq {
public:
  using value_type = typename Iter::value_type;
  using iterator = Iter;
  using const_iterator = const iterator;

  seq(const Iter begin,
      const Iter end,
      const std::shared_ptr<std::vector<typename Iter::value_type>> &data)
    : begin_(begin),
      end_(end),
      data_(data) { }

  seq(const Iter begin, const Iter end) : seq(begin, end, nullptr) { }

  // TODO remove both
  size_t size() const { return std::distance(begin_, end_); }
  const auto& operator[](size_t idx) const { return begin_[idx]; }

  bool empty() const { return begin_ == end_; }

  const Iter begin()  const { return begin_; }
  const Iter end()    const { return end_;   }
  const Iter cbegin() const { return begin_; }
  const Iter cend()   const { return end_;   }

  template <typename Func>
  auto map(Func f) const {
    auto res = std::make_shared<std::vector<decltype(f(*begin_))>>();
    res->reserve(this->size());

    for (const auto val : *this) {
      res->emplace_back(f(val));
    }

    return seq<decltype(res->begin())>(res->begin(), res->end(), res);
  }

  template <typename Func>
  auto map_lazy(Func f) const {
    class iterator {
    public:
      using value_type =
          typename std::result_of<Func(typename Iter::value_type)>::type;

      iterator(Iter it, Func f) : it_(it), f_(f) { }
      auto operator*() const { return f_(*it_); }
      iterator& operator++() { ++it_; return *this; }
      bool operator==(const iterator &rhs) const { return it_ == rhs.it_; }
      bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    private:
      Iter it_;
      Func f_;
    };

    return seq<iterator>(iterator(begin_, f), iterator(end_, f), data_);
  }

  template <typename T, typename Func>
  T reduce(T init, Func f) const {
    for (const auto val : *this) {
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

  template <typename Func>
  auto filter(Func f) const {
    auto res = std::make_shared<std::vector<value_type>>();

    for (const auto val : *this) {
      if (f(val)) {
        res->emplace_back(val);
      }
    }
    return seq<decltype(res->begin())>(res->begin(), res->end(), res);
  }

  template <typename Func>
  auto filter_lazy(Func f) const {
    class iterator {
    public:
      using value_type = typename Iter::value_type;

      iterator(Iter it, Iter end, Func f) : it_(it), end_(end), f_(f) {
        make_valid_();
      }
      auto operator*() const { return *it_; }
      iterator& operator++() { ++it_; make_valid_(); return *this; }
      bool operator==(const iterator &rhs) const { return it_ == rhs.it_; }
      bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    private:
      void make_valid_() { while (it_ != end_ && !f_(*it_)) { ++it_; } }
      Iter it_;
      Iter end_;
      Func f_;
    };

    return seq<iterator>(iterator(begin_, end_, f), iterator(end_, end_, f),
        data_);
  }

  template <typename Func>
  auto sorted(Func cmp) const {
    auto res = std::make_shared<std::vector<typename Iter::value_type>>();
    *res = std::vector<typename Iter::value_type>(begin_, end_);
    std::sort(res->begin(), res->end(), cmp);
    return seq<Iter>(res->begin(), res->end(), res);
  }

  // TODO: enable if operator< exists
  auto sorted() const {
    return sorted([](const auto &x, const auto &y) { return x < y; });
  }

private:
  const Iter begin_;
  const Iter end_;

  const std::shared_ptr<std::vector<typename Iter::value_type>> data_;
};

template <typename Iter>
seq<Iter> make_seq(const Iter begin, const Iter end) {
  return seq<Iter>(begin, end);
}

}  // namespace ftl

