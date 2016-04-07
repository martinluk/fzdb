#ifndef FUZZY_MODEL_TYPES_DATE
#define FUZZY_MODEL_TYPES_DATE

#include "./Base.h"

namespace model
{
    namespace types
    {
        // Class representing a date (no time).
        // The date is stored as an integer (Date_t)
        // and this can be converted to a StructuredDate
        // (year, month, day). Ordering can also be
        // specified, eg. before/after some date.
        class Date : public Base
        {
            friend class TypeSerialiser;
            typedef unsigned long Date_t;
        public:
            enum class Ordering
            {
                Before = -1,
                EqualTo = 0,
                After = 1
            };
            
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
            
            Date() : Base(), _value(0), _order(Ordering::EqualTo)
            { }
            
            Date(Date_t value, unsigned int author, Ordering order = Ordering::EqualTo, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Base(confidence, author, comment), _value(value), _order(order)
            { }
            
            Date(const StructuredDate &sd, unsigned int author, Ordering order = Ordering::EqualTo, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Base(confidence, author, comment), _value(encode(sd)), _order(order)
            { }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const Date* d = dynamic_cast<const Date*>(other);
                assert(d);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == d->_value && _order == d->_order;
            }

            Date_t rawValue() const
            {
                return _value;
            }

            SubType subtype() const
            {
                return SubType::TypeDate;
            }
            
            StructuredDate date() const
            {
                return decode(_value);
            }
            
            Ordering ordering() const
            {
                return _order;
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

            virtual std::string logString(const Database* db =  NULL) const override
            {
                return std::string("Date(") + std::to_string(_value) + std::string(", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            virtual bool memberwiseEqual(const Base* other) const
            {
                const Date* cOther = dynamic_cast<const Date*>(other);
                return Base::memberwiseEqual(other) && cOther &&
                        _value == cOther->_value &&
                        _order == cOther->_order;
            }

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser)
            {
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

            Date(const char* &serialisedData, std::size_t length) : Base(serialisedData, length)
            {
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }

            Date_t _value;
            Ordering _order;

		private:
            MemberSerialiser _memberSerialiser;

            void initMemberSerialiser() override
            {
                _memberSerialiser.addPrimitive(&_value, sizeof(_value));
                _memberSerialiser.addPrimitive(&_order, sizeof(_order));

                _memberSerialiser.setInitialised();
            }
        };
    }
}

#endif    // FUZZY_MODEL_TYPES_DATE
