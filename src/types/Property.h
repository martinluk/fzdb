#ifndef FUZZY_MODEL_TYPES_PROPERTY
#define FUZZY_MODEL_TYPES_PROPERTY

#include <string>
#include <memory>

#include "./Base.h"

namespace model {
    namespace types {

        class Property : public Base {
        private:
            friend class TypeSerialiser;
            unsigned int _value;
            MemberSerialiser _memberSerialiser;
            
            void initMemberSerialiser()
            {
				_memberSerialiser.addPrimitive(&_value, sizeof(_value));

                _memberSerialiser.setInitialised();
            }
            
        public:
			Property() : Base(), _value()
            {
                _manager = NULL;
            }
            
            Property(const unsigned int value, const EntityManager* manager, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                Base(), _value(value)
            {
                _manager = manager;
            }
            
            virtual bool valuesEqualOnly(const Base *other) const
            {
                const Property* s = dynamic_cast<const Property*>(other);
                assert(s);
                
                // If the subtypes are not the same then the base implementation
                // will return false and the statement will short-circuit, meaning
                // we should avoid dereferencing the pointer if it's null!
                return Base::valuesEqualOnly(other)
                        && _value == s->_value;
            }
            
            virtual ~Property() {}

			unsigned int value() const { return _value; }

            virtual SubType subtype() const
            {
                return SubType::PropertyReference;
            }

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<Property>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
            }

            virtual std::string logString(const Database* db = NULL) const override
            {
                return std::string("String(\"") + std::to_string(_value) + std::string("\", ")
                    + std::to_string(confidence()) + std::string(")");
            }

			std::string toString() const override;

            // Inherited via Base
            virtual unsigned char Equals(const std::string &val) const override {
                return _value == std::stoul(val) ? 100 : 0;
            }

        protected:
            virtual std::size_t serialiseSubclass(Serialiser &serialiser)
            {
				if (!_memberSerialiser.initialised())initMemberSerialiser();
                return Base::serialiseSubclass(serialiser) + _memberSerialiser.serialiseAll(serialiser);
            }

            Property(const char* &serialisedData, std::size_t length) : Base(serialisedData, length), _value()
            {
               initMemberSerialiser();
                serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_PROPERTY
