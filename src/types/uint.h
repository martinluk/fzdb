#ifndef FUZZY_MODEL_TYPES_UINT
#define FUZZY_MODEL_TYPES_UINT

#include <string>

#include "./ordered_type.h"
#include <iostream>

namespace model {
    namespace types {

        // Stores an integer value.
        class UInt : public OrderedType {
        protected:
            friend class TypeSerialiser;
            uint32_t _value;
            MemberSerialiser _memberSerialiser;
            
            void initMemberSerialiser()
            {
                _memberSerialiser.addPrimitive(&_value, sizeof(_value));

                _memberSerialiser.setInitialised();
            }

        public:

            UInt() : OrderedType(), _value(0)
            {
            }
            
            UInt(uint32_t value) :  OrderedType(), _value(value)
            {
            }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const UInt* i = dynamic_cast<const UInt*>(other);
                assert(i);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == i->_value;
            }
            
            virtual ~UInt() {}

            uint32_t value() const { return _value; }

            virtual SubType subtype() const
            {
                return SubType::UInt32;
            }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<UInt>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("Int(") + std::to_string(_value) + std::string(", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            // Inherited via Base
            virtual unsigned char Equals(const std::string &val) const override {
                return _value == std::stoi(val) ? 100 : 0;
            }

            virtual std::string toString() const override {
                return std::to_string(_value);
            }

            virtual bool memberwiseEqual(const Base* other) const
            {
                const UInt* cOther = dynamic_cast<const UInt*>(other);
                return Base::memberwiseEqual(other) && cOther &&
                        _value == cOther->_value;
            }

			bool greaterThan(const std::string rhs) override {
				return _value > std::stoul(rhs);
			}

			bool lessThan(const std::string rhs) override {
				return _value < std::stoul(rhs);
			}

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser) 
            {
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

            UInt(const char* &serialisedData, std::size_t length) : OrderedType(serialisedData, length)
            {
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_UINT
