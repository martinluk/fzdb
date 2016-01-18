#ifndef FUZZY_MODEL_TYPES_ENTITY
#define FUZZY_MODEL_TYPES_ENTITY

#include <string>

#include "./Base.h"

typedef unsigned long long EHandle_t;

namespace model {
	namespace types {
		class EntityRef : public Base {
		private:
                        EHandle_t _value;
		public:

			EntityRef(const EHandle_t value) : _value(value), Base(100) {}
			EntityRef(unsigned char confidence, const EHandle_t value) : _value(value), Base(confidence) {}

			EHandle_t value() { return _value; }

                        virtual Subtype subtype() const
                        {
                            return TypeEntityRef;
                        }

                        virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
                        {
                            return Base::serialiseSubclass(serialiser)
                                    + serialiser.serialise(Serialiser::SerialProperty(&_value, sizeof(EHandle_t)));
                        }

                        virtual std::string logString() const
                        {
                            return std::string("EntityRef(\"") + std::to_string(_value) + std::string("\",")
                                    + std::to_string(confidence()) + std::string(")");
                        }

                protected:
                        EntityRef(const char* &serialisedData) : Base(serialisedData)
                        {
                            _value = *(reinterpret_cast<const EHandle_t*>(serialisedData));
                            serialisedData += sizeof(_value);
                        }
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_ENTITY
