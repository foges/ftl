#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <ftl/optional.h>
#include <ftl/utils.h>

namespace ftl {
namespace impl {

template <typename Iter>
class seq_iter {
public:
  seq_iter(const Iter &begin, const Iter &end) : begin_(begin), end_(end) { }

  template <typename Func>
  void operator()(const Func &f_next) const {
    for (auto it = begin_; it != end_; ++it) {
      if (!f_next(*it)) {
        break;
      }
    }
  }

private:
  Iter begin_;
  Iter end_;
};

}  // namespace impl

template <typename Function,
          typename Value,
          typename Data=std::vector<Value>>
class seq {
public:
  using value_type = Value;
  using seq_iter_type =
      impl::seq_iter<typename std::vector<value_type>::iterator>;

  seq(const Function &f, const std::shared_ptr<const Data> &data)
      : f_(f), data_(data) { }

  seq(const Function &f) : seq(f, nullptr) { }

  template <typename Func>
  void apply(const Func &f) const {
    Function f_prev(f_);
    f_prev([&f](const auto &x){ return f(x); });
  }

  auto get() const {
    std::vector<value_type> res;
    apply([&res](const auto &x){ res.emplace_back(x); return true; });
    return res;
  }

  auto get_shared() const {
    return std::make_shared<std::vector<value_type>>(this->get());
  }

  auto eval() const {
    auto res = this->get_shared();
    return seq<seq_iter_type, value_type>(
        seq_iter_type(res->begin(), res->end()), res);
  }

  template <typename Func>
  auto map(Func f) const {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([&f_next, &f](const auto &x){
            return f_next(f(x));
        });
    };

    return seq<decltype(lambda),
               decltype(f(*(value_type*)(0))), Data>(lambda, data_);
  }

  template <typename Func>
  auto flat_map(Func f) const {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([&f_next, &f](const auto &x){
            bool do_continue = true;
            x.apply([&f_next, &f, &do_continue](const auto &x) {
                do_continue = f_next(f(x));
                return do_continue;
            });
            return do_continue;
        });
    };

    return seq<decltype(lambda),
               decltype(f(*(typename value_type::value_type*)(0))), Data>(lambda, data_);
  }


  template <typename Func>
  auto filter(Func f) const {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([&f_next, f](const auto &x){
            if (f(x)) {
              return f_next(x);
            } else {
              return true;
            }
        });
    };

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename T, typename Func>
  T reduce(T init, Func f) const {
    apply([&init, f](const auto &x){ init = f(init, x); return true; });

    return init;
  }

  template <typename T=value_type>
  typename std::enable_if<impl::plus_exists<T>::value, T>::type
  sum(const T& init=T()) const {
    return reduce(init, [](const T &acc, const value_type &x){
        return acc + static_cast<T>(x);
    });
  }

  template <typename Func>
  ftl::optional<value_type> max(const Func &cmp) const {
    Function f_prev(f_);
    ftl::optional<value_type> res;
    f_prev([&res, &cmp](const auto &x){
      if (!res || cmp(*res, x)) {
        res = ftl::optional<value_type>(x);
      }
      return true;
    });
    return res;
  }

  template <typename T=value_type>
  typename std::enable_if<impl::lt_exists<T>::value, ftl::optional<T>>::type
  max() const {
    return max([](const T &x, const T &y) { return x < y; });
  }

  template <typename Func>
  auto take_while(const Func &f) const {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([&f_next, f](const auto &x){
            if (f(x)) {
              return f_next(x);
            } else {
              return false;
            }
        });
    };

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto take(const size_t num) {
    Function f_prev(f_);
    auto lambda = [f_prev, num](const auto &f_next){
        size_t idx = 0;
        f_prev([&f_next, &idx, num](const auto &x) {
            if (idx < num) {
              ++idx;
              return f_next(x);
            } else {
              return false;
            }
        });
    };

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto sorted(Func cmp) const {
    auto res = this->get_shared();
    std::sort(res->begin(), res->end(), cmp);
    return seq<seq_iter_type, value_type>(
        seq_iter_type(res->begin(), res->end()), res);
  }

  template <typename T=Value>
  typename std::enable_if<
      impl::lt_exists<T>::value,
      seq<impl::seq_iter<typename std::vector<value_type>::iterator>,
          value_type>>::type
  sorted() const {
    return sorted([](const auto &x, const auto &y) { return x < y; });
  }

  /* Note: Result must conform to a standard container interface
   */
  template <typename Result=std::vector<value_type>>
  auto split(const value_type &separator) const {
    Function f_prev(f_);
    auto lambda = [f_prev, separator](const auto &f_next){
        Result result;
        bool do_continue = false;
        f_prev([&f_next, &separator, &result, &do_continue](const auto &x) {
            do_continue = true;
            if (x == separator) {
              do_continue = f_next(result);
              result.clear();
            } else {
              result.push_back(x);
            }
            return do_continue;
        });

        if (do_continue) {
          f_next(result);
        }
    };

    return seq<decltype(lambda), Result, Data>(lambda, data_);
  }

  ftl::optional<value_type> head() const {
    ftl::optional<value_type> h;
    apply([&h](const auto &x){ h = ftl::make_optional(x); return false; });
    return h;
  }

  ftl::optional<value_type> tail() const {
    ftl::optional<value_type> t;
    apply([&t](const auto &x){ t = ftl::make_optional(x); return true; });
    return t;
  }

  template <typename Func>
  bool any(const Func &f) const {
    bool test = false;
    apply([&test, f](const auto &x) {
        if (f(x)) {
          test = true;
          return false;
        } else {
          return true;
        }
    });
    return test;
  }

  template <typename T=value_type>
  typename std::enable_if<impl::bool_exists<T>::value, bool>::type
  any() const {
    return any([](const T &x) { return static_cast<bool>(x); });
  }

  template <typename Func>
  bool all(const Func &f) const {
    bool test = true;
    apply([&test, f](const auto &x) {
        if (!f(x)) {
          test = false;
          return false;
        } else {
          return true;
        }
    });
    return test;
  }

  template <typename T=value_type>
  typename std::enable_if<impl::bool_exists<T>::value, bool>::type
  all() const {
    return all([](const T &x) { return static_cast<bool>(x); });
  }

private:
  Function f_;

  std::shared_ptr<const Data> data_;
};

template <typename Iter>
auto make_seq(const Iter &begin, const Iter &end) {
  return seq<impl::seq_iter<Iter>, typename Iter::value_type>(
      impl::seq_iter<Iter>(begin, end));
}

}  // namespace ftl

