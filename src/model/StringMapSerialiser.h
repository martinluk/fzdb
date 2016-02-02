#ifndef STRINGMAPSERIALISER_H
#define STRINGMAPSERIALISER_H

#include <map>
#include <string>
#include "Serialiser.h"

class StringMapSerialiser
{
public:
	StringMapSerialiser(std::map<std::string, unsigned int>* map);

	std::size_t serialise(Serialiser &serialiser) const;
	void unserialise(const char* serialisedData);

private:
	std::map<std::string, unsigned int>*	_map;
};

#endif // STRINGMAPSERIALISER_H

