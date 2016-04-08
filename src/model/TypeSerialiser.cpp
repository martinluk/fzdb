#include "TypeSerialiser.h"
#include "types/Base.h"
#include "types/Int.h"
#include "types/String.h"
#include "types/EntityRef.h"
#include "types/Date.h"
#include "spdlog/spdlog.h"
#include <cassert>
#include <stdexcept>

using namespace model::types;
struct SerialHeader
{
    std::size_t size;
    SubType subtype;
};

TypeSerialiser::TypeSerialiser(const std::shared_ptr<Base> &type) : baseType_(type)
{
}

std::size_t TypeSerialiser::serialise(Serialiser &serialiser) const
{
    std::size_t initialSize = serialiser.size();
    SerialHeader header;
    Serialiser::zeroBuffer(&header, sizeof(SerialHeader));
    header.size = 0;
    header.subtype = baseType_->subtype();

    std::size_t bytesSerialised = serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
    bytesSerialised += baseType_->serialiseSubclass(serialiser);
    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(initialSize);
    pHeader->size = bytesSerialised;

    return bytesSerialised;
}

std::shared_ptr<Base> TypeSerialiser::unserialise(const char* serialisedData, std::size_t* advance)
{
    const char* d = serialisedData;

    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(d);
    d += sizeof(SerialHeader);
    std::shared_ptr<Base> b;
    std::size_t dataSize = pHeader->size - sizeof(SerialHeader);

    // Since TypeSerialiser is the friend class of the subtypes,
    // we need to use the shared_ptr constructor directly instead
    // of make_shared(). The latter won't work because that would
    // require the function to be a friend of the classes, which
    // it's not.
    const char* dBefore = d;
    switch (pHeader->subtype)
    {
//    case SubType::Undefined:
//        //b = std::make_shared<Base>(d);
//        b = std::shared_ptr<Base>(new Base(d, dataSize));
//                break;

    case SubType::Int32:
        //b = std::make_shared<Int>(d);
        b = std::shared_ptr<Int>(new Int(d, dataSize));
                break;

    case SubType::String:
        //b = std::make_shared<String>(d);
        b = std::shared_ptr<String>(new String(d, dataSize));
                break;

    case SubType::EntityRef:
        //b = std::make_shared<EntityRef>(d);
        b = std::shared_ptr<EntityRef>(new EntityRef(d, dataSize));
                break;
        
    case SubType::Date:
        //b = std::make_shared<Date>(d);
        b = std::shared_ptr<Date>(new Date(d, dataSize));
        break;

    default:
        assert(false);
        break;
    }

    if ( (d - dBefore) != dataSize )
    {
        throw std::runtime_error("Expected unserialisation of " + std::to_string(dataSize)
                                 + " bytes but got " + std::to_string((std::size_t)(d - dBefore)) + " instead.");
    }

    if ( advance )
    {
        *advance = d - serialisedData;
    }

    return b;
}
