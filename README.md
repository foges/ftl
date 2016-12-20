## C++ functional tools library

The goal of this minimal library is to promote functional concepts in C++.

### Summary

This library is based around a sequence class and the let keyword.

- `class seq` -- creates an immutable sequence from an iterator, and defines
methods such as map, map_lazy, filter, filter_lazy, reduce, sorted, etc..
- `#define let const auto` -- an easy way to promote the use of immutable
variables and encourage more focus on naming, by decreasing the focus on
explicitly writing out the type. If you'd prefer this three letter word not
polluting your workspace, you can comment it out in `include/ftl/flt.h`. The
library itself does not use the keyword.

Other classes of interestes are
- `class enumerate` -- enumerates a sequence, similar to pyton's enumerate
- `class zip` -- zip together a list of sequences, similar to pyton's zip

### Usage

Simply add `#include <ftl/ftl.h>` to your file.

### Examples

``` c++
#include <assert.h>
#include <vector>

#include <ftl/ftl.h>

int main() {
  let a = std::vector<int>{1, 2, 3};
  let res = ftl::make_seq(a.begin(), a.end())
      .map_lazy([](let x){ return x * x;})
      .filter_lazy([](let x){ return x > 1; })
      .reduce(0, [](let acc, let x) { return acc + x;});
  assert(res == 13);
}
```

### Todo
- move to gTest
- add zip.h
- complete seq
  - .contains
  - .join
  - .split
  - .reverseIterators
- add project euler examples
- add generators?
  - iota
  - file
  - fib

