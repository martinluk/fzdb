#include "./serialiser.h"
#include <cstring>

const unsigned char Serialiser::StructPaddingChar = 0x00;
void Serialiser::zeroBuffer(void *dest, std::size_t size) {
  memset(dest, StructPaddingChar, size);
}

Serialiser::Serialiser() {
  lastSerialiseBytes_ = 0;
}

std::size_t Serialiser::serialise(const std::vector<SerialProperty> &properties) {
  // Calculate the final size of all the data to be serialised.
  std::size_t propSize = 0;

  for ( auto it = properties.cbegin(); it != properties.cend(); ++it ) {
    propSize += it->second;
  }

  // Resize the vector to be long enough.
  std::size_t prevSize = serialData_.size();
  serialData_.reserve(prevSize + propSize);
  auto startIt = serialData_.begin() + prevSize;

  // Copy all the properties in.
  for ( auto it = properties.cbegin(); it != properties.cend(); it++ ) {
    const char* first = static_cast<const char*>(it->first);
    const char* last = first + it->second;
    serialData_.insert(startIt, first, last);
    std::advance(startIt, it->second);
  }

  lastSerialiseBytes_ = propSize;
  return lastSerialiseBytes_;
}

std::size_t Serialiser::serialise(const SerialProperty &property) {
  std::size_t propSize = property.second;
  std::size_t prevSize = serialData_.size();
  serialData_.reserve(prevSize + propSize);
  auto startIt = serialData_.begin() + prevSize;
  const char* first = static_cast<const char*>(property.first);
  const char* last = first + property.second;
  serialData_.insert(startIt, first, last);
  lastSerialiseBytes_ = propSize;
  return lastSerialiseBytes_;
}

void Serialiser::clear() {
  serialData_.clear();
  lastSerialiseBytes_ = 0;
}

char* Serialiser::begin() {
  return serialData_.data();
}

const char* Serialiser::cbegin() const {
  return serialData_.data();
}

char* Serialiser::end() {
  return serialData_.data() + serialData_.size() - 1 ;
}

const char* Serialiser::cend() const {
  return serialData_.data() + serialData_.size() - 1 ;
}

std::size_t Serialiser::size() const {
  return serialData_.size();
}

std::size_t Serialiser::lastSerialiseBytes() const {
  return lastSerialiseBytes_;
}
