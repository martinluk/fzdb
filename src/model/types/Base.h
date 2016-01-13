#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../Serialiser.h"
#include <cassert>

namespace model {
	namespace types {
                class Base
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
                                header.size = 0;
                                header.subtype = baseType_->subtype();

                                Serialiser::SerialProperty prop(&header, sizeof(SerialHeader));
                                std::size_t baseBytes = serialiser.serialise(prop);
                                std::size_t derivedBytes = baseType_->serialiseSubclass(serialiser);

                                serialiser.reinterpretCast<SerialHeader*>(initialSize)->size = baseBytes + derivedBytes;
                                return baseBytes + derivedBytes;
                            }

                            template <typename T>
                            static T unserialise(const char* serialisedData)
                            {
                                return T(serialisedData);
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

			void confidence(unsigned char confidence) {
				_confidence = confidence;
			}

                        virtual Subtype subtype() const
                        {
                            return TypeUndefined;
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
