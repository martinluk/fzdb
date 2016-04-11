#ifndef FUZZY_MODEL_TYPES_DATE
#define FUZZY_MODEL_TYPES_DATE

#include "./Base.h"
#include <stdexcept>
#include "../Util.h"

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

                std::string toString() const
                {
                    return (day < 9 ? std::string("0") : std::string("")) + std::to_string(day)
                            + (month < 9 ? std::string("/0") : std::string("/")) + std::to_string(month)
                            + (year < 999 ? std::string("/0") : std::string("/"))
                            + (year < 99 ? std::string("0") : std::string(""))
                            + (year < 9 ? std::string("0") : std::string("")) + std::to_string(year);
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
                int y = (((10000*((unsigned long long)g) + 14780)/3652425));
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

            class InvalidDateException : public std::invalid_argument
            {
            public:
                InvalidDateException(const std::string date, const std::string &msg) :
                    std::invalid_argument("Date \"" + date + "\" invalid: " + msg)
                {
                }
            };

            static StructuredDate parseDateString(const std::string &str)
            {
                // TODO: At some point it would be nice to have fuller date validation,
                // ie. checking whether the given day is valid for the given year in the
                // case of leap years, etc. We do, however, assume that the string
                // passed in matches the data regex within the parser.
                // For now, we make do with the following extra validation.

                std::vector<std::string> sections = util::split(str, '/');
                if ( sections.size() != 3 )
                    throw InvalidDateException(str, "Expected day/month/year format.");

                int day = std::stoi(sections.at(0));
                int month = std::stoi(sections.at(1));
                int year = std::stoi(sections.at(2));

                assert(day >= 1 && day <= 31);
                assert(month >= 1 && month <= 12);
                assert(year >= 0 && year <= 9999);

                switch (month)
                {
                // Thirty days has:
                case 9:     // September,
                case 4:     // April,
                case 6:     // June and
                case 11:    // November
                    if ( day > 30 )
                        throw InvalidDateException(str, "Invalid day for given month.");
                    break;

                // Febrary, you weirdo.
                case 2:
                    if ( day > 29 )
                        throw InvalidDateException(str, "Invalid day for given month.");
                    break;

                // 31 days for the rest. This is caught by the parser regex.
                default:
                    break;
                }

                // Because the following is awkward to do with the regex:
                if ( year < 1 )
                    throw InvalidDateException(str, "Invalid year.");

                return StructuredDate(year, month, day);
            }
            
            Date() : Base(), _value(0), _order(Ordering::EqualTo)
            { }
            
            Date(Date_t value, Ordering order = Ordering::EqualTo) :

            Base(), _value(value), _order(order)
            { }

            
            Date(const StructuredDate &sd, Ordering order = Ordering::EqualTo) :
                Base(), _value(encode(sd)), _order(order)
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
                return SubType::Date;
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
                return date().toString();
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

			virtual std::shared_ptr<Base> Clone() override {
				auto cloned = std::make_shared<Date>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
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
