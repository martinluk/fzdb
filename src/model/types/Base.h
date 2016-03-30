#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../Serialiser.h"
#include <cassert>
#include <cstring>
#include <memory>
#include "../ILogString.h"
#include "../Triple.h"
#include "../MemberSerialiser.h"
#include "../PropertyOwner.h"
#include "./SubType.h"

#include <boost/date_time/posix_time/posix_time.hpp>

namespace model {
    namespace types {

        // Base value type class. All other types inherit from this.
        class Base : public ILogString, public PropertyOwner
        {
            // Jonathan: this member serialiser needs to be private otherwise subclasses defining their own end up adding
            // their entries to this one!
        private:
            MemberSerialiser _memberSerialiser;

            void initMemberSerialiser()
            {
                _memberSerialiser.addPrimitive(&_confidence, sizeof(_confidence));
                _memberSerialiser.addPrimitive(&_orderingId, sizeof(_orderingId));
                _memberSerialiser.addPrimitive(&_sourceEntityId, sizeof(_sourceEntityId));
                _memberSerialiser.addPrimitive(&_originalAuthorId, sizeof(_originalAuthorId));

                _memberSerialiser.addPrimitive(&_cYearCreated, sizeof(&_cYearCreated));
                _memberSerialiser.addPrimitive(&_cMonthCreated, sizeof(&_cMonthCreated));
                _memberSerialiser.addPrimitive(&_cDayCreated, sizeof(&_cDayCreated));
                _memberSerialiser.addPrimitive(&_cHourCreated, sizeof(&_cHourCreated));
                _memberSerialiser.addPrimitive(&_cMinCreated, sizeof(&_cMinCreated));
                _memberSerialiser.addPrimitive(&_cSecCreated, sizeof(&_cSecCreated));
                _memberSerialiser.addPrimitive(&_cFracSecCreated, sizeof(&_cFracSecCreated));

                _memberSerialiser.addDynamicMember(&_commentWrapper);

                _memberSerialiser.setInitialised();
            }

            void initConvenienceMembers()
            {
                auto dateCreated = _timeCreated.date();
                auto timeCreated = _timeCreated.time_of_day();

                _cYearCreated = dateCreated.year();
                _cMonthCreated = dateCreated.month();
                _cDayCreated = dateCreated.day();
                _cHourCreated = timeCreated.hours();
                _cMinCreated = timeCreated.minutes();
                _cSecCreated = timeCreated.seconds();
                _cFracSecCreated = timeCreated.fractional_seconds();
            }

        protected:
            friend class TypeSerialiser;
            unsigned char _confidence;
            
            std::string _comment;
            MemberSerialiser::DynamicStringMember _commentWrapper;

            // JONATHAN: Un-const'd these as they don't strictly need to be const (we can just
            // not provide setter methods) and if they are it messes with the member serialiser.
            // I should fix that but we don't have time right now.

            // Entity id of source entity
            unsigned long long _sourceEntityId;

            // User who created the record
            unsigned int _originalAuthorId;

            // Time record was created
            boost::posix_time::ptime _timeCreated;

            // Convenience for serialisation.
            boost::gregorian::date::year_type _cYearCreated;
            boost::gregorian::date::month_type _cMonthCreated;
            boost::gregorian::date::day_type _cDayCreated;
            boost::posix_time::time_duration::hour_type _cHourCreated;
            boost::posix_time::time_duration::min_type _cMinCreated;
            boost::posix_time::time_duration::sec_type _cSecCreated;
            boost::posix_time::time_duration::fractional_seconds_type _cFracSecCreated;

            // id for this record - unique for entity/property/id - related to ordering
            unsigned int _orderingId;

        public:

            Base(unsigned char confidence, unsigned int author, const std::string &cmnt) :
                _comment(cmnt),
                _commentWrapper(_comment),
                _timeCreated(boost::posix_time::second_clock::universal_time()),
                _sourceEntityId(0),
                _originalAuthorId(author),
                _cYearCreated(1400),
                _cMonthCreated(1),
                _cDayCreated(1),
                _cHourCreated(0),
                _cMinCreated(0),
                _cSecCreated(0),
                _cFracSecCreated(0)
            {
                initMemberSerialiser();
                initConvenienceMembers();
                if (confidence > 100) confidence = 100;
                _confidence = confidence;
            }

            Base() : Base(100, 0, std::string()) {}
            
            virtual ~Base() {}

            virtual std::shared_ptr<Base> Clone() {
                return std::make_shared<Base>(_confidence, _originalAuthorId, _comment);
            }

            virtual bool Equals(const std::string &val) const {
                return false;
            }
            
            // This specifically should NOT compare the confidence, ordering, source, author, time of creation or comment.
            virtual bool valuesEqualOnly(const Base *other) const
            {
                return subtype() == other->subtype();
            }

            // Returns whether this value is equal to the given object.
            bool Equals(const model::Object &object) {
                if (object.type == model::Object::Type::VARIABLE) return false;
                if (object.type == model::Object::Type::INT && subtype() != SubType::TypeInt32) return false;
                if (object.type == model::Object::Type::STRING && subtype() != SubType::TypeString) return false;
                if (object.type == model::Object::Type::ENTITYREF && subtype() != SubType::TypeEntityRef) return false;
                return Equals(object.value);
            }

            // What's the string representation of this value?
            virtual std::string toString() const
            {
                return "";
            }

            unsigned char confidence() const
            {
                return _confidence;
            }

            void setConfidence(unsigned char confidence) {
                _confidence = confidence;
            }
            
            std::string comment() const
            {
                return _comment;
            }
            
            void setComment(const std::string &comment)
            {
                _comment = comment;
            }

            // Subclasses reimplement this.
            // As a base class, our type is undefined.
            virtual SubType subtype() const
            {
                return SubType::TypeUndefined;
            }

            virtual std::string logString(const Database* db = NULL) const
            {
                return std::string("Base(") + std::to_string(_confidence) + std::string(")");
            }

            void OrderingId(unsigned int id) {
                _orderingId = id;
            }

            unsigned int OrderingId() {
                return _orderingId;
            }

            // For debugging - make sure we are -exactly- the same as the other type.
            virtual bool memberwiseEqual(const Base* other) const
            {
                return subtype() == other->subtype() &&
                        _confidence == other->_confidence &&
                        _comment == other->_comment &&
                        _orderingId == other->_orderingId &&
                        _originalAuthorId == other->_originalAuthorId &&
                        _sourceEntityId == other->_sourceEntityId &&
                        _timeCreated == other->_timeCreated;
            }

        protected:
            // Called when serialising.
            virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
            {        
                return _memberSerialiser.serialiseAll(serialiser);
            }

            // Called to construct from serialised data.
            Base(const char* &serialisedData, std::size_t length) : Base(0, 0, "")
            {
                initConvenienceMembers();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };

        // Wrapper class used for comparing confidence with another type.
        template <typename T>
        class ConfidenceCompare {
        public:
            bool operator() (const std::shared_ptr<T> &a, const std::shared_ptr<T> &b) const
            {
                return a->confidence() > b->confidence();
            }
        };
        
        // Wrapper class used to check whether the value members of two types are equal.
        class ValuesEqualOnly
        {
            const Base* _ptr;
        public:
            explicit ValuesEqualOnly(const Base* ptr) : _ptr(ptr)
            {
            }
            
            bool operator () (const std::shared_ptr<Base> &a) const
            {
            return a->valuesEqualOnly(_ptr);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_BASE
