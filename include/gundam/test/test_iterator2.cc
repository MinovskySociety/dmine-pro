#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "gundam/component/iterator2.h"

class TestString {
 public:
  TestString(const char* c_str) noexcept : str_(c_str) {
    std::cout << "construct: " << c_str << std::endl;
  };

  explicit TestString(const TestString& other) noexcept : str_(other.str_) {
    std::cout << "copy " << str_ << std::endl;
  }

  explicit TestString(TestString&& other) noexcept
      : str_{std::move(other.str_)} {
    std::cout << "move " << str_ << std::endl;
  }

  const std::string& c_str() const { return str_; }

 private:
  std::string str_;
};

TEST(TestGUNDAM, TestGIterator) {
  {
    using C1 = std::vector<TestString>;

    using I1 = GUNDAM::GIterator<C1::iterator, C1::value_type>;

    I1 it0;
    ASSERT_TRUE(it0.IsDone());

    C1 c1{"a", "b", "c", "d", "e"};

    I1 it1{c1.begin(), c1.end()};

    TestString* p = &*it1;
    ASSERT_EQ("a", std::string(p->c_str()));

    int count = 0;
    while (!it1.IsDone()) {
      std::cout << it1->c_str() << std::endl;

      count++;
      ++it1;
    }
    ASSERT_EQ(5, count);
  }

  {
    using C2 = std::set<TestString*>;

    using I2 = GUNDAM::GIterator<C2::iterator, TestString, GUNDAM::PointerCast>;

    using CI2 =
        GUNDAM::GIterator<C2::const_iterator, TestString, GUNDAM::PointerCast>;

    C2 c2;
    c2.emplace(new TestString("a"));
    c2.emplace(new TestString("b"));
    c2.emplace(new TestString("c"));
    c2.emplace(new TestString("d"));
    c2.emplace(new TestString("e"));

    int count = 0;
    I2 it2{c2.begin(), c2.end()};
    while (!it2.IsDone()) {
      std::cout << it2->c_str() << std::endl;

      count++;
      ++it2;
    }
    ASSERT_EQ(5, count);

    count = 0;
    CI2 it2c{c2.cbegin(), c2.cend()};
    while (!it2c.IsDone()) {
      std::cout << it2c->c_str() << std::endl;

      count++;
      ++it2c;
    }
    ASSERT_EQ(5, count);

    for (auto it = c2.begin(); it != c2.end(); ++it) {
      delete *it;
    }
    c2.clear();
  }

  {
    using C3 = std::map<int, TestString>;

    using I3 =
        GUNDAM::GIterator<C3::iterator, TestString, GUNDAM::PairSecondCast>;

    C3 c3;
    c3.emplace(1, "a");
    c3.emplace(2, "b");
    c3.emplace(3, "c");
    c3.emplace(4, "d");
    c3.emplace(5, "e");

    I3 it3{c3.begin(), c3.end()};

    int count = 0;
    while (!it3.IsDone()) {
      std::cout << it3->c_str() << std::endl;

      count++;
      ++it3;
    }
    ASSERT_EQ(5, count);
  }

  {
    using C4 = std::map<int, std::shared_ptr<TestString>>;

    using I4 = GUNDAM::GIterator<C4::iterator, TestString,
                                 GUNDAM::PairSecondPointerCast>;

    using CI4 = GUNDAM::GIterator<C4::const_iterator, const TestString,
                                  GUNDAM::PairSecondPointerCast>;

    C4 c4;
    c4.emplace(1, std::make_shared<TestString>("a"));
    c4.emplace(2, std::make_shared<TestString>("b"));
    c4.emplace(3, std::make_shared<TestString>("c"));
    c4.emplace(4, std::make_shared<TestString>("d"));
    c4.emplace(5, std::make_shared<TestString>("e"));

    I4 it4{c4.begin(), c4.end()};

    int count = 0;
    while (!it4.IsDone()) {
      std::cout << it4->c_str() << std::endl;

      count++;
      ++it4;
    }
    ASSERT_EQ(5, count);

    count = 0;
    CI4 it4c{c4.cbegin(), c4.cend()};

    const TestString* p1 = &*it4c;
    const TestString* p2 = it4c;
    ASSERT_EQ(p1, p2);
    ASSERT_EQ("a", std::string(p1->c_str()));

    while (!it4c.IsDone()) {
      std::cout << it4c->c_str() << std::endl;

      count++;
      ++it4c;
    }
    ASSERT_EQ(5, count);
  }
}