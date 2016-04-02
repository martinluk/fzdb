#include "PrimitiveMapSerialiser.h"
#include "types/SubType.h"
#include <cassert>
#include <string>

struct SerialHeader
{
    std::size_t size;           // Total serialised size in bytes.
    std::size_t primitiveASize; // Size in bytes of primitive A.
    std::size_t primitiveBSize; // Size in bytes of primitive B.
    std::size_t count;          // Number of serialised pairs.
};

template<typename A, typename B>
std::size_t PrimitiveMapSerialiser<A,B>::serialise(Serialiser &serialiser) const
{
    std::size_t origSize = serialiser.size();

    // Create a header.
    SerialHeader header;
    Serialiser::zeroBuffer(&header, sizeof(SerialHeader));
    header.count = _map.size();
    header.primitiveASize = sizeof(A);
    header.primitiveBSize = sizeof(B);
    header.size = sizeof(SerialHeader) + (header.count * (header.primitiveASize + header.primitiveBSize));

    serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

    // Serialise each entry.
    for ( auto it = _map.cbegin(); it != _map.cend(); ++it )
    {
        A a = it->first;
        B b = it->second;
        serialiser.serialise(Serialiser::SerialProperty(&a, sizeof(A)));
        serialiser.serialise(Serialiser::SerialProperty(&b, sizeof(B)));
    }

    assert(serialiser.size() - origSize == header.size);

    return header.size;
}

template<typename A, typename B>
void PrimitiveMapSerialiser<A,B>::unserialise(const char *data, std::size_t length)
{
    _map.clear();

    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(data);
    const char* dataBegin = data + sizeof(SerialHeader);
    if ( dataBegin - data > length )
        throw InvalidPrimitiveMapInputException("Input data is not long enough to contain primitive map header.");

    std::size_t stride = pHeader->primitiveASize + pHeader->primitiveBSize;

    for ( int i = 0; i < pHeader->count; i++ )
    {
        const char* d = dataBegin + (i * stride);
        if ( d - data >= length || (d + stride) - data > length )
            throw InvalidPrimitiveMapInputException("Data item " + std::to_string(i) + " exceeded length of data.");

        A a;
        B b;
        memcpy(&a, d, pHeader->primitiveASize);
        memcpy(&b, d + pHeader->primitiveASize, pHeader->primitiveBSize);
        _map.insert(std::pair<A,B>(a,b));
    }
}

template class PrimitiveMapSerialiser<unsigned int, model::types::SubType>;
