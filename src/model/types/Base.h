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

		class Base : public ILogString, public PropertyOwner
		{
		protected:
			friend class TypeSerialiser;
			unsigned char _confidence;
			
			std::string _comment;
			MemberSerialiser::DynamicStringMember _commentWrapper;
			
			MemberSerialiser _memberSerialiser;

			// Entity id of source entity
			const unsigned long long _sourceEntityId;

			// User who created the record
			const unsigned int _originalAuthorId;

			// Time record was created
			const boost::posix_time::ptime _timeCreated;

			// id for this record - unique for entity/property/id - related to ordering
			unsigned int _orderingId;
		
			void initMemberSerialiser()
			{
				_memberSerialiser.addPrimitive(&_confidence, sizeof(_confidence));
				_memberSerialiser.addPrimitive(&_orderingId, sizeof(_orderingId));
				_memberSerialiser.addDynamicMember(&_commentWrapper);
			}

		public:
			
            // TODO: FIX CPP FILES NOT BEING READ FROM THIS FOLDER.
            // Until then, we use an ugly workaround.
            //static const char* SubtypeString[];
			
            static const char* SubtypeString(SubType type)
            {
                switch (type)
                {
                case SubType::TypeInt32:
                    return "Int32";
                    
                case SubType::TypeString:
                    return "String";
                    
                case SubType::TypeEntityRef:
                    return "EntityRef";
                    
                case SubType::PropertyReference:
                    return "PropertyReference";

				case SubType::TypeDate:
					return "Date";
                    
                default:
                    return "Undefined";
                }
            }

			Base(unsigned char confidence, unsigned int author, const std::string &cmnt) :
				_comment(cmnt),
				_commentWrapper(_comment),
				_timeCreated(boost::posix_time::second_clock::universal_time()),
				_sourceEntityId(0),
				_originalAuthorId(author)
			{
				initMemberSerialiser();
				if (confidence > 100) confidence = 100;
				_confidence = confidence;
			}
			
			virtual ~Base() {}

			virtual std::shared_ptr<Base> Clone() {
				return std::make_shared<Base>(_confidence, _originalAuthorId, _comment);
			}

			virtual bool Equals(const std::string &val) const {
				return false;
			}

			// TODO: Shouldn't this be virtual?
			bool Equals(const model::Object &object) {
				if (object.type == model::Object::Type::VARIABLE) return false;
				if (object.type == model::Object::Type::INT && subtype() != SubType::TypeInt32) return false;
				if (object.type == model::Object::Type::STRING && subtype() != SubType::TypeString) return false;
				if (object.type == model::Object::Type::ENTITYREF && subtype() != SubType::TypeEntityRef) return false;
				return Equals(object.value);
			}

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

			virtual SubType subtype() const
			{
				return SubType::TypeUndefined;
			}

			virtual std::string logString() const
			{
				return std::string("Base(") + std::to_string(_confidence) + std::string(")");
			}

			void OrderingId(unsigned int id) {
				_orderingId = id;
			}

			unsigned int OrderingId() {
				return _orderingId;
			}

		protected:
			// Called when serialising.
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{		
				return _memberSerialiser.serialiseAll(serialiser);
			}

			// Called to construct from serialised data.
			Base(const char* &serialisedData) : Base(0, 0, "")
			{
				serialisedData += _memberSerialiser.unserialiseAll(serialisedData);
			}
		};

		template <typename T>
		class ConfidenceCompare {
		public:
			bool operator() (const std::shared_ptr<T> &a, const std::shared_ptr<T> &b) const
			{
				return a->confidence() > b->confidence();
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_BASE
