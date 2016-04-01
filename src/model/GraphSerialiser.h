#ifndef MODEL_GRAPHSERIALISER_H
#define MODEL_GRAPHSERIALISER_H

#include <cstring>
#include "Serialiser.h"
#include <stdexcept>
#include <string>
#include <set>
#include <map>

#include "Entity.h"
class EntityManager;

// Serialises and unserialises all entity data within an EntityManager.
class GraphSerialiser
{
public:
    GraphSerialiser(EntityManager *manager);
    
    std::size_t serialise(Serialiser &serialiser) const;
    
    void unserialise(const char* serialisedData, std::size_t length);
    
    class InvalidInputGraphException : public std::runtime_error
    {
    public:
    explicit InvalidInputGraphException(const std::string &msg) : std::runtime_error(msg)
    {
    }
    };
    
private:
    std::size_t serialise(const std::map<Entity::EHandle_t, std::set<Entity::EHandle_t> > &map, Serialiser &serialiser) const;
    void unserialise(std::map<Entity::EHandle_t, std::set<Entity::EHandle_t> > &map, const char* serialisedData, std::size_t length);

    EntityManager* _manager;
};

#endif  // MODEL_GRAPHSERIALISER_H
