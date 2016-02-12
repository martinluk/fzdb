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
			
			virtual ~Int() {}

			int32_t value() const { return _value; }

			virtual Subtype subtype() const
			{
				return Subtype::TypeInt32;
			}

			virtual std::shared_ptr<Base> Clone() override {
				return std::make_shared<Int>(_value, _confidence);
			}

			virtual std::string logString() const override
			{
				return std::string("Int(") + std::to_string(_value) + std::string(", ")
					+ std::to_string(confidence()) + std::string(")");
			}

			// Inherited via Base
			virtual bool Equals(const std::string val) const override {
				return _value == std::stoi(val);
			}

			virtual std::string toString() const override {
				return std::to_string(_value);
			}

		protected:
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialisePrimitives(serialiser);
			}

			Int(const char* &serialisedData) : Base(serialisedData)
			{
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialisePrimitives(serialisedData);
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_INT
