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
			/**
			*   Encodes a date
			*
			*   @param int y The year of the date to be encoded
			*   @param int m The month of the date to be encoded
			*   @param int d The day of the date to be encoded
			*   @return int The encoded date
			*/
			static int Encode(int y, int m, int d)
			{
			    m = (m + 9) % 12;
			    y = y - (m/10);
			    return (365 * y) + (y/4) - (y/100) + (y/400) + ((m*306 + 5) / 10) + (d-1);
			}
			
			/**
			*   Decodes a date
			*
			*   @param int g An encoded date
			*   @param int y Year output
			*	@param int mm Month output
			*	@prarm int dd Day output
			*/
			static void Decode(int g, int &y, int &mm, int &dd)
			{
			    y = ((10000*g + 14780)/3652425);
			    int ddd = g - (365*y + (y/4) - (y/100) + (y/400));
			    if (ddd < 0)
				{
			        y = y - 1;
			        ddd = g - (365*y + (y/4) - (y/100) + (y/400));
			    }
			    int mi = ((100*ddd + 52)/3060);
			    mm = (mi + 2)%12 + 1;
			    y = y + ((mi + 2)/12);
			    dd = ddd - ((mi*306 + 5)/10) + 1;
			}
			
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
