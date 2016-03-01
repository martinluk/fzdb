#ifndef STRINGMAPSERIALISER_H
#define STRINGMAPSERIALISER_H

#include <map>
#include <string>
#include "Serialiser.h"
#include <exception>

class StringMapSerialiser
{
public:
	StringMapSerialiser(std::map<std::string, unsigned int>* map);

	std::size_t serialise(Serialiser &serialiser) const;
	void unserialise(const char* serialisedData, std::size_t length);
	
	class InvalidStringTableException : public std::runtime_error
	{
	public:
	    explicit InvalidStringTableException(const std::string &msg) : std::runtime_error(msg)
	    {
	    }
	};

private:
	std::map<std::string, unsigned int>*	_map;
};

#endif // STRINGMAPSERIALISER_H

