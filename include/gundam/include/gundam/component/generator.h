#ifndef _GUNDAM_COMPONENT_GENERATOR_H
#define _GUNDAM_COMPONENT_GENERATOR_H

#include <type_traits>

namespace GUNDAM {

template <class IDType>
class VoidIDGenerator {
 public:
  void Reset() {}

  IDType GetID() {}

  void UseID(const IDType& id) {}
};

template <class IDType, class = typename std::enable_if<
                            std::is_arithmetic<IDType>::value, bool>::type>
class SimpleArithmeticIDGenerator {
 public:
  void Reset() { used_max_ = 0; }

  IDType GetID() { return ++used_max_; }

  void UseID(const IDType& id) {
    if (id > used_max_) used_max_ = id;
  }

 private:
  IDType used_max_ = 0;
};

}  // namespace GUNDAM
#endif