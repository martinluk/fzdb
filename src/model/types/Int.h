#ifndef FUZZY_MODEL_TYPES_INT
#define FUZZY_MODEL_TYPES_INT

#include <string>

#include "./Base.h"

namespace model {
	namespace types {
		class Int : public Base {
		private:
			const int32_t _value;
		public:

			Int(const int32_t value) : _value(value), Base(100) {}
			Int(unsigned char confidence, const int32_t value) : _value(value), Base(confidence) {}

			int32_t value() { return _value; }
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_INT
