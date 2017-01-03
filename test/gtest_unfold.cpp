#include <gtest/gtest.h>

#include <ftl/ftl.h>

class UnfoldTest : public ::testing::Test {
public:
  UnfoldTest() { }
};

TEST_F(UnfoldTest, Iota) {
  let iota = ftl::unfold(0, [](let x) { return x + 1; }).take(4).get();

  auto it = iota.begin();
  EXPECT_EQ(*it, 0);
  ++it;
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
}

TEST_F(UnfoldTest, Fibonacci) {
  let fib = ftl::unfold(std::make_tuple(1, 1), [](let x) {
        return std::make_tuple(std::get<1>(x),
                               std::get<0>(x) + std::get<1>(x));
      })
      .take(6)
      .get();

  auto it = fib.begin();
  EXPECT_EQ(std::get<0>(*it), 1);
  ++it;
  EXPECT_EQ(std::get<0>(*it), 1);
  ++it;
  EXPECT_EQ(std::get<0>(*it), 2);
  ++it;
  EXPECT_EQ(std::get<0>(*it), 3);
  ++it;
  EXPECT_EQ(std::get<0>(*it), 5);
  ++it;
  EXPECT_EQ(std::get<0>(*it), 8);
}
