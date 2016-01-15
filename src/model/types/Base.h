#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../Serialiser.h"
#include <cassert>
#include <cstring>
#include "../ILogString.h"

namespace model {
	namespace types {
                class Base : public ILogString
                {
		private:
                    friend class TypeSerialiser;
                    unsigned char _confidence;
		public:
                        enum Subtype
                        {
                            TypeUndefined = 0,
                            TypeInt32,
                            TypeString,
                            TypeEntityRef
                        };

			Base(unsigned char confidence) {
				if (confidence > 100) confidence = 100;
				_confidence = confidence;
			}

			unsigned char confidence() const {
				return _confidence;
			}

                        void setConfidence(unsigned char confidence) {
				_confidence = confidence;
			}

                        virtual Subtype subtype() const
                        {
                            return TypeUndefined;
                        }

                        virtual std::string logString() const
                        {
                            return std::string("Base(") + std::to_string(_confidence) + std::string(")");
                        }

                protected:
                        // Called when serialising.
                        virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
                        {
                            // Serialise the confidence.
                            return serialiser.serialise(Serialiser::SerialProperty(&_confidence, sizeof(unsigned char)));
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
