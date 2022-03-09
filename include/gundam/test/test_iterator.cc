#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "gundam/component/container.h"
#include "gundam/component/iterator.h"

TEST(TestGUNDAM, TestInnerIterator) {
  using namespace GUNDAM;

  using C1 = Container<ContainerType::Vector, SortType::Default, int, std::string>;
  using II1 = InnerIterator_<C1, false, 1>;
  using C2 = Container<ContainerType::Vector, SortType::Default, int, C1>;
  using II2 = InnerIterator_<C2, false, 2>;

  II1 it0;
  ASSERT_TRUE(it0.IsDone());

  C1 c1;

  c1.Insert(1).first.get<1>() = "A";
  c1.Insert(2).first.get<1>() = "B";
  c1.Insert(3).first.get<1>() = "C";

  II1 it1(c1.begin(), c1.end());

  ASSERT_FALSE(it1.IsDone());
  ASSERT_EQ(1, (it1.get<int, 0, 0>()));
  ASSERT_EQ("A", (it1.get<std::string, 1, 0>()));

  it1.ToNext();
  ASSERT_FALSE(it1.IsDone());
  ASSERT_EQ(2, (it1.get<int, 0, 0>()));
  ASSERT_EQ("B", (it1.get<std::string, 1, 0>()));

  it1.ToNext();
  ASSERT_FALSE(it1.IsDone());
  ASSERT_EQ(3, (it1.get_const<int, 0, 0>()));
  ASSERT_EQ("C", (it1.get_const<std::string, 1, 0>()));

  it1.ToNext();
  ASSERT_TRUE(it1.IsDone());

  C2 c2;
  C1 &c1r1 = c2.Insert(1).first.get<1>();  
  c1r1.Insert(101);
  c1r1.Insert(102);
  c1r1.Insert(103);  
  C1 &c1r2 = c2.Insert(2).first.get<1>();
  c1r2.Insert(201);
  c1r2.Insert(202);
  c1r2.Insert(203);
  C1 &c1r3 = c2.Insert(3).first.get<1>();  
  c1r3.Insert(301);
  c1r3.Insert(302);
  c1r3.Insert(303);

  II2 it2(c2.begin(), c2.end());

  while (!it2.IsDone()) {
    std::cout << it2.get<int, 0, 1>() << std::endl;    
    it2.ToNext();    
  }
}