#include <vector>
#include <string>

#include <gtest/gtest.h>

#include <ftl/ftl.h>

class EnumIntTest : public ::testing::Test {
public:
  EnumIntTest() : a({1, 2, 3}), s(ftl::make_seq(a.begin(), a.end())) { }

  std::vector<int> a;
  ftl::seq<std::vector<int>::iterator> s;
};

TEST_F(EnumIntTest, FromSeq) {
  let res = ftl::enumerate(s);
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

TEST_F(EnumIntTest, FromIter) {
  let res = ftl::enumerate(a.begin(), a.end());

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

class EnumStringTest : public ::testing::Test {
public:
  EnumStringTest()
      : a({"aaa", "bb", "c"}),
        s(ftl::make_seq(a.begin(), a.end())) { }

  std::vector<std::string> a;
  ftl::seq<std::vector<std::string>::iterator> s;
};

TEST_F(EnumStringTest, FromSeq) {
  let res = ftl::enumerate(s);
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

TEST_F(EnumStringTest, FromIter) {
  let res = ftl::enumerate(a.begin(), a.end());

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
