#include "TypeSerialiser.h"
#include "types/Base.h"
#include "types/Int.h"
#include "types/String.h"
#include "types/EntityRef.h"

using namespace model::types;
struct SerialHeader
{
    std::size_t size;
    Base::Subtype subtype;
};

TypeSerialiser::TypeSerialiser(const std::shared_ptr<Base> type) : baseType_(type)
{
}

std::size_t TypeSerialiser::serialise(Serialiser &serialiser) const
{
    std::size_t initialSize = serialiser.size();
    SerialHeader header;
    memset(&header, 0, sizeof(SerialHeader));
    header.size = 0;
    header.subtype = baseType_->subtype();

    std::size_t bytesSerialised = serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
    bytesSerialised += baseType_->serialiseSubclass(serialiser);

    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(initialSize);
    pHeader->size = bytesSerialised;

    return bytesSerialised;
	return 0;
}

std::shared_ptr<Base> TypeSerialiser::unserialise(const char* serialisedData, std::size_t* advance)
{
			
    const char* begin = serialisedData;

    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
    serialisedData += sizeof(SerialHeader);
	std::shared_ptr<Base> b;

    switch (pHeader->subtype)
    {
    case Base::Subtype::TypeUndefined:
        b = std::make_shared<Base>(serialisedData);
        break;

    case Base::Subtype::TypeInt32:
        b = std::make_shared<Int>(serialisedData);
        break;

    case Base::Subtype::TypeString:
        b = std::make_shared<String>(serialisedData);
        break;

    case Base::Subtype::TypeEntityRef:
        b = std::make_shared<EntityRef>(serialisedData);
        break;

    default:
        b = NULL;
        break;
    }

    if ( advance )
    {
        *advance = serialisedData - begin;
    }

    return b;
}
