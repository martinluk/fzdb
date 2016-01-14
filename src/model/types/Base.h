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
			unsigned char _confidence;
		public:

                        enum Subtype
                        {
                            TypeUndefined = 0,
                            TypeInt32,
                            TypeString,
                            TypeEntityRef
                        };

                        class TypeSerialiser
                        {
                        public:
                            TypeSerialiser(const Base* t) : baseType_(t) {}

                            struct SerialHeader
                            {
                                std::size_t size;   // Total serialised size, in bytes.
                                Subtype subtype;    // Subtype identifier for this type.
                            };

                            static Subtype identify(const char* serialisedData)
                            {
                                return reinterpret_cast<const SerialHeader*>(serialisedData)->subtype;
                            }

                            std::size_t serialise(Serialiser &serialiser) const
                            {
                                std::size_t initialSize = serialiser.size();
                                SerialHeader header;

                                // Things I learned today: the size of a struct is sometimes padded to
                                // make byte alignment better. This means that if we don't zero out the
                                // struct initially, we might sometimes get trash at the end of it which
                                // is confusing.
                                memset(&header, 0, sizeof(SerialHeader));

                                header.size = 0;
                                header.subtype = baseType_->subtype();

                                Serialiser::SerialProperty prop(&header, sizeof(SerialHeader));
                                std::size_t baseBytes = serialiser.serialise(prop);
                                std::size_t derivedBytes = baseType_->serialiseSubclass(serialiser);

                                serialiser.reinterpretCast<SerialHeader*>(initialSize)->size = baseBytes + derivedBytes;
                                return baseBytes + derivedBytes;
                            }

                        private:
                            const Base*   baseType_;
                        };

                        friend class TypeSerialiser;

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
