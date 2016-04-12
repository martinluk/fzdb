#ifndef FUZZY_MODEL_TYPES_ENTITY
#define FUZZY_MODEL_TYPES_ENTITY

#include <string>

#include "./Base.h"

typedef unsigned long long EHandle_t;

namespace model {
    namespace types {

        // Class representing a reference to another entity.
        // This reference is stored as a handle to the entity concerned.
        // There is no guarantee that this handle will still be valid
        // when it is used - the caller needs to check!
        class EntityRef : public Base {
        protected:
            friend class TypeSerialiser;
            EHandle_t _value;
            MemberSerialiser _memberSerialiser;
            
			void initMemberSerialiser();
            
        public:
			EntityRef();

			EntityRef(EHandle_t value);
            
            EntityRef(const std::string &value) :
                EntityRef(std::atoll(value.c_str()))
            {
                // Already initialised
            }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const EntityRef* r = dynamic_cast<const EntityRef*>(other);
                assert(r);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == r->_value;
            }
            
            virtual ~EntityRef() {}

            EHandle_t value() const { return _value; }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<EntityRef>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
            }

            virtual SubType subtype() const
            {
                return SubType::EntityRef;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("EntityRef(") + std::to_string(_value) + std::string(", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            // Inherited via Base
            virtual unsigned char Equals(const std::string &val) const override {
                return _value == std::stoll(val) ? 100 : 0;
            }

            virtual std::string toString() const override {
                return std::to_string(_value);
            }

            virtual bool memberwiseEqual(const Base* other) const
            {
                const EntityRef* cOther = dynamic_cast<const EntityRef*>(other);
                return Base::memberwiseEqual(other) && cOther &&
                        _value == cOther->_value;
            }

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser)
            {
                //return Base::serialiseSubclass(serialiser)
                //    + serialiser.serialise(Serialiser::SerialProperty(&_value, sizeof(EHandle_t)));
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

            EntityRef(const char* &serialisedData, std::size_t length) : Base(serialisedData, length)
            {
                //_value = *(reinterpret_cast<const EHandle_t*>(serialisedData));
                //serialisedData += sizeof(_value);
                
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_ENTITY
