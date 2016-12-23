#include <ftl/optional.h>
#include <ftl/seq.h>

namespace ftl {
namespace impl {

template <typename T, typename Func>
class unfold_iterator {
public:
  using value_type = T;

  unfold_iterator(const optional<T> &acc, const Func &f)
      : acc_(acc), f_(f) { }

  value_type operator*() const {
    return *acc_;
  }

  unfold_iterator& operator++() {
    acc_ = f_(*acc_);
    return *this;
  }

  bool operator==(const unfold_iterator &rhs) const {
    return acc_ == rhs.acc_;
  }

  bool operator!=(const unfold_iterator &rhs) const {
    return !(*this == rhs);
  }

private:
  optional<T> acc_;
  Func f_;
};

}  // namespace impl

template <typename T, typename Func>
auto unfold(const T &acc, const Func &f) {
  return make_seq(impl::unfold_iterator<T, Func>(optional<T>(acc), f),
      impl::unfold_iterator<T, Func>(optional<T>(), f));
}

}  // namespace flt

