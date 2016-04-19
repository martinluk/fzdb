#ifndef FUZZY_MODEL_TYPES_TIMESTAMP
#define FUZZY_MODEL_TYPES_TIMESTAMP

#include <string>

#include "./base.h"
#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>

namespace model {
    namespace types {

        // Stores an integer value.
        class TimeStamp : public Base {
        private:
            friend class TypeSerialiser;

			// Time record was created
			boost::posix_time::ptime _value;

			// Convenience for serialisation.
			boost::gregorian::date::year_type _cYearCreated;
			boost::gregorian::date::month_type _cMonthCreated;
			boost::gregorian::date::day_type _cDayCreated;
			boost::posix_time::time_duration::hour_type _cHourCreated;
			boost::posix_time::time_duration::min_type _cMinCreated;
			boost::posix_time::time_duration::sec_type _cSecCreated;
			boost::posix_time::time_duration::fractional_seconds_type _cFracSecCreated;

            MemberSerialiser _memberSerialiser;
            
            void initMemberSerialiser()
            {
                //_memberSerialiser.addPrimitive(&_value, sizeof(_value));

                _memberSerialiser.setInitialised();
            }

        public:
			TimeStamp() : Base(), _value(boost::posix_time::second_clock::universal_time()), _cYearCreated(1400),
				_cMonthCreated(1),
				_cDayCreated(1),
				_cHourCreated(0),
				_cMinCreated(0),
				_cSecCreated(0),
				_cFracSecCreated(0)
            {
            }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const TimeStamp* i = dynamic_cast<const TimeStamp*>(other);
                assert(i);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == i->_value;
            }
            
            virtual ~TimeStamp() {}

			boost::posix_time::ptime value() const { return _value; }

            virtual SubType subtype() const
            {
                return SubType::TimeStamp;
            }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<TimeStamp>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("Int(") + toString() + std::string(", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            // Inherited via Base
            virtual unsigned char Equals(const std::string &val) const override {				
				return _value == boost::posix_time::time_from_string(val) ? 100 : 0;
            }

            virtual std::string toString() const override {
				return boost::posix_time::to_simple_string(_value);
            }

            virtual bool memberwiseEqual(const Base* other) const
            {
                const TimeStamp* cOther = dynamic_cast<const TimeStamp*>(other);
                return Base::memberwiseEqual(other) && cOther &&
                        _value == cOther->_value;
            }

			unsigned char confidence() const override {
				return 100;
			}

			//timestamps don't have any metadata
			void setupDefaultMetaData(const unsigned char confidence) override {}

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser) 
            {
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

			TimeStamp(const char* &serialisedData, std::size_t length) : Base(serialisedData, length), _cYearCreated(1400),
				_cMonthCreated(1),
				_cDayCreated(1),
				_cHourCreated(0),
				_cMinCreated(0),
				_cSecCreated(0),
				_cFracSecCreated(0)
            {
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_TIMESTAMP
