#ifndef MODEL_ENTITYSERIALISER_H
#define MODEL_ENTITYSERIALISER_H

#include "Serialiser.h"
#include <cstring>
#include <memory>

class Entity;

class EntitySerialiser
{
public:
    EntitySerialiser(const std::shared_ptr<Entity> ent);

    std::size_t serialise(Serialiser &serialiser) const;

    // TODO: This is probably unsafe without a length parameter!
    static std::shared_ptr<Entity> unserialise(const char* serialData);

private:
    const std::shared_ptr<Entity> _entity;
};

#endif  // MODEL_ENTITYSERIALISER_H
