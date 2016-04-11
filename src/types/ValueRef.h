#ifndef FUZZY_MODEL_TYPES_VALUEREF
#define FUZZY_MODEL_TYPES_VALUEREF

#include <string>

#include "./Base.h"

typedef unsigned long long EHandle_t;

namespace model {
    namespace types {

        // This class is used only by query processing.
        // Holds a reference to another value. This is
        // addressed by storing the entity handle, the
        // property handle within the entity and the
        // value handle within the property.
        class ValueRef : public Base {
        private:

            EHandle_t _entity;
            unsigned int _property;
            unsigned int _value;
            
        public:
            ValueRef(unsigned long long entity, unsigned int prop, unsigned int value) : Base(), 
                _value(value), _entity(entity), _property(prop) {}
    
            virtual ~ValueRef() {}

            EHandle_t entity() const { return _entity; }
            unsigned int prop() const { return _property; }
            unsigned int value() const { return _value; }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<ValueRef>(_entity, _property, _value);
				copyValues(cloned);
				return cloned;
            }

            virtual SubType subtype() const
            {
                return SubType::ValueReference;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("EntityRef(") + std::to_string(_value) + std::string(", ")
                    + std::to_string(confidence()) + std::string(")");
            }

            // Inherited via Base
            virtual bool Equals(const std::string &val) const override {
                return _value == std::stoll(val);
            }

            virtual bool valuesEqualOnly(const Base* other) const
            {
                const ValueRef* r = dynamic_cast<const ValueRef*>(other);
                assert(r);

                return Base::valuesEqualOnly(other) && _entity == r->_entity &&
                    _property == r->_property && _value == r->_value;
            }

            virtual std::string toString() const override {
                return std::to_string(_value);
            }


			unsigned char confidence() const override {
				return 100;
			}

            virtual bool memberwiseEqual(const Base* other) const
            {
                const ValueRef* cOther = dynamic_cast<const ValueRef*>(other);
                return Base::memberwiseEqual(other) && cOther &&
                        _entity == cOther->_entity &&
                        _property == cOther->_property &&
                        _value == cOther->_value;
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_VALUEREF
