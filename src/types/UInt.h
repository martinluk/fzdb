#ifndef FUZZY_MODEL_TYPES_INT
#define FUZZY_MODEL_TYPES_INT

#include <string>

#include "./Base.h"
#include <iostream>

namespace model {
    namespace types {

        // Stores an integer value.
        class UInt : public Base {
        private:
            friend class TypeSerialiser;
            uint32_t _value;
            MemberSerialiser _memberSerialiser;
            
            void initMemberSerialiser()
            {
                _memberSerialiser.addPrimitive(&_value, sizeof(_value));

                _memberSerialiser.setInitialised();
            }

        public:
            UInt() : Base(100, 0, std::string()), _value(0)
            {
            }
            
            UInt(uint32_t value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Base(confidence, author, comment), _value(value)
            {
            }
            
            UInt(const std::string &value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                UInt(std::atoi(value.c_str()), author, confidence, comment)
            {
                // Already initialised
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

            int32_t value() const { return _value; }

            virtual SubType subtype() const
            {
                return SubType::TypeInt32;
            }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<UInt>(_value, _originalAuthorId, _confidence);
				cloned->_orderingId = _orderingId;
				return cloned;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("Int(") + std::to_string(_value) + std::string(", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            // Inherited via Base
            virtual bool Equals(const std::string &val) const override {
                return _value == std::stoi(val);
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

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser) 
            {
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

            UInt(const char* &serialisedData, std::size_t length) : Base(serialisedData, length)
            {
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_INT
