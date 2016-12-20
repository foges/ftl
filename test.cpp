#include <iostream>
#include <vector>

#include <ftl/ftl.h>

template <typename T, typename U>
void test(T x, U y) {
  if (x != y) {
    std::cout << "ERROR: " << std::endl;
  }
}

int main() {
  let a = std::vector<int>{1, 2, 3};
  let s = ftl::make_seq(a.begin(), a.end());

  // map
  let b = std::vector<int>{1, 4, 9};
  let r_map = s.map([](let x){ return x * x;});

  test(b[0], r_map[0]);
  test(b[1], r_map[1]);
  test(b[2], r_map[2]);

  // reduce
  let acc = s.reduce(0, [](let acc, let x) { return acc + x; });
  test(acc, 6);

  // map-reduce
  let map_acc = s.map([](let x){ return x * x;})
      .reduce(0, [](let acc, let x) { return acc + x;});
  test(map_acc, 14);

  // filter
  let r_filter = s.filter([](let x) { return x > 1; });
  test(r_filter[0], 2);
  test(r_filter[1], 3);
  test(r_filter.size(), static_cast<size_t>(2));

  // max-element
  let max_el = s.max_element([](let x, let y) { return x < y; });
  test(*max_el, 3);
  let min_el = s.max_element([](let x, let y) { return x > y; });
  test(*min_el, 1);

  // sort
  let sorted = s.sorted();
  {
    auto idx = sorted.begin();
    test(*idx, a[0]);
    ++idx;
    test(*idx, a[1]);
    ++idx;
    test(*idx, a[2]);
    ++idx;
    test(idx, sorted.end());
  }
  let sorted_reverse = s.sorted([](let x, let y){ return x > y; });
  {
    auto idx = sorted_reverse.begin();
    test(*idx, a[2]);
    ++idx;
    test(*idx, a[1]);
    ++idx;
    test(*idx, a[0]);
    ++idx;
    test(idx, sorted_reverse.end());
  }

  // enumerate
  let enm = ftl::enumerate(s);
  auto idx_val = enm.begin();
  test(std::get<0>(*idx_val), 0);
  test(std::get<1>(*idx_val), 1);
  ++idx_val;
  test(std::get<0>(*idx_val), 1);
  test(std::get<1>(*idx_val), 2);
  ++idx_val;
  test(std::get<0>(*idx_val), 2);
  test(std::get<1>(*idx_val), 3);
  ++idx_val;
  test(idx_val, enm.end());

  // lazy-map
  let r_map_lazy = s.map_lazy([](let x){ return x * x;});

  auto it = r_map_lazy.begin();
  test(b[0], *it);
  ++it;
  test(b[1], *it);
  ++it;
  test(b[2], *it);
  ++it;
  test(it, r_map_lazy.end());

  // lazy-filter
  let r_filter_lazy = s.filter_lazy([](let x) { return x > 1; });

  {
    auto it = r_filter_lazy.begin();
    test(2, *it);
    ++it;
    test(3, *it);
    ++it;
    test(it, r_filter_lazy.end());
  }

  // lazy-map-reduce
  let map_acc_lazy = s.map_lazy([](let x){ return x * x;})
      .reduce(0, [](let acc, let x) { return acc + x;});
  test(map_acc_lazy, 14);

  // lazy-map-filter-reduce
  let map_filter_acc_lazy = s.map_lazy([](let x){ return x * x;})
      .filter_lazy([](let x){ return x > 1; })
      .reduce(0, [](let acc, let x) { return acc + x;});
  test(map_filter_acc_lazy, 13);

}

