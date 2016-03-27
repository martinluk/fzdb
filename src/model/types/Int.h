#ifndef FUZZY_MODEL_TYPES_INT
#define FUZZY_MODEL_TYPES_INT

#include <string>

#include "./Base.h"

namespace model {
    namespace types {
        class Int : public Base {
        private:
            friend class TypeSerialiser;
            int32_t _value;
            MemberSerialiser _memberSerialiser;
            
            void initMemberSerialiser()
            {
                _memberSerialiser.addPrimitive(&_value, sizeof(_value));
            }

        public:
            Int() : Base(100, 0, std::string()), _value(0)
            {
                initMemberSerialiser();
            }
            
            Int(int32_t value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Base(confidence, author, comment), _value(value)
            {
                initMemberSerialiser();
            }
            
            Int(std::string value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Int(std::atoi(value.c_str()), author, confidence, comment)
            {
                // Already initialised
            }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const Int* i = dynamic_cast<const Int*>(other);
                assert(i);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == i->_value;
            }
            
            virtual ~Int() {}

            int32_t value() const { return _value; }

            virtual SubType subtype() const
            {
                return SubType::TypeInt32;
            }

            virtual std::shared_ptr<Base> Clone() override {
                return std::make_shared<Int>(_value, 0, _confidence);
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

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
            {
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialisePrimitives(serialiser);
            }

            Int(const char* &serialisedData, std::size_t length) : Base(serialisedData, length)
            {
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialisePrimitives(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_INT
