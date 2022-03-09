#include "gtest/gtest.h"

#include "gundam/data_type/datetime.h"
#include "gundam/data_type/datatype.h"

TEST(TestGUNDAM, TestDataType) {
  using namespace GUNDAM;

  DateTime t("1984-5-23 19:3:1");

  std::cout << t << std::endl;

  ASSERT_EQ("1984-05-23 19:03:01", t.to_string());

  ASSERT_EQ(BasicDataType::kTypeString, StringToEnum("string"));
  ASSERT_EQ(BasicDataType::kTypeInt, StringToEnum("int"));
  ASSERT_EQ(BasicDataType::kTypeInt64, StringToEnum("int64"));
  ASSERT_EQ(BasicDataType::kTypeFloat, StringToEnum("float"));
  ASSERT_EQ(BasicDataType::kTypeDouble, StringToEnum("double"));  
  ASSERT_EQ(BasicDataType::kTypeDateTime, StringToEnum("datetime"));
  ASSERT_EQ(BasicDataType::kTypeUnknown, StringToEnum("unknown"));
  ASSERT_EQ(BasicDataType::kTypeUnknown, StringToEnum("xxx"));
  ASSERT_EQ(BasicDataType::kTypeUnknown, StringToEnum(""));

  ASSERT_EQ(std::string("string"), EnumToString(BasicDataType::kTypeString));
  ASSERT_EQ(std::string("int"), EnumToString(BasicDataType::kTypeInt));
  ASSERT_EQ(std::string("int64"), EnumToString(BasicDataType::kTypeInt64));
  ASSERT_EQ(std::string("float"), EnumToString(BasicDataType::kTypeFloat));
  ASSERT_EQ(std::string("double"), EnumToString(BasicDataType::kTypeDouble));  
  ASSERT_EQ(std::string("datetime"), EnumToString(BasicDataType::kTypeDateTime));
  ASSERT_EQ(std::string("unknown"), EnumToString(BasicDataType::kTypeUnknown));
  
  ASSERT_EQ(BasicDataType::kTypeString, TypeToEnum<std::string>());
  ASSERT_EQ(BasicDataType::kTypeInt, TypeToEnum<uint8_t>());
  ASSERT_EQ(BasicDataType::kTypeInt, TypeToEnum<short>());  
  ASSERT_EQ(BasicDataType::kTypeInt, TypeToEnum<int>());
  ASSERT_EQ(BasicDataType::kTypeInt, TypeToEnum<uint32_t>());
  ASSERT_EQ(BasicDataType::kTypeInt64, TypeToEnum<int64_t>());
  ASSERT_EQ(BasicDataType::kTypeInt64, TypeToEnum<uint64_t>());
  ASSERT_EQ(BasicDataType::kTypeFloat, TypeToEnum<float>());
  ASSERT_EQ(BasicDataType::kTypeDouble, TypeToEnum<double>());  
  ASSERT_EQ(BasicDataType::kTypeDateTime, TypeToEnum<DateTime>());
  ASSERT_EQ(BasicDataType::kTypeUnknown, TypeToEnum<std::vector<int>>());

  ASSERT_EQ(BasicDataType::kTypeString, TypeToEnum(std::string()));
  ASSERT_EQ(BasicDataType::kTypeInt, TypeToEnum(int()));
  ASSERT_EQ(BasicDataType::kTypeInt64, TypeToEnum(int64_t()));
  ASSERT_EQ(BasicDataType::kTypeFloat, TypeToEnum(float()));
  ASSERT_EQ(BasicDataType::kTypeDouble, TypeToEnum(double()));
  ASSERT_EQ(BasicDataType::kTypeDateTime, TypeToEnum(DateTime()));
  ASSERT_EQ(BasicDataType::kTypeUnknown, TypeToEnum(std::vector<int>()));

  ASSERT_EQ(std::string("string"), TypeToString<std::string>());
  ASSERT_EQ(std::string("int"), TypeToString<int>());
  ASSERT_EQ(std::string("int64"), TypeToString<int64_t>());
  ASSERT_EQ(std::string("float"), TypeToString<float>());
  ASSERT_EQ(std::string("double"), TypeToString<double>());
  ASSERT_EQ(std::string("datetime"), TypeToString<DateTime>());
  ASSERT_EQ(std::string("unknown"), TypeToString<std::vector<double>>());

  ASSERT_EQ(std::string("string"), TypeToString(std::string()));
  ASSERT_EQ(std::string("int"), TypeToString(int()));
  ASSERT_EQ(std::string("int64"), TypeToString(int64_t()));
  ASSERT_EQ(std::string("float"), TypeToString(float()));
  ASSERT_EQ(std::string("double"), TypeToString(double()));
  ASSERT_EQ(std::string("datetime"), TypeToString(DateTime()));
  ASSERT_EQ(std::string("unknown"), TypeToString(std::vector<int>()));
}