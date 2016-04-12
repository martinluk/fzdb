#ifndef FUZZY_MODEL_TYPES_STRING
#define FUZZY_MODEL_TYPES_STRING

#include <string>
#include <memory>

#include "./Base.h"

namespace model {
    namespace types {

        // Stores a string value.
        class String : public Base {
        private:
            friend class TypeSerialiser;
            std::string _value;
            MemberSerialiser::DynamicStringMember _valueWrapper;
            MemberSerialiser _memberSerialiser;
            
            void initMemberSerialiser()
            {
                _memberSerialiser.addDynamicMember(&_valueWrapper);

                _memberSerialiser.setInitialised();
            }
            
        public:
            String() :Base(), _value(), _valueWrapper(_value)
            {
            }
            
            String(const std::string &value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Base(), _value(value), _valueWrapper(_value)
            {
            }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const String* s = dynamic_cast<const String*>(other);
                assert(s);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == s->_value;
            }
            
            virtual ~String() {}

            std::string value() const { return _value; }

            virtual SubType subtype() const
            {
                return SubType::String;
            }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<String>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("String(\"") + _value + std::string("\", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            virtual std::string toString() const override {
                return _value;
            }

            // Inherited via Base
            virtual unsigned char Equals(const std::string &val) const override {
                return _value == val ? 100 : 0;
            }

            virtual bool memberwiseEqual(const Base* other) const
            {
                const String* cOther = dynamic_cast<const String*>(other);
                return Base::memberwiseEqual(other) && cOther &&
                        _value == cOther->_value;
            }

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser)
            {
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

            String(const char* &serialisedData, std::size_t length) : Base(serialisedData, length), _value(), _valueWrapper(_value)
            {
                initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_STRING
