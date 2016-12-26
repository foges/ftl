#include <gtest/gtest.h>

#include <ftl/ftl.h>

class MemoizeIntTest : public ::testing::Test {
public:
  MemoizeIntTest() : a({1, 2, 3}), s(ftl::make_seq(a.begin(), a.end())) { }

  std::vector<int> a;
  ftl::seq<std::vector<int>::iterator> s;
};

TEST_F(MemoizeIntTest, Basic) {
  let is_even_impl = [](let x){ return x % 2 == 0; };
  let is_even = ftl::memoize<decltype(is_even_impl), int>(is_even_impl);

  EXPECT_EQ(is_even(1),   false);
  EXPECT_EQ(is_even(2),   true);
  EXPECT_EQ(is_even(111), false);
  EXPECT_EQ(is_even(222), true);
  EXPECT_EQ(is_even(1),   false);
  EXPECT_EQ(is_even(2),   true);
}

TEST_F(MemoizeIntTest, Adversarial) {
  bool adversarial = false;
  let is_even_impl = [&adversarial](let x){ return x % 2 == adversarial; };
  let is_even = ftl::memoize<decltype(is_even_impl), int>(is_even_impl);

  EXPECT_EQ(is_even(1),   false);
  EXPECT_EQ(is_even(2),   true);
  EXPECT_EQ(is_even(111), false);
  EXPECT_EQ(is_even(222), true);
  adversarial = true;
  EXPECT_EQ(is_even_impl(1), true);
  EXPECT_EQ(is_even_impl(2), false);
  EXPECT_EQ(is_even(1),   false);
  EXPECT_EQ(is_even(2),   true);
}

