#ifndef FUZZY_MODEL_TYPES_ENTITY
#define FUZZY_MODEL_TYPES_ENTITY

#include <string>

#include "./Base.h"

typedef unsigned long long EHandle_t;

namespace model {
	namespace types {
		class EntityRef : public Base {
		private:
			const EHandle_t _value;
		public:

			EntityRef(const EHandle_t value) : _value(value), Base(100) {}
			EntityRef(unsigned char confidence, const EHandle_t value) : _value(value), Base(confidence) {}

			EHandle_t value() { return _value; }

			// Inherited via Base
			virtual bool Equals(const std::string val) override {
				return _value == std::stoll(val);
			}

			virtual Subtype subtype() const
			{
				return Subtype::TypeEntityRef;
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_ENTITY
