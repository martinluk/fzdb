#ifndef MODEL_GRAPHSERIALISER_H
#define MODEL_GRAPHSERIALISER_H

#include <cstring>
#include "Serialiser.h"

class EntityManager;

class GraphSerialiser
{
public:
    GraphSerialiser(EntityManager *manager);
    
    std::size_t serialise(Serialiser &serialiser) const;
    
    // NOTE: This function does not clear the current list of entities within the manager.
    void unserialise(const char* serialisedData);
    
private:
    EntityManager* _manager;
};

#endif  // MODEL_GRAPHSERIALISER_H
