#ifndef MODEL_TYPES_TYPESERIALISER_H
#define MODEL_TYPES_TYPESERIALISER_H

#include <cstring>
#include <memory>
#include "./serialiser.h"
#include "../types/base.h"

namespace model {
namespace types {
// Convenience class for serialising value types.
class TypeSerialiser {
 public:
  TypeSerialiser(const std::shared_ptr<Base> &type);

  std::size_t serialise(Serialiser &serialiser) const;

  // TODO: This is probably unsafe without a length parameter!
  static std::shared_ptr<Base> unserialise(const char* serialisedData, std::size_t* advance = NULL);

 private:
  const std::shared_ptr<Base> baseType_;
};
}
}

#endif  // MODEL_TYPES_TYPESERIALISER_H
