#ifndef FUZZY_MODEL_TYPES_INT
#define FUZZY_MODEL_TYPES_INT

#include <string>

#include "./Base.h"

namespace model {
	namespace types {
		class Int : public Base {
		private:
                    friend class TypeSerialiser;
                        int32_t _value;

		public:
                        Int(const int32_t value) : _value(value), Base(100) {}
                        Int(int32_t value, unsigned char confidence) : Base(confidence), _value(value) {}

			int32_t value() { return _value; }

                        virtual Subtype subtype() const
                        {
                            return TypeInt32;
                        }

                        virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
                        {
                            return Base::serialiseSubclass(serialiser)
                                    + serialiser.serialise(Serialiser::SerialProperty(&_value, sizeof(int32_t)));
                        }

                        virtual std::string logString() const
                        {
                            return std::string("Int(") + std::to_string(_value) + std::string(",")
                                    + std::to_string(confidence()) + std::string(")");
                        }

                protected:
                        Int(const char* &serialisedData) : Base(serialisedData)
                        {
                            // Base will have incremented the pointer appropriately.
                            // We can now copy in our value.
                            _value = *(reinterpret_cast<const int32_t*>(serialisedData));
                            serialisedData += sizeof(int32_t);
                        }
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_INT
