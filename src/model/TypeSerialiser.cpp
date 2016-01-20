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

        std::size_t TypeSerialiser::serialise(Serialiser &serialiser)
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

        Base* TypeSerialiser::unserialise(const char *serialisedData)
        {
            const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
            serialisedData += sizeof(SerialHeader);

            switch (pHeader->subtype)
            {
            case Base::TypeUndefined:
                return new Base(serialisedData);

            case Base::TypeInt32:
                return new Int(serialisedData);

            case Base::TypeString:
                return new String(serialisedData);

            case Base::TypeEntityRef:
                return new EntityRef(serialisedData);

            default:
                return NULL;
            }
        }
    }
}
