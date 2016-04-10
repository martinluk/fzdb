#ifndef MODEL_PROPERTYOWNERSERIALISER_H
#define MODEL_PROPERTYOWNERSERIALISER_H

#include "Serialiser.h"
#include <cstring>
#include <memory>
#include <stdexcept>

class PropertyOwner;

// Serialises and unserialises the data within an Entity object.
class PropertyOwnerSerialiser
{
public:
    PropertyOwnerSerialiser(const std::shared_ptr<PropertyOwner> po);

    std::size_t serialise(Serialiser &serialiser) const;
    std::shared_ptr<PropertyOwner> unserialise(const char* &serialData, std::size_t length);

    class InvalidInputEntityException : public std::runtime_error
    {
    public:
        explicit InvalidInputEntityException(const std::string &msg) : std::runtime_error(msg)
        {
        }
    };

private:
    const std::shared_ptr<PropertyOwner> _entity;
};

#endif  // MODEL_PROPERTYOWNERSERIALISER_H
