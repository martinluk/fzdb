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
			class StructuredDate
			{
			public:
				explicit StructuredDate(int y, int m, int d) :
					year(y), month(m), day(d)
				{
				}
				
				int year;
				int month;
				int day;
			};

			static Date_t encode(const StructuredDate &sd)
			{
				int d = sd.day;
				int m = sd.month;
				int y = sd.year;
				
			    m = (m + 9) % 12;
			    y = y - (m/10);
			    return (365 * y) + (y/4) - (y/100) + (y/400) + ((m*306 + 5) / 10) + (d-1);
			}
			
			static StructuredDate decode(Date_t g)
			{
			    int y = ((10000*g + 14780)/3652425);
			    int ddd = g - (365*y + (y/4) - (y/100) + (y/400));
			    if (ddd < 0)
				{
			        y = y - 1;
			        ddd = g - (365*y + (y/4) - (y/100) + (y/400));
			    }
			    int mi = ((100*ddd + 52)/3060);
			    int mm = (mi + 2)%12 + 1;
			    y = y + ((mi + 2)/12);
			    int dd = ddd - ((mi*306 + 5)/10) + 1;
				
				return StructuredDate(y, mm, dd);
			}
			
			Date() : _value(0), Base(100, std::string())
			{
				initMemberSerialiser();
			}
			
			Date(Date_t value, unsigned char confidence = 100, const std::string &comment = std::string()) :
				Base(confidence, comment), _value(value)
			{
				initMemberSerialiser();
			}
			
			Date(const StructuredDate &sd, unsigned char confidence = 100, const std::string &comment = std::string()) :
				Base(confidence, comment), _value(encode(sd))
			{
				initMemberSerialiser();
			}

			Date_t rawValue() const
			{
				return _value;
			}
			
			StructuredDate date() const
			{
				return decode(_value);
			}
			
			virtual ~Date() {}

			virtual std::string toString() const override
			{
				return std::to_string(_value);
			}

			virtual bool Equals(const std::string &val) const override
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
				return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
			}

			Date(const char* &serialisedData) : Base(serialisedData)
			{
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialiseAll(serialisedData);
			}
		};
	}
}

#endif	// FUZZY_MODEL_TYPES_DATE
