#ifndef FUZZY_MODEL_TYPES_INT
#define FUZZY_MODEL_TYPES_INT

#include <string>

#include "./Base.h"

namespace model {
	namespace types {
		class Int : public Base {
		private:
			const int32_t _value;

                        struct SerialHeader
                        {
                            std::size_t size;       // Total serialised size of this object in bytes, including base serialisation.
                        };
		public:

			Int(const int32_t value) : _value(value), Base(100) {}
			Int(unsigned char confidence, const int32_t value) : _value(value), Base(confidence) {}

			int32_t value() { return _value; }

                        virtual Subtype subtype() const
                        {
                            return TypeInt32;
                        }

                        virtual std::size_t serialise(Serialiser &serialiser) const
                        {
                            std::vector<Serialiser::SerialProperty> propList;
                            std::size_t initialSize = serialiser.size();

                            // Create a header to serialise first.
                            SerialHeader header;
                            header.size = 0;

                            // Serialise this before anything else.
                            propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
                            serialiser.serialise(propList);
                            propList.clear();

                            // Serialise the base.
                            std::size_t baseBytes = Base::serialise(serialiser);

                            // Serialise our value.
                            propList.push_back(Serialiser::SerialProperty(&_value, sizeof(int32_t)));
                            std::size_t ourBytes = serialiser.serialise(propList);
                            propList.clear();

                            // Get a pointer to the original header.
                            SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(initialSize);
                            pHeader->size = baseBytes + ourBytes;

                            return baseBytes + ourBytes;
                        }
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_INT
