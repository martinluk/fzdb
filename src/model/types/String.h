#ifndef FUZZY_MODEL_TYPES_STRING
#define FUZZY_MODEL_TYPES_STRING

#include <string>
#include <memory>

#include "./Base.h"

namespace model {
	namespace types {
		class String : public Base {
		private:
			friend class TypeSerialiser;
			std::string _value;
			MemberSerialiser::DynamicStringMember _valueWrapper;
			MemberSerialiser _memberSerialiser;
			
			void initMemberSerialiser()
			{
				_memberSerialiser.addDynamicMember(&_valueWrapper);
			}
			
		public:
			String() :Base(100, 0, std::string()), _value(), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}
			
			String(const std::string &value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
				Base(confidence, author, comment), _value(value), _valueWrapper(_value)
			{
				initMemberSerialiser();
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
				return SubType::TypeString;
			}

			virtual std::shared_ptr<Base> Clone() override {
				return std::make_shared<String>(_value, _confidence);
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
			virtual bool Equals(const std::string &val) const override {
				return _value == val;
			}

		protected:
			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
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
