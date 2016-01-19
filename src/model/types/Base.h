#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../ISerialisable.h"

namespace model {
	namespace types {
		class Base : public model::ISerialisable
		{
		private:
			unsigned char _confidence;

			struct SerialHeader
			{
				std::size_t size;   // Total serialised size in bytes, including this header.
			};
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

			Base(unsigned char confidence) {
				if (confidence > 100) confidence = 100;
				_confidence = confidence;
			}

			virtual bool Equals(const std::string val) {
				return false;
			};

			virtual std::string toString() {
				return "";
			}

			unsigned char confidence() const {
				return _confidence;
			}

			void confidence(unsigned char confidence) {
				_confidence = confidence;
			}

			virtual std::size_t serialise(Serialiser &serialiser) const
			{
				std::size_t initialSize = serialiser.size();
				std::vector<Serialiser::SerialProperty> propList;

				SerialHeader header;
				header.size = 0;

				// Get the subtype - this is overridden by derived classes.
				Subtype st = subtype();

				propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
				propList.push_back(Serialiser::SerialProperty(&st, sizeof(Subtype)));
				propList.push_back(Serialiser::SerialProperty(&_confidence, sizeof(unsigned char)));

				std::size_t serialisedBytes = serialiser.serialise(propList);

				SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(initialSize);
				pHeader->size = serialisedBytes;

				return serialisedBytes;
			}

			virtual Subtype subtype() const
			{
				return Subtype::TypeUndefined;
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
