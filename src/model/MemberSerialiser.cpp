#include "MemberSerialiser.h"
#include <cstring>

MemberSerialiser::MemberSerialiser()
{
	
}

std::size_t MemberSerialiser::serialisePrimitives(Serialiser &serialiser) const
{
	std::vector<Serialiser::SerialProperty> propList;
	for ( auto it = _primitives.cbegin(); it != _primitives.cend(); ++it )
	{
		propList.push_back(Serialiser::SerialProperty(it->first, it->second));
	}
	return serialiser.serialise(propList);
}


std::size_t MemberSerialiser::unserialisePrimitives(const char *serialisedData)
{
	std::size_t unserialised = 0;
	for ( auto it = _primitives.begin(); it != _primitives.end(); ++it )
	{
		memcpy(it->first, serialisedData, it->second);
		unserialised += it->second;
		serialisedData += it->second;
	}
	
	return unserialised;
}

void MemberSerialiser::addPrimitive(void *location, std::size_t size)
{
	_primitives.push_back(PrimitivePair(location, size));
}

void MemberSerialiser::clearPrimitives()
{
	_primitives.clear();
}

void MemberSerialiser::addDynamicMember(IDynamicMember *idm)
{
	_dynamicMembers.push_back(idm);
}

void MemberSerialiser::clearDynamicMembers()
{
	_dynamicMembers.clear();
}

std::size_t MemberSerialiser::serialiseDynamicMembers(Serialiser &serialiser) const
{
	for ( auto it = _dynamicMembers.cbegin(); it != _dynamicMembers.cend(); ++it )
	{
		const IDynamicMember* d = *it;
		std::size_t length = d->memberSize();
		serialiser.serialise(Serialiser::SerialProperty(&length, sizeof(length)));
		serialiser.serialise(Serialiser::SerialProperty(d->memberLocation(), length));
	}
}

std::size_t MemberSerialiser::unserialiseDynamicMembers(const char *serialisedData)
{
	std::size_t unserialised = 0;
	for ( auto it = _dynamicMembers.begin(); it != _dynamicMembers.end(); ++it )
	{
		IDynamicMember* d = *it;
		const std::size_t* length = reinterpret_cast<const std::size_t*>(serialisedData);
		serialisedData += sizeof(std::size_t);
		d->memberUnserialise(serialisedData, *length);
		unserialised += *length;
		serialisedData += *length;
	}
	
	return unserialised;
}

std::size_t MemberSerialiser::serialiseAll(Serialiser &serialiser) const
{
	// Serialise primitives first, then dynamic members.
	return serialisePrimitives(serialiser) + serialiseDynamicMembers(serialiser);
}

std::size_t MemberSerialiser::unserialiseAll(const char *serialisedData)
{
	// Unserialise primitives first, then dynamic members.
	std::size_t unserialised = 0;
	
	std::size_t primitives = unserialisePrimitives(serialisedData);
	unserialised += primitives;
	serialisedData += primitives;
	
	std::size_t dynamicMembers = unserialiseDynamicMembers(serialisedData);
	unserialised += dynamicMembers;
	return unserialised;
}
