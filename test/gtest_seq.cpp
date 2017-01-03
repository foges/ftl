#include <vector>

#include <gtest/gtest.h>

#include <ftl/ftl.h>

class SeqIntTest : public ::testing::Test {
public:
  SeqIntTest() : a({1, 2, 3}), s(ftl::make_seq(a.begin(), a.end())) { }

  const std::vector<int> a;
  ftl::seq<ftl::impl::seq_iter<std::vector<int>::const_iterator>, int> s;
};

TEST_F(SeqIntTest, Map) {
  let res = s.map([](let x){ return x * x; }).get();

  auto it  = res.begin();
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(*it, 4);
  ++it;
  EXPECT_EQ(*it, 9);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, FlatMap) {
  let res = s.map([this](let a){
      return s.map([a](let b){
          return std::make_tuple(a, b);
      });
  })
  .flat_map([](let x){ return std::get<0>(x) * std::get<1>(x); })
  .sum();

   EXPECT_EQ(res, 36);
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
  let res = s.filter([](let x) { return x > 1; }).get();

  auto it = res.begin();
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, MaxValue) {
  let max_el = s.max();
  EXPECT_EQ(*max_el, 3);
}

TEST_F(SeqIntTest, MaxValueFunctor) {
  let max_val = s.max([](let x, let y) { return x < y; });
  EXPECT_EQ(*max_val, 3);
  let min_val = s.max([](let x, let y) { return x > y; });
  EXPECT_EQ(*min_val, 1);
}

TEST_F(SeqIntTest, Sorted) {
  let res = s.sorted().get();

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
  let res = s.sorted([](let x, let y){ return x > y; }).get();

  auto it = res.begin();
  EXPECT_EQ(*it, 3);
  ++it;
  EXPECT_EQ(*it, 2);
  ++it;
  EXPECT_EQ(*it, 1);
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqIntTest, TakeWhile) {
  let res = s.take_while([](let x){ return x < 3; }).sum();
  EXPECT_EQ(res, 3);
}

TEST_F(SeqIntTest, Tail) {
  let res = s.tail();
  EXPECT_EQ(*res, 3);
}

TEST_F(SeqIntTest, Head) {
  let res = s.head();
  EXPECT_EQ(*res, 1);
}

TEST_F(SeqIntTest, AnyTrue) {
  let res = s.map([](let x){ return x == 2; }).any();
  EXPECT_EQ(res, true);
}

TEST_F(SeqIntTest, AnyFalse) {
  let res = s.map([](let x){ return x == 4; }).any();
  EXPECT_EQ(res, false);
}

TEST_F(SeqIntTest, AllTrue) {
  let res = s.map([](let x){ return x > 0; }).all();
  EXPECT_EQ(res, true);
}

TEST_F(SeqIntTest, AllFalse) {
  let res = s.map([](let x){ return x == 2; }).all();
  EXPECT_EQ(res, false);
}

//----------------------------------------------------------------------------//

class SeqStringTest : public ::testing::Test {
public:
  SeqStringTest()
    : a({"aaa", "bb", "c"}),
      s(ftl::make_seq(a.begin(), a.end())) { }

  const std::vector<std::string> a;
  ftl::seq<ftl::impl::seq_iter<std::vector<std::string>::const_iterator>,
           std::string> s;
};

TEST_F(SeqStringTest, Map) {
  let res = s.map([](let x){ return x + x;}).get();

  auto it  = res.begin();
  EXPECT_EQ(*it, "aaaaaa");
  ++it;
  EXPECT_EQ(*it, "bbbb");
  ++it;
  EXPECT_EQ(*it, "cc");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, FlatMap) {
  let res = s.map([this](let a){
      let size = static_cast<int>(a.size());
      return s.map([size](let b){
          return std::make_tuple(size, b.size());
      });
  })
  .flat_map([](let x){ return std::get<0>(x) * std::get<1>(x); })
  .sum();

   EXPECT_EQ(res, 36);
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
  let res = s.filter([](let x) { return x.size() > 1; }).get();

  auto it = res.begin();
  EXPECT_EQ(*it, "aaa");
  ++it;
  EXPECT_EQ(*it, "bb");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, MaxValueFunctor) {
  let max_el = s.max([](let x, let y) { return x.size() < y.size(); });
  EXPECT_EQ(*max_el, "aaa");
  let min_el = s.max([](let x, let y) { return x.size() > y.size(); });
  EXPECT_EQ(*min_el, "c");
}

TEST_F(SeqStringTest, Sorted) {
  let res = s.sorted().get();

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
  let res = s.sorted([](let x, let y){ return x > y; }).get();

  auto it = res.begin();
  EXPECT_EQ(*it, "c");
  ++it;
  EXPECT_EQ(*it, "bb");
  ++it;
  EXPECT_EQ(*it, "aaa");
  ++it;
  EXPECT_EQ(it, res.end());
}

TEST_F(SeqStringTest, TakeWhile) {
  let res = s.take_while([](let x){ return x.size() > 1; }).sum();
  EXPECT_EQ(res, "aaabb");
}

TEST_F(SeqStringTest, Tail) {
  let res = s.tail();
  EXPECT_EQ(*res, "c");
}

TEST_F(SeqStringTest, Head) {
  let res = s.head();
  EXPECT_EQ(*res, "aaa");
}

TEST_F(SeqStringTest, AnyTrue) {
  let res = s.map([](let x){ return x == "bb"; }).any();
  EXPECT_EQ(res, true);
}

TEST_F(SeqStringTest, AnyFalse) {
  let res = s.map([](let x){ return x == "ccc"; }).any();
  EXPECT_EQ(res, false);
}

TEST_F(SeqStringTest, AllTrue) {
  let res = s.map([](let x){ return x.size() > 0; }).all();
  EXPECT_EQ(res, true);
}

TEST_F(SeqStringTest, AllFalse) {
  let res = s.map([](let x){ return x == "aaa"; }).all();
  EXPECT_EQ(res, false);
}

//------------------------------------------------------------------------------

class SeqNoOpsTest : public ::testing::Test {
public:
  struct NoOps { int val; };

  SeqNoOpsTest() : a({{1}, {2}, {3}}) { }

  const std::vector<NoOps> a;
};

TEST_F(SeqNoOpsTest, Sorted) {
  let s = ftl::make_seq(a.begin(), a.end());
  let res = s.sorted([](auto x, auto y){ return x.val < y.val; }).get();

  auto it = res.begin();
  EXPECT_EQ(it->val, 1);
  ++it;
  EXPECT_EQ(it->val, 2);
  ++it;
  EXPECT_EQ(it->val, 3);
  ++it;
  EXPECT_EQ(it, res.end());
}

//------------------------------------------------------------------------------

class StringTest : public ::testing::Test {
public:
  StringTest() : a("the quick brown fox, jumps over the lazy dog"),
                 s(ftl::make_seq(a.begin(), a.end())) { }

  const std::string a;
  ftl::seq<ftl::impl::seq_iter<std::string::const_iterator>, char> s;
};

TEST_F(StringTest, SplitToVectorOnWhitespace) {
  let s = ftl::make_seq(a.begin(), a.end());
  let split = s.split(' ').get();

  auto it = split.begin();
  EXPECT_EQ(std::string(it->data(), it->size()), "the");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "quick");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "brown");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "fox,");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "jumps");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "over");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "the");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "lazy");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()), "dog");
  ++it;
  EXPECT_EQ(it, split.end());
}

TEST_F(StringTest, SplitToVectorOnComma) {
  let s = ftl::make_seq(a.begin(), a.end());
  let split = s.split(',').get();

  auto it = split.begin();
  EXPECT_EQ(std::string(it->data(), it->size()), "the quick brown fox");
  ++it;
  EXPECT_EQ(std::string(it->data(), it->size()),  " jumps over the lazy dog");
  ++it;
  EXPECT_EQ(it, split.end());
}

TEST_F(StringTest, SplitToStringOnWhitespace) {
  let s = ftl::make_seq(a.begin(), a.end());
  let split = s.split<std::string>(' ').get();

  auto it = split.begin();
  EXPECT_EQ(*it, "the");
  ++it;
  EXPECT_EQ(*it, "quick");
  ++it;
  EXPECT_EQ(*it, "brown");
  ++it;
  EXPECT_EQ(*it, "fox,");
  ++it;
  EXPECT_EQ(*it, "jumps");
  ++it;
  EXPECT_EQ(*it, "over");
  ++it;
  EXPECT_EQ(*it, "the");
  ++it;
  EXPECT_EQ(*it, "lazy");
  ++it;
  EXPECT_EQ(*it, "dog");
  ++it;
  EXPECT_EQ(it, split.end());
}

TEST_F(StringTest, SplitToStringOnComma) {
  let s = ftl::make_seq(a.begin(), a.end());
  let split = s.split<std::string>(',').get();

  auto it = split.begin();
  EXPECT_EQ(*it, "the quick brown fox");
  ++it;
  EXPECT_EQ(*it,  " jumps over the lazy dog");
  ++it;
  EXPECT_EQ(it, split.end());
}

