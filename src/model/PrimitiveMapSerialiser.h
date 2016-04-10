#ifndef PRIMITIVEMAPSERIALISER_H
#define PRIMITIVEMAPSERIALISER_H

#include <map>
#include <stdexcept>
#include "Serialiser.h"

template<typename A, typename B>
class PrimitiveMapSerialiser
{
public:
    class InvalidPrimitiveMapInputException : public std::runtime_error
    {
    public:
        explicit InvalidPrimitiveMapInputException(const std::string &msg) : std::runtime_error(msg)
        {
        }
    };

    PrimitiveMapSerialiser(std::map<A,B> &map) :
        _map(map)
    {
    }

    std::size_t serialise(Serialiser &serialiser) const;
    void unserialise(const char* data, std::size_t length);

private:
    std::map<A,B> &_map;
};

#endif // PRIMITIVEMAPSERIALISER_H

