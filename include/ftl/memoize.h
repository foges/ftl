#include <unordered_map>
#include <utility>

namespace ftl {
namespace impl {

typedef std::tuple<int, char, char> key_t;

template <int N, typename... Args>
struct tuple_hash : public std::unary_function<std::tuple< Args...>, size_t> {
  size_t operator()(const std::tuple<Args...>& key) const {
    return std::get<N>(key) ^ tuple_hash<N - 1, Args...>(key);
  }
};

template <typename... Args>
struct tuple_hash<0, Args...>
    : public std::unary_function<std::tuple< Args...>, size_t> {
  size_t operator()(const std::tuple<Args...>& key) const {
    return std::get<0>(key);
  }
};

}  // namespace impl

template <typename Func, typename... Args>
class memoize {
public:
  using value_type = typename std::result_of<Func(Args...)>::type;

  memoize(const Func &f) : f_(f) { }

  const value_type& operator()(const Args&... args) const {
    const auto args_tup = std::tuple<Args...>(args...);
    auto it = cache_.find(args_tup);
    if (it == cache_.end()) {
      it = cache_.emplace(args_tup, f_(args...)).first;
    }
    return it->second;
  }

private:
  mutable std::unordered_map<
      std::tuple<Args...>,
      value_type,
      impl::tuple_hash<sizeof...(Args) - 1, Args...>
      > cache_;
  Func f_;
};

}  // namespace ftl

