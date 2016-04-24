#include "./author_id.h"
#include "../singletons.h"

void model::types::AuthorID::setupDefaultMetaData(const unsigned char confidence) {
}

std::string model::types::AuthorID::toString() const {
  if (_value == 0) return "System";
  return Singletons::cDatabase()->users().getUserName(_value);
}
