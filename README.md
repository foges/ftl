## C++ functional tools library

The goal of this minimal library is to promote functional concepts in C++.

### Summary

This library is based around a sequence class and the let keyword.

- `class seq` -- creates an immutable sequence from an iterator, and defines
methods such as map, map_lazy, filter, filter_lazy, reduce, sorted, max_element,
split, split_lazy, and take_while.
etc..
- `#define let const auto` -- C++ treats variables as mutable by default, and
as programmers we have a tendancy to stick to defaults. Behavior can be hard to
get around, especially if you don't give people an incentive to change. By
defining `let == const auto`, we give C++ programmers an _easier_ way
to define variables, while at the same time encouraging immutability.
In addition, by decreasing the focus on explicitly writing the type name,
there is more space to focus on writing readable variable names. That being
said, we understand that not everyone likes immutability (or more probably
preprocessor hash-defines) forced on them. If you're one of these people you can
simply comment it out one line in `include/ftl/flt.h`. Don't worry, the library
itself does not use the let keyword, so you won't break anything (apart from
tests) by commenting out this line.

Other classes of interestes are
- `class enumerate` -- enumerates a sequence, similar to python's enumerate.
- `class unfold` -- creates a stream of values such that x{n+1} = f(x{n}),
similar to elixir's unfold.
- `class zip` -- zip together a list of sequences, similar to python's zip.

### Examples

``` c++
#include <assert.h>
#include <vector>

#include <ftl/ftl.h>

int main() {
  let a = std::vector<int>{1, 2, 3};
  let res = ftl::make_seq(a.begin(), a.end())
      .map_lazy([](let x){ return x * x; })
      .filter_lazy([](let x){ return x > 1; })
      .reduce(0, [](let acc, let x) { return acc + x; });
  assert(res == 13);
}
```

### Usage

This is a header only library. Simply add `#include <ftl/ftl.h>` to your file.

If you'd like syntax highlighting for `let` in vim, just add the keyword to
your `c.vim` syntax file. Find the line `syn keyword	cStorageClass` and add
it.

### Tests

To run the tests please install gtest and then run

```
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DGTEST_PATH=<your-gtest-path> ..
make -j
./test_ftl
```

### Todo
- complete seq
  - .split (for splitting on sequences)
  - Reduce convenience methods (.contains, ...)
  - .scan
  - .join
  - .reverseIterators
- add project euler examples
- add generators?
  - iota
  - file
  - fib
- investigate parallelism

