#ifndef MODEL_GRAPHSERIALISER_H
#define MODEL_GRAPHSERIALISER_H

#include <cstring>
#include "Serialiser.h"
#include <stdexcept>
#include <string>

class EntityManager;

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
    EntityManager* _manager;
};

#endif  // MODEL_GRAPHSERIALISER_H
