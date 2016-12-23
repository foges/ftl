#include <vector>

#include <gtest/gtest.h>

#include <ftl/ftl.h>

class ZipTest : public ::testing::Test {
public:
  ZipTest()
      : a1({1, 2, 3}),
        s1(ftl::make_seq(a1.begin(), a1.end())),
        a2({"aaa", "bb", "c"}),
        s2(ftl::make_seq(a2.begin(), a2.end())),
        zip_int(ftl::zip(s1)),
        zip_string(ftl::zip(s2)),
        zip_int_string(ftl::zip(s1, s2)) { }

  const std::vector<int> a1;
  const ftl::seq<std::vector<const int>::iterator> s1;
  const std::vector<std::string> a2;
  const ftl::seq<std::vector<const std::string>::iterator> s2;

  const ftl::seq<
    ftl::impl::zip_iterator<std::vector<const int>::iterator>> zip_int;
  const ftl::seq<
    ftl::impl::zip_iterator<std::vector<const std::string>::iterator>>
    zip_string;
  const ftl::seq<
    ftl::impl::zip_iterator<std::vector<const int>::iterator,
                            std::vector<const std::string>::iterator>>
    zip_int_string;
};

TEST_F(ZipTest, IteratorCorrectness) {
  {
    auto it  = zip_int.begin();

    EXPECT_EQ(std::get<0>(*it), 1);
    ++it;
    EXPECT_EQ(std::get<0>(*it), 2);
    ++it;
    EXPECT_EQ(std::get<0>(*it), 3);
    ++it;
    EXPECT_EQ(it, zip_int.end());
  }

  {
    auto it  = zip_string.begin();

    EXPECT_EQ(std::get<0>(*it), "aaa");
    ++it;
    EXPECT_EQ(std::get<0>(*it), "bb");
    ++it;
    EXPECT_EQ(std::get<0>(*it), "c");
    ++it;
    EXPECT_EQ(it, zip_string.end());
  }

  {
    auto it  = zip_int_string.begin();

    EXPECT_EQ(std::get<0>(*it), 1);
    EXPECT_EQ(std::get<1>(*it), "aaa");
    ++it;
    EXPECT_EQ(std::get<0>(*it), 2);
    EXPECT_EQ(std::get<1>(*it), "bb");
    ++it;
    EXPECT_EQ(std::get<0>(*it), 3);
    EXPECT_EQ(std::get<1>(*it), "c");
    ++it;
    EXPECT_EQ(it, zip_int_string.end());
  }
}


TEST_F(ZipTest, ReferenceAddressEquality) {
  {
    let it = zip_int.begin();
    EXPECT_EQ(&std::get<0>(*it), &a1[0]);
  }

  {
    let it = zip_string.begin();
    EXPECT_EQ(&std::get<0>(*it), &a2[0]);
  }

  {
    let it = zip_int_string.begin();
    EXPECT_EQ(&std::get<0>(*it), &a1[0]);
    EXPECT_EQ(&std::get<1>(*it), &a2[0]);
  }
}

TEST_F(ZipTest, Looping) {
  for (let val : zip_int) {
    EXPECT_GT(std::get<0>(val), 0);
  }

  for (let val : zip_string) {
    EXPECT_GT(std::get<0>(val).size(), 0);
  }

  for (let val : zip_int_string) {
    EXPECT_GT(std::get<0>(val), 0);
    EXPECT_GT(std::get<1>(val).size(), 0);
  }
}

TEST_F(ZipTest, LoopingWithReference) {
  for (let &val : zip_int) {
    EXPECT_GT(std::get<0>(val), 0);
  }

  for (let &val : zip_string) {
    EXPECT_GT(std::get<0>(val).size(), 0);
  }

  for (let &val : zip_int_string) {
    EXPECT_GT(std::get<0>(val), 0);
    EXPECT_GT(std::get<1>(val).size(), 0);
  }
}

