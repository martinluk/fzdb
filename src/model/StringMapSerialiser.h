#ifndef STRINGMAPSERIALISER_H
#define STRINGMAPSERIALISER_H

#include <boost/bimap.hpp>
#include <string>
#include "Serialiser.h"
#include <stdexcept>

// Specialised class used to serialise a table that maps strings to
// unsigned integers.
class StringMapSerialiser
{
public:
    StringMapSerialiser(std::map<std::string, unsigned int>* map);
    StringMapSerialiser(boost::bimap<std::string, unsigned int>* bimap);

    std::size_t serialise(Serialiser &serialiser) const;
    void unserialise(const char* serialisedData, std::size_t length);
    
    class InvalidInputStringTableException : public std::runtime_error
    {
    public:
        explicit InvalidInputStringTableException(const std::string &msg) : std::runtime_error(msg)
        {
        }
    };

private:
    std::map<std::string, unsigned int>*    _map;
    boost::bimap<std::string, unsigned int>* _bimap;
};

#endif // STRINGMAPSERIALISER_H

