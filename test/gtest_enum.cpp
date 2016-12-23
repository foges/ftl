#include <vector>
#include <string>

#include <gtest/gtest.h>

#include <ftl/ftl.h>

class EnumTest : public ::testing::Test {
public:
  EnumTest()
      : a1({1, 2, 3}),
        s1(ftl::make_seq(a1.begin(), a1.end())),
        a2({"aaa", "bb", "c"}),
        s2(ftl::make_seq(a2.begin(), a2.end())) { }

  const std::vector<int> a1;
  const ftl::seq<std::vector<const int>::iterator> s1;
  const std::vector<std::string> a2;
  const ftl::seq<std::vector<const std::string>::iterator> s2;
};

TEST_F(EnumTest, IteratorCorrectness) {
  {
    let res = ftl::enumerate(s1);
    auto it = res.begin();

    EXPECT_EQ(std::get<0>(*it), 0);
    EXPECT_EQ(std::get<1>(*it), 1);
    ++it;
    EXPECT_EQ(std::get<0>(*it), 1);
    EXPECT_EQ(std::get<1>(*it), 2);
    ++it;
    EXPECT_EQ(std::get<0>(*it), 2);
    EXPECT_EQ(std::get<1>(*it), 3);
    ++it;
    EXPECT_EQ(it, res.end());
  }

  {
    let res = ftl::enumerate(s2);
    auto it = res.begin();

    EXPECT_EQ(std::get<0>(*it), 0);
    EXPECT_EQ(std::get<1>(*it), "aaa");
    ++it;
    EXPECT_EQ(std::get<0>(*it), 1);
    EXPECT_EQ(std::get<1>(*it), "bb");
    ++it;
    EXPECT_EQ(std::get<0>(*it), 2);
    EXPECT_EQ(std::get<1>(*it), "c");
    ++it;
    EXPECT_EQ(it, res.end());
  }
}

TEST_F(EnumTest, ReferenceAddressEquality) {
  {
    let res = ftl::enumerate(s1);
    let it = res.begin();
    EXPECT_EQ(&std::get<1>(*it), &a1[0]);
  }

  {
    let res = ftl::enumerate(s2);
    let it = res.begin();
    EXPECT_EQ(&std::get<1>(*it), &a2[0]);
  }
}

TEST_F(EnumTest, Loop) {
  for (let val : ftl::enumerate(s1)) {
    EXPECT_GT(std::get<1>(val), 0);
  }

  for (let val : ftl::enumerate(s2)) {
    EXPECT_GT(std::get<1>(val).size(), 0);
  }
}

TEST_F(EnumTest, LoopWithReference) {
  for (let &val : ftl::enumerate(s1)) {
    EXPECT_GT(std::get<1>(val), 0);
  }

  for (let &val : ftl::enumerate(s2)) {
    EXPECT_GT(std::get<1>(val).size(), 0);
  }
}

