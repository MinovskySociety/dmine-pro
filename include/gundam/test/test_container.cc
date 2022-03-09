#include <iostream>
#include <string>
#include <tuple>

#include "gtest/gtest.h"

#include "gundam/component/container.h"

TEST(TestGUNDAM, TestContainer) {
  using namespace GUNDAM;

  Container<ContainerType::Vector, SortType::Default, int, int, std::string> c1;

  auto r = c1.Insert(100);
  ASSERT_TRUE(r.second);
  ASSERT_EQ(100, r.first.get<0>());

  ASSERT_TRUE(c1.Insert(101).second);
  ASSERT_TRUE(c1.Insert(102).second);
  ASSERT_TRUE(c1.Insert(103).second);
  ASSERT_TRUE(c1.Insert(104).second);

  r = c1.Insert(104);
  ASSERT_FALSE(r.second);
  ASSERT_EQ(104, r.first.get<0>());

  r.first.get<1>() = 1;
  r.first.get<2>() = "AAA";
}