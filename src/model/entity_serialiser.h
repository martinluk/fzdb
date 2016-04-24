#ifndef MODEL_ENTITYSERIALISER_H
#define MODEL_ENTITYSERIALISER_H

#include "./serialiser.h"
#include <cstring>
#include <memory>
#include <stdexcept>

class Entity;

// Serialises and unserialises the data within an Entity object.
class EntitySerialiser {
 public:
  EntitySerialiser(const std::shared_ptr<Entity> ent);

  std::size_t serialise(Serialiser &serialiser) const;
  static std::shared_ptr<Entity> unserialise(const char* serialData, std::size_t length);

  class InvalidInputEntityException : public std::runtime_error {
   public:
    explicit InvalidInputEntityException(const std::string &msg) : std::runtime_error(msg) {
    }
  };

 private:
  const std::shared_ptr<Entity> _entity;
};

#endif  // MODEL_ENTITYSERIALISER_H
