#include "MemberSerialiser.h"
#include <cstring>
#include <cassert>
//#include <iostream>

MemberSerialiser::MemberSerialiser()
{
    _initialised = false;
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
            throw InvalidInputMemberException("Length of primitive member (index " + std::to_string(count)
                                              + " of " + std::to_string(_primitives.size()) + " members) exceeds length of input data.");

        memcpy(it->first, serialisedData, it->second);
        unserialised += it->second;
        serialisedData += it->second;
        count++;
    }
    
    return unserialised;
}

void MemberSerialiser::addPrimitive(void *location, std::size_t size)
{
    assert(!_initialised);
    _primitives.push_back(PrimitivePair(location, size));
}

void MemberSerialiser::clearPrimitives()
{
    _primitives.clear();
    _initialised = false;
}

void MemberSerialiser::addDynamicMember(IDynamicMember *idm)
{
    assert(!_initialised);
    _dynamicMembers.push_back(idm);
}

void MemberSerialiser::clearDynamicMembers()
{
    _dynamicMembers.clear();
    _initialised = false;
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
            throw InvalidInputMemberException("Length of primitive member (index " + std::to_string(count)
                                              + " of " + std::to_string(_dynamicMembers.size()) + " members) exceeds length of input data.");

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
    std::size_t p = serialisePrimitives(serialiser);
    std::size_t d = serialiseDynamicMembers(serialiser);
//    std::cout << "Serialised " << p  << " primitive and " << d << " dynamic bytes ("
//              << (p+d) << " total)." << std::endl;
    return p+d;
}

std::size_t MemberSerialiser::unserialiseAll(const char *serialisedData, std::size_t length)
{
    // Unserialise primitives first, then dynamic members.
    std::size_t unserialised = 0;
    std::size_t l = length;
    
    std::size_t primitives = unserialisePrimitives(serialisedData, l);
//    std::cout << "Size of primitives to unserialise: " << totalPrimitiveBytes() << " bytes, actually unserialised "
//              + std::to_string(primitives) << " bytes." << std::endl;
    unserialised += primitives;
    serialisedData += primitives;

    // This should never happen...
    assert(primitives <= l);
    l -= primitives;
    
    std::size_t dynamicMembers = unserialiseDynamicMembers(serialisedData, l);
//    std::cout << "Size of dynamic members to unserialise: " << totalDynamicBytes() << " bytes, actually unserialised "
//              + std::to_string(dynamicMembers) << " bytes." << std::endl;
    unserialised += dynamicMembers;

    // Check this elsewhere instead!
    // May not be true if superclass and subclass both need to unserialise.
//    if ( unserialised != length )
//        throw InvalidInputMemberException("Unserialising member data did not exhaust input data. (Unserialised "
//                                          + std::to_string(unserialised) + " of " + std::to_string(length) + " bytes)");

    return unserialised;
}

int MemberSerialiser::primitiveMemberCount() const
{
    return _primitives.size();
}

int MemberSerialiser::dynamicMemberCount() const
{
    return _dynamicMembers.size();
}

void MemberSerialiser::setInitialised()
{
    _initialised = true;
}

std::size_t MemberSerialiser::totalPrimitiveBytes() const
{
    std::size_t bytes = 0;
    for ( auto it = _primitives.begin(); it != _primitives.end(); ++it )
    {
        bytes += it->second;
    }
    return bytes;
}

std::size_t MemberSerialiser::totalDynamicBytes() const
{
    std::size_t bytes = 0;
    for ( auto it = _dynamicMembers.begin(); it != _dynamicMembers.end(); ++it )
    {
        bytes += (*it)->memberSize();
    }
    return bytes;
}
