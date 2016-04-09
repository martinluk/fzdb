#include "StringMapSerialiser.h"
#include <cstring>
#include <cassert>

#include "spdlog/spdlog.h"

struct SerialHeader
{
    std::size_t size;    // Total serialised size in bytes.
    std::size_t count;    // Number of headers to expect after this header.
};

struct EntryHeader
{
    std::size_t offset;        // Offset from the beginning of the entire serialisation.
    std::size_t stringSize;    // Serialised size of the string.
};


StringMapSerialiser::StringMapSerialiser(NameManager* nameManager) : _nameManager(nameManager)
{
}

std::size_t StringMapSerialiser::serialise(Serialiser &serialiser) const
{
    assert( _nameManager);
    std::size_t origSize = serialiser.size();

    // Create a header.
    SerialHeader header;
    Serialiser::zeroBuffer(&header, sizeof(SerialHeader));
    header.size = 0;    // We don't know this yet.
    header.count = _nameManager->size();

    std::vector<Serialiser::SerialProperty> propList;
    propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

    // Push back the appropriate amount of dummy entry headers.
    EntryHeader eHeader;
    Serialiser::zeroBuffer(&eHeader, sizeof(EntryHeader));
    for ( int i = 0; i < header.count; i++ )
    {
        propList.push_back(Serialiser::SerialProperty(&eHeader, sizeof(EntryHeader)));
    }

    // Serialise all of these properties.
    serialiser.serialise(propList);
    std::size_t dataBegin = serialiser.size();
    std::size_t headerChunkSize = dataBegin - origSize;

    // Serialise each entry.
    std::size_t bytesSerialised = 0;
    int i = 0;

    auto lambda = [&] (const std::string &first, unsigned int second)
    {
        std::size_t ourOffset = serialiser.size() - origSize;
        std::size_t serialisedThisLoop = serialiser.serialise(Serialiser::SerialProperty(&second, sizeof(unsigned int)));

        // We use size+1 to ensure that a null terminator is present in the serialised stream.
        char* buffer = new char[first.size() + 1];
        memset(buffer, 0, first.size()+1);
        memcpy(buffer, first.c_str(), first.size());
        serialisedThisLoop += serialiser.serialise(Serialiser::SerialProperty(buffer, first.size()+1));
        delete buffer;

        EntryHeader* pHeader = serialiser.reinterpretCast<EntryHeader*>(origSize + sizeof(SerialHeader));
        pHeader += i;
        pHeader->stringSize = first.size()+1;
        pHeader->offset = ourOffset;

        bytesSerialised += serialisedThisLoop;
        i++;
    };

    for( auto it = _nameManager->_stringToIdMap.begin(); it != _nameManager->_stringToIdMap.end(); ++it )
    {
        lambda(it->first, it->second);
    }

    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(origSize);
    pHeader->size = headerChunkSize + bytesSerialised;
    assert(pHeader->size == serialiser.size() - origSize);

    return pHeader->size;
}

void StringMapSerialiser::unserialise(const char *serialisedData, std::size_t length)
{
    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
    if ( pHeader->size != length )
    {
        throw InvalidInputStringTableException("Internal string table size does not match size of data (recorded length is "
                                               + std::to_string(pHeader->size) + " but length passed in is " + std::to_string(length) + ").");
    }

    const EntryHeader* eHeader = reinterpret_cast<const EntryHeader*>(serialisedData + sizeof(SerialHeader));
    for ( int i = 0; i < pHeader->count; i++ )
    {
        const EntryHeader* e = &(eHeader[i]);
        if ( (const char*)e - serialisedData >= length )
            throw InvalidInputStringTableException("Header start for string " + std::to_string(i) + " exceeds length of input data.");
        
        const char* data = serialisedData + e->offset;
        if ( data - serialisedData >= length )
            throw InvalidInputStringTableException("Data start for string " + std::to_string(i) + " exceeds length of input data.");
        
        if ( e->offset + e->stringSize > length )
            throw InvalidInputStringTableException("Length of string " + std::to_string(i) + " exceeds length of input data.");
        
        if ( e->stringSize > 0 && *(data + sizeof(int) + e->stringSize - 1) != '\0' )
            throw InvalidInputStringTableException("String " + std::to_string(i) + " is not null-terminated.");
        
        const unsigned int* id = reinterpret_cast<const unsigned int*>(data);
        data += sizeof(unsigned int);

		_nameManager->add(std::string(data));
    }
}
