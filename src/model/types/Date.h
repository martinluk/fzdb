#ifndef FUZZY_MODEL_TYPES_DATE
#define FUZZY_MODEL_TYPES_DATE

#include "./Base.h"

namespace model
{
	namespace types
	{
		class Date : public Base
		{
			friend class TypeSerialiser;
			typedef unsigned long Date_t;
			
			Date_t _value;
			MemberSerialiser _memberSerialiser;

			void initMemberSerialiser()
			{
				_memberSerialiser.addPrimitive(&_value, sizeof(_value));
			}

		public:
			Date() : _value(0), Base(100)
			{
				initMemberSerialiser();
			}
			
			Date(Date_t value, unsigned char confidence = 100) : Base(confidence), _value(value)
			{
				initMemberSerialiser();
			}

			Date_t rawValue() const
			{
				return _value;
			}
			
			virtual ~Date() {}

			virtual std::string toString() const override
			{
				return std::to_string(_value);
			}

			virtual bool Equals(const std::string val) const override
			{
				return _value == std::stoul(val);
			}

			virtual std::string logString() const override
			{
				return std::string("Date(") + std::to_string(_value) + std::string(", ")
					+ std::to_string(confidence()) + std::string(")");
			}

		private:
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialisePrimitives(serialiser);
			}

			Date(const char* &serialisedData)
			{
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialisePrimitives(serialisedData);
			}
		};
	}
}

#endif	// FUZZY_MODEL_TYPES_DATE
