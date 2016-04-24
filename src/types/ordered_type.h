#ifndef FUZZY_MODEL_TYPES_ORDERED_TYPE
#define FUZZY_MODEL_TYPES_ORDERED_TYPE

#include <string>

#include "./base.h"
#include <iostream>

namespace model {
namespace types {

// Stores an integer value.
class OrderedType : public Base {

 public:
  OrderedType() : Base() {}
  virtual ~OrderedType() {}
  virtual bool greaterThan(const std::string rhs) = 0;
  virtual bool lessThan(const std::string rhs) = 0;
 protected:

  OrderedType(const char* &serialisedData, std::size_t length)
    : Base(serialisedData, length) { }
};
}
}


#endif // !FUZZY_MODEL_TYPES_ORDERED_TYPE
