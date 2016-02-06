#include "TypeSerialiser.h"
#include "types/Base.h"
#include "types/Int.h"
#include "types/String.h"
#include "types/EntityRef.h"
#include "types/Date.h"
#include "spdlog/spdlog.h"
#include <cassert>

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
			Serialiser::zeroBuffer(&header, sizeof(SerialHeader));
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
			const char* d = serialisedData;

			const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(d);
			//spdlog::get("main")->info("Size: {} Subtype: {}", pHeader->size, (int)pHeader->subtype);
			d += sizeof(SerialHeader);
            Base* b = NULL;

			switch (pHeader->subtype)
            {
            case Base::Subtype::TypeUndefined:
				b = new Base(d);
                break;

            case Base::Subtype::TypeInt32:
				b = new Int(d);
                break;

            case Base::Subtype::TypeString:
				b = new String(d);
                break;

            case Base::Subtype::TypeEntityRef:
				b = new EntityRef(d);
                break;

						case Base::Subtype::TypeDate:
						b = new Date(d);

            default:
						    assert(false);
                b = NULL;
                break;
            }

            if ( advance )
            {
				*advance = d - serialisedData;
				//spdlog::get("main")->info("Base: {} Result: {} Advance: {}", (unsigned long)serialisedData, (unsigned long)d, *advance);
				assert(*advance == pHeader->size);
            }

            return b;
        }
    }
}
