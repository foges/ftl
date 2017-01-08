#pragma once

#include <algorithm>
#include <memory>
#include <vector>
#include <set>

#include <ftl/functors.h>
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

  /* \brief Apply a function to each element in the sequence
   *
   *  The sequence stops when f(x) returns false.
   */
  template <typename Func>
  void apply(const Func &f) const {
    Function f_prev(f_);
    f_prev([&f](const auto &x){ return f(x); });
  }

  /* \brief Constructs lambda to pipe the result of one sequence into the next
   *
   *  The function f must take as arguments two lambdas, the first one
   *  containing the generator for the previous sequence and the second
   *  containing the acceptor for the next sequence
   */
  template <typename Func>
  auto pipe(const Func &f) const {
    Function f_prev(f_);
    return [f_prev, f](const auto &f_next){ f(f_prev, f_next); };
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

  // Implementations

  template <typename Func>
  bool all(const Func &f) const {
    bool test = true;
    apply([&test, &f](const auto &x) {
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

  template <typename Func>
  bool any(const Func &f) const {
    bool test = false;
    apply([&test, &f](const auto &x) {
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
  size_t count(const Func &f) const {
    size_t num = 0;;
    apply([&num, &f](const auto &x){ if (f(x)) { num += 1; }; return true; });
    return num;
  }

  size_t count() const {
    return count([](const auto&){ return true; });
  }

  template <typename Func>
  auto dedup(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        optional<decltype(f(impl::instance_of<value_type>()))> last;
        f_prev([&f_next, &f, &last](const auto &x){
            const auto fx = f(x);
            if (!last || *last != fx) {
              last = make_optional(fx);
              return f_next(x);
            }
            return true;
        });
    });

    return seq<decltype(lambda), value_type>(lambda, data_);
  }

  auto dedup() const {
    return dedup(identity());
  }

  auto drop(size_t num) const {
    auto lambda = pipe([num](const auto &f_prev, const auto &f_next) {
        size_t idx = 0;
        f_prev([&f_next, num, &idx](const auto &x){
            ++idx;
            if (idx <= num) {
              return true;
            } else {
              return f_next(x);
            }
        });
    });

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  auto drop_every(size_t num) const {
    auto lambda = pipe([num](const auto &f_prev, const auto &f_next) {
        size_t idx = 0;
        f_prev([&f_next, num, &idx](const auto &x){
            idx++;
            if (idx % num == 0) {
              return true;
            } else {
              return f_next(x);
            }
        });
    });

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto drop_while(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        bool do_drop = true;
        f_prev([&f_next, &do_drop, &f](const auto &x){
            do_drop = do_drop && f(x);
            if (do_drop) {
              return true;
            } else {
              return f_next(x);
            }
        });
    });

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto filter(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        f_prev([&f_next, &f](const auto &x){
            if (f(x)) {
              return f_next(x);
            } else {
              return true;
            }
        });
    });

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto flat_map(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        f_prev([&f_next, &f](const auto &x){
            bool do_continue = true;
            x.apply([&f_next, &f, &do_continue](const auto &x) {
                do_continue = f_next(f(x));
                return do_continue;
            });
            return do_continue;
        });
    });

    using result_type = decltype(f(*(typename value_type::value_type*)(0)));
    return seq<decltype(lambda), result_type, Data>(lambda, data_);
  }

  ftl::optional<value_type> head() const {
    ftl::optional<value_type> h;
    apply([&h](const auto &x){ h = ftl::make_optional(x); return false; });
    return h;
  }

  template <typename Func>
  auto map(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        f_prev([&f_next, &f](const auto &x){
            return f_next(f(x));
        });
    });

    return seq<decltype(lambda),
               decltype(f(*(value_type*)(0))), Data>(lambda, data_);
  }

  template <typename Func>
  ftl::optional<value_type> max(const Func &cmp) const {
    ftl::optional<value_type> res;
    apply([&res, &cmp](const auto &x){
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

  template <typename T, typename Func>
  T reduce(T init, const Func &f) const {
    apply([&init, &f](const auto &x){ init = f(init, x); return true; });

    return init;
  }

  template <typename Func>
  auto reject(const Func &f) const {
    return filter([f](const auto &x){ return !f(x); });
  }

  auto reverse() const {
    auto res = get_shared();
    std::reverse(res->begin(), res->end());

    return seq<seq_iter_type, value_type>(
        seq_iter_type(res->begin(), res->end()), res);
  }

  template <typename Func>
  auto scan(const Func &f) const {
    using result_type = decltype(f(impl::instance_of<value_type>(),
                                   impl::instance_of<value_type>()));
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        optional<result_type> acc;
        f_prev([&f_next, &f, &acc](const auto &x){
            if (acc) {
              acc = f(x, *acc);
            } else {
              acc = x;
            }
            return f_next(*acc);
        });
    });

    return seq<decltype(lambda), result_type>(lambda, data_);
  }

  template <typename T, typename Func>
  auto scan(const T &init, const Func &f) const {
    using result_type = decltype(f(init, impl::instance_of<value_type>()));
    auto lambda = pipe([init, f](const auto &f_prev, const auto &f_next) {
        result_type acc = init;
        f_prev([&f_next, &f, &acc](const auto &x){
            acc = f(x, acc);
            return f_next(acc);
        });
    });

    return seq<decltype(lambda), result_type>(lambda, data_);
  }


  template <typename Func>
  auto sorted(const Func &cmp) const {
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
    auto lambda = pipe([separator](const auto &f_prev, const auto &f_next) {
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
    });

    return seq<decltype(lambda), Result, Data>(lambda, data_);
  }

  template <typename T=value_type>
  typename std::enable_if<impl::plus_exists<T>::value, T>::type
  sum(const T& init=T()) const {
    return reduce(init, [](const T &acc, const value_type &x){
        return acc + static_cast<T>(x);
    });
  }

  ftl::optional<value_type> tail() const {
    ftl::optional<value_type> t;
    apply([&t](const auto &x){ t = ftl::make_optional(x); return true; });
    return t;
  }

  auto take(const size_t num) {
    auto lambda = pipe([num](const auto &f_prev, const auto &f_next) {
        size_t idx = 0;
        f_prev([&f_next, &idx, num](const auto &x) {
            if (idx < num) {
              ++idx;
              return f_next(x);
            } else {
              return false;
            }
        });
    });

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto take_while(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        f_prev([&f_next, &f](const auto &x){
            if (f(x)) {
              return f_next(x);
            } else {
              return false;
            }
        });
    });

    return seq<decltype(lambda), value_type, Data>(lambda, data_);
  }

  template <typename Func>
  auto uniq(const Func &f) const {
    auto lambda = pipe([f](const auto &f_prev, const auto &f_next) {
        std::set<decltype(f(impl::instance_of<value_type>()))> vals;
        f_prev([&f_next, &f, &vals](const auto &x){
            const auto fx = f(x);
            const auto it = vals.find(fx);
            if (it == vals.end()) {
              vals.insert(fx);
              return f_next(x);
            }
            return true;
        });
    });

    return seq<decltype(lambda), value_type>(lambda, data_);
  }

  auto uniq() const {
    return uniq(identity());
  }

  auto with_index() const {
    auto lambda = pipe([](const auto &f_prev, const auto &f_next) {
        size_t idx = 0;
        f_prev([&f_next, &idx](const auto &x) {
            return f_next(std::make_tuple(idx++, x));
        });
    });

    return seq<decltype(lambda), std::tuple<size_t, value_type>, Data>(lambda,
        data_);
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

