#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../Serialiser.h"
#include <cassert>
#include <cstring>
#include <memory>
#include "../ILogString.h"
#include "../Triple.h"

namespace model {
	namespace types {
		class Base : public ILogString
		{
		protected:
			//friend class TypeSerialiser;
			unsigned char _confidence;

		public:

			enum class Subtype
			{
				TypeUndefined = 0,
				TypeInt32,
				TypeString,
				TypeEntityRef,

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

				default:
					return "Undefined";
				}
			}

			Base(unsigned char confidence = 100) {
				if (confidence > 100) confidence = 100;
				_confidence = confidence;
			}

			virtual std::shared_ptr<Base> Clone() {
				return std::make_shared<Base>(_confidence);
			}

			virtual bool Equals(const std::string val) {
				return false;
			}

			bool Equals(const model::Object object) {
				if (object.type == model::Object::Type::VARIABLE) return false;
				if (object.type == model::Object::Type::INT && subtype() != Subtype::TypeInt32) return false;
				if (object.type == model::Object::Type::STRING && subtype() != Subtype::TypeString) return false;
				if (object.type == model::Object::Type::ENTITYREF && subtype() != Subtype::TypeEntityRef) return false;
				return Equals(object.value);
			}

			virtual std::string toString() {
				return "";
			}

			unsigned char confidence() const {
				return _confidence;
			}

			void setConfidence(unsigned char confidence) {
				_confidence = confidence;
			}

			virtual Subtype subtype() const
			{
				return Subtype::TypeUndefined;
			}

			virtual std::string logString() const
			{
				return std::string("Base(") + std::to_string(_confidence) + std::string(")");
			}

			// Called to construct from serialised data.
			Base(const char* &serialisedData)
			{
				// The data should point to our confidence value.
				_confidence = *(reinterpret_cast<const unsigned char*>(serialisedData));

				// We have to increment the pointer (which is actually a reference to a pointer)
				// so that the next class can get its data.
				serialisedData += sizeof(unsigned char);
			}

			// Called when serialising.
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				// Serialise the confidence.
				return serialiser.serialise(Serialiser::SerialProperty(&_confidence, sizeof(unsigned char)));
			}			
		};

		class ConfidenceCompare {
		public:
			bool operator() (Base a, Base b)
			{
				return a.confidence() > b.confidence();
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_BASE
