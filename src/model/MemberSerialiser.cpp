#include "MemberSerialiser.h"
#include <cstring>
#include <cassert>

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


std::size_t MemberSerialiser::unserialisePrimitives(const char *serialisedData, std::size_t length)
{
    std::size_t unserialised = 0;
    int count = 0;
    for ( auto it = _primitives.begin(); it != _primitives.end(); ++it )
    {
        if ( unserialised + it->second > length )
            throw InvalidInputMemberException("Length of primitive member " + std::to_string(count) + " exceeds length of input data.");

        memcpy(it->first, serialisedData, it->second);
        unserialised += it->second;
        serialisedData += it->second;
        count++;
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
    std::size_t bytesSerialised = 0;
    for ( auto it = _dynamicMembers.cbegin(); it != _dynamicMembers.cend(); ++it )
    {
        const IDynamicMember* d = *it;
        std::size_t length = d->memberSize();
        bytesSerialised += serialiser.serialise(Serialiser::SerialProperty(&length, sizeof(length)));
        bytesSerialised += serialiser.serialise(Serialiser::SerialProperty(d->memberLocation(), length));
    }
    return bytesSerialised;
}

std::size_t MemberSerialiser::unserialiseDynamicMembers(const char *serialisedData, std::size_t length)
{
    std::size_t unserialised = 0;
    int count = 0;
    for ( auto it = _dynamicMembers.begin(); it != _dynamicMembers.end(); ++it )
    {
        IDynamicMember* d = *it;
        const std::size_t* l = reinterpret_cast<const std::size_t*>(serialisedData);

        if ( unserialised + *l > length )
            throw InvalidInputMemberException("Length of primitive member " + std::to_string(count) + " exceeds length of input data.");

        serialisedData += sizeof(std::size_t);
        d->memberUnserialise(serialisedData, *l);
        unserialised += sizeof(std::size_t) + *l;
        serialisedData += *l;
        count++;
    }
    
    return unserialised;
}

std::size_t MemberSerialiser::serialiseAll(Serialiser &serialiser) const
{
    // Serialise primitives first, then dynamic members.
    return serialisePrimitives(serialiser) + serialiseDynamicMembers(serialiser);
}

std::size_t MemberSerialiser::unserialiseAll(const char *serialisedData, std::size_t length)
{
    // Unserialise primitives first, then dynamic members.
    std::size_t unserialised = 0;
    
    std::size_t primitives = unserialisePrimitives(serialisedData, length);
    unserialised += primitives;
    serialisedData += primitives;

    // This should never happen...
    assert(primitives <= length);
    length -= primitives;
    
    std::size_t dynamicMembers = unserialiseDynamicMembers(serialisedData, length);
    unserialised += dynamicMembers;

    if ( dynamicMembers != length )
        throw InvalidInputMemberException("Unserialising member data did not exhaust input data.");

    return unserialised;
}
