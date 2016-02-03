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
			MemberSerialiser _memberSerialiser;
			
			void initMemberSerialiser()
			{
				_memberSerialiser.addPrimitive(&_value, sizeof(_value));
			}

		public:
			Int() : _value(0), Base(100)
			{
				initMemberSerialiser();
			}
			
			Int(const int32_t value) : _value(value), Base(100)
			{
				initMemberSerialiser();
			}
			
			Int(int32_t value, unsigned char confidence) : Base(confidence), _value(value)
			{
				initMemberSerialiser();
			}
			
			Int(std::string value, unsigned char confidence) : Int(std::atoi(value.c_str()), confidence)
			{
				// Already initialised
			}

			int32_t value() { return _value; }

			virtual Subtype subtype() const
			{
				return Subtype::TypeInt32;
			}

			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				//return Base::serialiseSubclass(serialiser)
				//	+ serialiser.serialise(Serialiser::SerialProperty(&_value, sizeof(int32_t)));
				
				return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialisePrimitives(serialiser);
			}

			virtual std::string logString() const
			{
				return std::string("Int(") + std::to_string(_value) + std::string(", ")
					+ std::to_string(confidence()) + std::string(")");
			}

			// Inherited via Base
			virtual bool Equals(const std::string val) override {
				return _value == std::stoi(val);
			}

			virtual std::string toString() override {
				return std::to_string(_value);
			}

		protected:
			Int(const char* &serialisedData) : Base(serialisedData)
			{
				// Base will have incremented the pointer appropriately.
				// We can now copy in our value.
				//_value = *(reinterpret_cast<const int32_t*>(serialisedData));
				//serialisedData += sizeof(int32_t);
				
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialisePrimitives(serialisedData);
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_INT
