#include <vector>

#include <gtest/gtest.h>

#include <ftl/ftl.h>

class SeqIntTest : public ::testing::Test {
public:
  SeqIntTest() : a({1, 2, 3}), s(ftl::make_seq(a.begin(), a.end())) { }

  std::vector<int> a;
  ftl::seq<std::vector<int>::iterator> s;
};

TEST_F(SeqIntTest, Map) {
  let res = s.map([](let x){ return x * x;});

  auto it  = res.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(*it, 9);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, Reduce) {
  let acc = s.reduce(0, [](let acc, let x) { return acc + x; });
  EXPECT_EQ(acc, 6);
}

TEST_F(SeqIntTest, MapReduce) {
  let res = s.map([](let x){ return x * x;})
      .reduce(0, [](let acc, let x) { return acc + x;});
  EXPECT_EQ(res, 14);
}

TEST_F(SeqIntTest, Filter) {
  let res = s.filter([](let x) { return x > 1; });

  auto it = res.begin();
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, MaxElement) {
  let max_el = s.max_element([](let x, let y) { return x < y; });
  EXPECT_EQ(*max_el, 3);
  let min_el = s.max_element([](let x, let y) { return x > y; });
  EXPECT_EQ(*min_el, 1);
}

TEST_F(SeqIntTest, Sorted) {
  let res = s.sorted();

  auto it = res.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, SortedReverse) {
  let res = s.sorted([](let x, let y){ return x > y; });

  auto it = res.begin();
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, LazyMap) {
  let res = s.map_lazy([](let x){ return x * x;});

  auto it = res.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(*it, 9);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, LazyFilter) {
  let res = s.filter_lazy([](let x) { return x > 1; });

  auto it = res.begin();
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, LazyMapReduce) {
  let res = s.map_lazy([](let x){ return x * x;})
      .reduce(0, [](let acc, let x) { return acc + x;});
  EXPECT_EQ(res, 14);
}

TEST_F(SeqIntTest, LazyMapFilterReduce) {
  let res = s.map_lazy([](let x){ return x * x;})
      .filter_lazy([](let x){ return x > 1; })
      .reduce(0, [](let acc, let x) { return acc + x;});
  EXPECT_EQ(res, 13);
}

//----------------------------------------------------------------------------//

class SeqStringTest : public ::testing::Test {
public:
  SeqStringTest()
    : a({"aaa", "bb", "c"}),
      s(ftl::make_seq(a.begin(), a.end())) { }

  std::vector<std::string> a;
  ftl::seq<std::vector<std::string>::iterator> s;
};

TEST_F(SeqStringTest, Map) {
  let res = s.map([](let x){ return x + x;});

  auto it  = res.begin();
  EXPECT_EQ(*it, "aaaaaa");
  ++it;
  EXPECT_EQ(*it, "bbbb");
  ++it;
  EXPECT_EQ(*it, "cc");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, Reduce) {
  let acc = s.reduce(std::string(), [](let acc, let x) { return acc + x; });
  EXPECT_EQ(acc, "aaabbc");
}

TEST_F(SeqStringTest, MapReduce) {
  let res = s.map([](let x){ return x + x;})
      .reduce(std::string(), [](let acc, let x) { return acc + x;});
  EXPECT_EQ(res, "aaaaaabbbbcc");
}

TEST_F(SeqStringTest, Filter) {
  let res = s.filter([](let x) { return x.size() > 1; });

  auto it = res.begin();
  EXPECT_EQ(*it, "aaa");
  ++it;
  EXPECT_EQ(*it, "bb");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, MaxElement) {
  let max_el = s.max_element([](let x, let y) { return x.size() < y.size(); });
  EXPECT_EQ(*max_el, "aaa");
  let min_el = s.max_element([](let x, let y) { return x.size() > y.size(); });
  EXPECT_EQ(*min_el, "c");
}

TEST_F(SeqStringTest, Sorted) {
  let res = s.sorted();

  auto it = res.begin();
  EXPECT_EQ(*it, "aaa");
  ++it;
  EXPECT_EQ(*it, "bb");
  ++it;
  EXPECT_EQ(*it, "c");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, SortedReverse) {
  let res = s.sorted([](let x, let y){ return x > y; });

  auto it = res.begin();
  EXPECT_EQ(*it, "c");
  ++it;
  EXPECT_EQ(*it, "bb");
  ++it;
  EXPECT_EQ(*it, "aaa");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, LazyMap) {
  let res = s.map_lazy([](let x){ return x + x;});

  auto it = res.begin();
  EXPECT_EQ(*it, "aaaaaa");
  ++it;
  EXPECT_EQ(*it, "bbbb");
  ++it;
  EXPECT_EQ(*it, "cc");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, LazyFilter) {
  let res = s.filter_lazy([](let x) { return x.size() > 1; });

  auto it = res.begin();
  EXPECT_EQ(*it, "aaa");
  ++it;
  EXPECT_EQ(*it, "bb");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, LazyMapReduce) {
  let res = s.map_lazy([](let x){ return x + x; })
      .reduce(std::string(), [](let acc, let x) { return acc + x; });
  EXPECT_EQ(res, "aaaaaabbbbcc");
}

TEST_F(SeqStringTest, LazyMapFilterReduce) {
  let res = s.map_lazy([](let x){ return x + x; })
      .filter_lazy([](let x){ return x.size() != 4; })
      .reduce(std::string(), [](let acc, let x) { return acc + x; });
  EXPECT_EQ(res, "aaaaaacc");
}

