#ifndef FUZZY_MODEL_TYPES_VALUEREF
#define FUZZY_MODEL_TYPES_VALUEREF

#include <string>

#include "./Base.h"

typedef unsigned long long EHandle_t;

namespace model {
	namespace types {
		class ValueRef : public Base {
		private:

			EHandle_t _entity;
			unsigned int _property;
			unsigned int _value;
			
		public:
			ValueRef(unsigned long long entity, unsigned int prop, unsigned int value) : Base(100, 0, std::string()), 
				_value(0), _entity(entity), _property(prop) {}
	
			virtual ~ValueRef() {}

			EHandle_t value() const { return _value; }

			virtual std::shared_ptr<Base> Clone() override {
				return std::make_shared<ValueRef>(_entity, _property, _value);
			}

			virtual Subtype subtype() const
			{
				return Subtype::ValueReference;
			}

			virtual std::string logString() const override 
			{
				return std::string("EntityRef(") + std::to_string(_value) + std::string(", ")
					+ std::to_string(confidence()) + std::string(")");
			}

			// Inherited via Base
			virtual bool Equals(const std::string &val) const override {
				return _value == std::stoll(val);
			}

			virtual std::string toString() const override {
				return std::to_string(_value);
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_VALUEREF
