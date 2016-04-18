#ifndef FUZZY_MODEL_TYPES_BASE
#define FUZZY_MODEL_TYPES_BASE

#include "../model/Serialiser.h"
#include <cassert>
#include <cstring>
#include <memory>
#include "../model/ILogString.h"
#include "../model/Triple.h"
#include "../model/MemberSerialiser.h"
#include "../model/PropertyOwner.h"
#include "./SubType.h"

#include <boost/date_time/posix_time/posix_time.hpp>

class EntityManager;

namespace model {
    namespace types {

        // Base value type class. All other types inherit from this.
        class Base : public ILogString, public PropertyOwner
        {
		private:
			bool _initialised;
            MemberSerialiser _memberSerialiser;

			virtual void initMemberSerialiser();

		protected:
			void copyValues(const std::shared_ptr<model::types::Base> ptr);

            friend class TypeSerialiser;			

            // id for this record - unique for entity/property/id - related to ordering
            unsigned int _orderingId;

        public:

			Base();

			void Init(const unsigned char confidence);

			virtual void setupDefaultMetaData(const unsigned char confidence);
            
			virtual ~Base();

			virtual std::shared_ptr<Base> Clone();

			virtual unsigned char Equals(const std::string &val) const;
            
            // This specifically should NOT compare the confidence, ordering, source, author, time of creation or comment.
			virtual bool valuesEqualOnly(const Base *other) const;

            // Returns whether this value is equal to the given object.
			unsigned char Equals(const model::Object &object);

            // What's the string representation of this value?
			virtual std::string toString() const;

			virtual unsigned char confidence() const;

            // Subclasses reimplement this.
            // As a base class, our type is undefined.
			virtual SubType subtype() const;

			virtual std::string logString(const Database* db = NULL) const;

			void OrderingId(unsigned int id);

			unsigned int OrderingId();

            // For debugging - make sure we are -exactly- the same as the other type.
			virtual bool memberwiseEqual(const Base* other) const;

        protected:
            // Called when serialising.
			virtual std::size_t serialiseSubclass(Serialiser &serialiser);

            // Called to construct from serialised data.
			Base(const char* &serialisedData, std::size_t length);
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
