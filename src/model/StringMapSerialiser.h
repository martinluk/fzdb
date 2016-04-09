#ifndef STRINGMAPSERIALISER_H
#define STRINGMAPSERIALISER_H

#include "../util/NameManager.h"
#include <string>
#include "Serialiser.h"
#include <stdexcept>

// Specialised class used to serialise a table that maps strings to
// unsigned integers.
class StringMapSerialiser
{
public:
    StringMapSerialiser(NameManager* nameManager);

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
	NameManager* _nameManager;
};

#endif // STRINGMAPSERIALISER_H

