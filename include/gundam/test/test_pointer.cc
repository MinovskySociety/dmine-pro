#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "gtest/gtest.h"

#include "gundam/component/pointer.h"

class TestString {
 public:
  TestString() { std::cout << "construct" << std::endl; }

  TestString(const char* c_str) noexcept : str_(c_str) {
    std::cout << "construct: " << str_ << std::endl;
  };

  TestString(const TestString& other) noexcept : str_(other.str_) {
    std::cout << "copy " << str_ << std::endl;
  }

  TestString(TestString&& other) noexcept
      : str_{std::move(other.str_)} {
    std::cout << "move " << str_ << std::endl;
  }

  TestString& operator=(const TestString& other) {
    str_ = other.str_;
    std::cout << "copy assign " << str_ << std::endl;
    return *this;
  }

  TestString& operator=(TestString&& other) {
    str_ = std::move(other.str_);
    std::cout << "move assign " << str_ << std::endl;
    return *this;
  }

  bool operator==(const TestString& other) const { return str_ == other.str_; }

  bool operator<(const TestString& other) const { return str_ < other.str_; }

  const std::string& c_str() const { return str_; }

  void assign(const char* c_str) { str_ = c_str; }

 private:
  std::string str_;
};

/*
TEST(TestGUNDAM, TestGPointer) {
  using TestStringPtr = GUNDAM::GPointer<TestString, false>;
  using TestStringConstPtr = GUNDAM::GPointer<TestString, true>;

  TestString s("abc");

  TestString* p0 = &s;
  const TestString* cp0 = &s;

  // TestString* p1(cp0);
  const TestString* cp1 = p0;
  const TestString* cp2(cp0);

  p0->assign("ABC");
  //cp0->assign("ABC");

  ASSERT_TRUE(p0);
  ASSERT_TRUE(cp0);

  ASSERT_EQ(p0, &s);
  ASSERT_EQ(cp0, cp1);
  ASSERT_EQ(p0, cp0);

  TestStringPtr t0(&s);
  TestStringConstPtr ct0(&s);

  ASSERT_EQ("ABC", t0->c_str());
  ASSERT_EQ("ABC", ct0->c_str());

  t0->assign("ABC");
  //ct0->assign("ABC");

  TestStringPtr t1(p0);
  //TestStringPtr t2(cp0);
  TestStringConstPtr ct1 = p0;
  TestStringConstPtr ct2(cp0);

  ASSERT_FALSE(t0.IsNull());
  ASSERT_FALSE(ct0.IsNull());

  ASSERT_TRUE(t0);
  ASSERT_TRUE(ct0);
  
  ASSERT_EQ(t0, t1);  
  ASSERT_EQ(t1, ct2);  

  std::set<TestStringConstPtr> ss;
  ss.emplace(TestString("b"));
  ss.emplace(TestString("c"));
  ss.emplace(TestString("a"));
  ss.emplace(TestString("d"));
  ss.emplace(TestString("e"));
  ss.emplace(TestString("d"));
  ASSERT_EQ(5, ss.size());
}
*/