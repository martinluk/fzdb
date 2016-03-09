#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../Serialiser.h"
#include <cassert>
#include <cstring>
#include <memory>
#include "../ILogString.h"
#include "../Triple.h"
#include "../MemberSerialiser.h"

namespace model {
	namespace types {
		class Base : public ILogString
		{
		protected:
			friend class TypeSerialiser;
			unsigned char _confidence;
			
			std::string _comment;
			MemberSerialiser::DynamicStringMember _commentWrapper;
			
			MemberSerialiser _memberSerialiser;
			
			void initMemberSerialiser()
			{
				_memberSerialiser.addPrimitive(&_confidence, sizeof(_confidence));
				_memberSerialiser.addDynamicMember(&_commentWrapper);
			}

		public:

			enum class Subtype
			{
				TypeUndefined = 0,
				TypeInt32,
				TypeString,
				TypeEntityRef,
				TypeDate,

				//Doesn't have an associated type
				PropertyReference
			};
			
            // TODO: FIX CPP FILES NOT BEING READ FROM THIS FOLDER.
            // Until then, we use an ugly workaround.
            //static const char* SubtypeString[];
			
            static const char* SubtypeString(Subtype type)
            {
                switch (type)
                {
                case Subtype::TypeInt32:
                    return "Int32";
                    
                case Subtype::TypeString:
                    return "String";
                    
                case Subtype::TypeEntityRef:
                    return "EntityRef";
                    
                case Subtype::PropertyReference:
                    return "PropertyReference";

				case Subtype::TypeDate:
					return "Date";
                    
                default:
                    return "Undefined";
                }
            }

			Base(unsigned char confidence, const std::string &cmnt) : _comment(cmnt), _commentWrapper(_comment)
			{
				initMemberSerialiser();
				if (confidence > 100) confidence = 100;
				_confidence = confidence;
			}
			
			virtual ~Base() {}

			virtual std::shared_ptr<Base> Clone() {
				return std::make_shared<Base>(_confidence, _comment);
			}

			virtual bool Equals(const std::string &val) const {
				return false;
			}
			
			virtual bool equalsIgnoreConfidence(const Base *other) const
			{
			    return subtype() == other->subtype();
			}

			// TODO: Shouldn't this be virtual?
			bool Equals(const model::Object &object) {
				if (object.type == model::Object::Type::VARIABLE) return false;
				if (object.type == model::Object::Type::INT && subtype() != Subtype::TypeInt32) return false;
				if (object.type == model::Object::Type::STRING && subtype() != Subtype::TypeString) return false;
				if (object.type == model::Object::Type::ENTITYREF && subtype() != Subtype::TypeEntityRef) return false;
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

			virtual Subtype subtype() const
			{
				return Subtype::TypeUndefined;
			}

			virtual std::string logString() const
			{
				return std::string("Base(") + std::to_string(_confidence) + std::string(")");
			}

		protected:
			// Called when serialising.
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{		
				return _memberSerialiser.serialiseAll(serialiser);
			}

			// Called to construct from serialised data.
            Base(const char* &serialisedData, std::size_t length) : _confidence(0), _comment(), _commentWrapper(_comment)
			{
				initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
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
		
		template<typename T>
		class EqualsIgnoreConfidence
		{
		    const T* _ptr;
		public:
		    explicit EqualsIgnoreConfidence(const T* ptr) : _ptr(ptr)
		    {
		    }
		    
		    bool operator () (const std::shared_ptr<T> &a) const
		    {
			return a->equalsIgnoreConfidence(_ptr);
		    }
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_BASE
