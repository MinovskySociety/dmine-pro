#include <iostream>
#include <string>
#include <tuple>

#include "gtest/gtest.h"
#include "gundam/component/container2.h"

template <class ContainerType>
inline void TestDictContainer() {
  ContainerType c;

  ASSERT_EQ(0, c.Count());

  ASSERT_TRUE(c.Insert(5, "e").second);
  ASSERT_TRUE(c.Insert(1, "a").second);
  ASSERT_TRUE(c.Insert(3, "c").second);
  ASSERT_TRUE(c.Insert(2, "b").second);
  ASSERT_TRUE(c.Insert(4, "d").second);

  auto ret = c.Insert(5, "EE");
  ASSERT_FALSE(ret.second);
  ASSERT_EQ("e", ret.first->second);

  typename ContainerType::iterator it = c.Find(3);
  ASSERT_NE(c.end(), it);
  ASSERT_EQ(3, it->first);
  ASSERT_EQ("c", it->second);

  const ContainerType &c_const = c;
  auto it_const = c_const.Find(3);
  ASSERT_NE(c.cend(), it_const);

  ASSERT_TRUE(c.Erase(4));
  ASSERT_FALSE(c.Erase(4));
  ASSERT_TRUE(c.Insert(4, "D").second);

  ASSERT_EQ(5, c.Count());

  int sum_key = 0;
  int count = 0;
  for (auto it = c.cbegin(); it != c.cend(); ++it) {
    sum_key += it->first;
    ++count;

    //std::cout << it->first << " " << it->second << std::endl;
  }
  ASSERT_EQ(15, sum_key);
  ASSERT_EQ(5, count);

  sum_key = 0;
  count = 0;
  for (auto it = c.begin(); it != c.end(); ++it) {
    sum_key += it->first;
    ++count;
  }
  ASSERT_EQ(15, sum_key);
  ASSERT_EQ(5, count);

  sum_key = 0;
  count = 0;
  for (auto p : c) {
    sum_key += p.first;
    ++count;
  }
  ASSERT_EQ(15, sum_key);
  ASSERT_EQ(5, count);

  sum_key = 0;
  count = 0;
  for (const auto &p : c) {
    sum_key += p.first;
    ++count;
  }
  ASSERT_EQ(15, sum_key);
  ASSERT_EQ(5, count);

  ContainerType c1{c};
  ASSERT_EQ(5, c1.Count());

  c1.Clear();
  ASSERT_EQ(0, c1.Count());

  ContainerType c2{std::move(c)};
  ASSERT_EQ(5, c2.Count());
}

TEST(TestContainer2, TestTreeDict) {
  using namespace GUNDAM;

  TestDictContainer<TreeDict<int, std::string>>();
}

TEST(TestContainer2, TestVectorDict) {
  using namespace GUNDAM;

  TestDictContainer<VectorDict<int, std::string>>();
}

TEST(TestContainer2, TestSortedVectorDict) {
  using namespace GUNDAM;

  TestDictContainer<SortedVectorDict<int, std::string>>();
}

TEST(TestContainer2, TestSortedVectorMultiKey) {
  using namespace GUNDAM;

  using ValueType = std::tuple<std::string, int, std::string, double>;

  ValueType v("a", 1, "a1", 0.1);
  ASSERT_EQ(std::string("a"), (GetTuple<0, ValueType>()(v)));
  ASSERT_EQ(1, (GetTuple<1, ValueType>()(v)));
  ASSERT_EQ("a1", (GetTuple<2, ValueType>()(v)));
  ASSERT_EQ(0.1, (GetTuple<3, ValueType>()(v)));

  using Container =
      SortedVectorMultiKey<ValueType, std::string, GetTuple<0, ValueType>, int,
                           GetTuple<1, ValueType>>;

  ASSERT_FALSE(Container::CompareKey1L(v, "a"));
  ASSERT_TRUE(Container::CompareKey2L(v, 2));

  Container c;

  ASSERT_TRUE(c.Empty());
  ASSERT_EQ(0, c.Count());

  bool r;
  Container::iterator it;
  std::tie(it, r) = c.Insert("a", 5, std::make_tuple("a", 5, "a5", 0.5));
  ASSERT_TRUE(r);
  ASSERT_NE(c.end(), it);

  ASSERT_FALSE(c.Empty());
  ASSERT_EQ(1, c.Count());

  ASSERT_TRUE(c.Insert("b", 4, "b", 4, "b4", 0.4).second);
  ASSERT_TRUE(c.Insert("b", 2, "b", 2, "b2", 0.2).second);
  ASSERT_TRUE(c.Insert("b", 6, "b", 6, "b6", 0.6).second);
  ASSERT_TRUE(c.Insert("a", 1, std::make_tuple("a", 1, "a1", 0.1)).second);
  ASSERT_TRUE(c.Insert("a", 3, std::make_tuple("a", 3, "a3", 0.3)).second);

  ASSERT_EQ(6, c.Count());

  for (auto &v : c) {
    std::cout << std::get<0>(v) << " " << std::get<1>(v) << std::endl;
  }

  ASSERT_FALSE(c.Insert("a", 5, "a", 5, "a5d", 0.55).second);

  it = c.Find("a", 5);
  ASSERT_EQ(std::get<2>(*it), "a5");
  std::get<3>(*it) += 0.01;

  Container::const_iterator it_c = static_cast<const Container &>(c).Find("b", 4);
  ASSERT_EQ(std::get<2>(*it_c), "b4");

  auto range_b = c.CRange("b");
  for (auto it_b = range_b.first; it_b != range_b.second; ++it_b) {
    auto &v = *it_b;
    std::cout << std::get<0>(v) << " " << std::get<1>(v) << std::endl;
  }

  auto [it_a_first, it_a_last] = c.Range("a");
  for (auto it_a = it_a_first; it_a != it_a_last; ++it_a) {
    auto &v = *it_a;
    std::cout << std::get<0>(v) << " " << std::get<1>(v) << std::endl;
  }

  auto range_c = c.Range("c");
  ASSERT_EQ(range_c.first, range_c.second);


  ASSERT_EQ(1, c.Erase("a", 3));
  ASSERT_EQ(0, c.Erase("a", 3));
  ASSERT_EQ(2, c.Erase("a"));
  ASSERT_EQ(3, c.Count());

  c.Clear();
  ASSERT_TRUE(c.Empty());
  ASSERT_EQ(0, c.Count());
}