#ifndef _GPDEF_H
#define _GPDEF_H

#include <gundam/datatype.h>
#include <gundam/geneator.h>

#include <cstdint>
#include <string>

namespace graphpackage {

using ID = uint64_t;
using Label = uint32_t;
using AttributeKey = std::string;
using DataType = GUNDAM::BasicDataType;

using IDGen = GUNDAM::SimpleArithmeticIDGenerator<ID>;

enum class LabelType : int {
  kUnknown = 0,
  kEntity = 1,
  kRelation = 2,
  kAttribute = 3,
  kValue = 4,
  kConstant = 5,
  kValueConstant = 6,
  kAttributeValueConstant = 7
};

enum ReservedLabel : Label {
  kSource = 1,
  kTarget = 2,
  kHas = 3,
  kIs = 4,
  kEqual = 5
};

}  // namespace graphpackage

#endif