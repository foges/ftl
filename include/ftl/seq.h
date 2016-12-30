#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <ftl/optional.h>
#include <ftl/utils.h>

namespace ftl {

template <typename Function,
          typename Value,
          typename Data=std::vector<Value>>
class seq {
public:
  using value_type = Value;

  seq(const Function &f, const std::shared_ptr<const Data> &data)
      : f_(f), data_(data) { }

  seq(const Function &f) : seq(f, nullptr) { }

  auto get() const {
    std::vector<value_type> res;
    f_([&res](const auto &x){ res.emplace_back(x); return true; });
    return res;
  }

  // TODO make use of .get()
  auto eval() const {
    auto res = std::make_shared<std::vector<value_type>>();
    auto ptr = res.get();
    f_([ptr](const auto &x){ ptr->emplace_back(x); return true; });

    auto begin = res->begin();
    auto end = res->end();
    auto lambda = [begin, end](const auto &f){
        for (auto it = begin; it != end; ++it) {
          if (!f(*it)) {
            break;
          }
        }
    };

    return seq<decltype(lambda), value_type>(lambda, res);
  }

  template <typename Func>
  auto map(Func f) const {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([f_next, f](const auto &x) {
            return f_next(f(x));
        });
    };

    return seq<decltype(lambda),
               decltype(f(*(value_type*)(0))), Data>(lambda, data_);
  }

  template <typename Func>
  auto filter(Func f) const {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([f_next, f](const auto &x) {
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
    f_([&init, f](const auto &x){ init = f(init, x); return true; });

    return init;
  }

  template <typename Func>
  auto take_while(const Func &f) {
    Function f_prev(f_);
    auto lambda = [f_prev, f](const auto &f_next){
        f_prev([f_next, f](const auto &x) {
            if (f(x)) {
              return f_next(x);
            } else {
              return false;
            }
        });
    };

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

//   // TODO make use of .get method
//   template <typename Func>
//   auto sorted(Func cmp) const {
//     auto res = std::make_shared<std::vector<value_type>>();
//     *res = std::vector<value_type>(begin_, end_);
//     std::sort(res->begin(), res->end(), cmp);
//     return seq<Iter>(res->begin(), res->end(), res);
//   }
//
//  template <typename T=Iter>
//  typename std::enable_if<impl::lt_exists<typename T::value_type>::value,
//                          seq<Iter>>::type
//  sorted() const {
//    return sorted([](const auto &x, const auto &y) { return x < y; });
//  }
//
//  template <typename Res=std::vector<typename Iter::value_type>>
//  auto split(const typename Iter::value_type &separator) const {
//    return seq<impl::split_iterator<Iter, Res>, Data>(
//        impl::split_iterator<Iter, Res>(begin_, end_, separator),
//        impl::split_iterator<Iter, Res>(end_, end_, separator), data_);
//  }
//
//
//  auto take(size_t num) {
//    return seq<impl::take_iterator<Iter>, Data>(
//        impl::take_iterator<Iter>(begin_, 0),
//        impl::take_iterator<Iter>(end_, num), data_);
//  }

  ftl::optional<value_type> head() const {
    ftl::optional<value_type> h;
    f_([&h](const auto &x){ h = ftl::optional<value_type>(x); return false; });
    return h;
  }

  ftl::optional<value_type> tail() const {
    ftl::optional<value_type> t;
    f_([&t](const auto &x){ t = ftl::optional<value_type>(x); return true; });
    return t;
  }

  template <typename Func>
  bool any(const Func &f) const {
    bool test = false;
    f_([&test, f](const auto &x) {
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
    f_([&test, f](const auto &x) {
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
  auto lambda = [begin, end](const auto &f){
      for (auto it = begin; it != end; ++it) {
        if (!f(*it)) {
          break;
        }
      }
  };

  return seq<decltype(lambda), typename Iter::value_type>(lambda);
}

}  // namespace ftl

