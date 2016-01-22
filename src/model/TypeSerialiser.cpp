#include "TypeSerialiser.h"
#include "types/Base.h"
#include "types/Int.h"
#include "types/String.h"
#include "types/EntityRef.h"

namespace model
{
    namespace types
    {
        struct SerialHeader
        {
            std::size_t size;
            Base::Subtype subtype;
        };

        TypeSerialiser::TypeSerialiser(const Base *type) : baseType_(type)
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
        }

        Base* TypeSerialiser::unserialise(const char* serialisedData, std::size_t* advance)
        {
            const char* begin = serialisedData;

            const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
            serialisedData += sizeof(SerialHeader);
            Base* b = NULL;

            switch (pHeader->subtype)
            {
            case Base::Subtype::TypeUndefined:
                b = new Base(serialisedData);
                break;

            case Base::Subtype::TypeInt32:
                b = new Int(serialisedData);
                break;

            case Base::Subtype::TypeString:
                b = new String(serialisedData);
                break;

            case Base::Subtype::TypeEntityRef:
                b = new EntityRef(serialisedData);
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
    }
}
