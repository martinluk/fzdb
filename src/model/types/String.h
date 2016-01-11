#ifndef FUZZY_MODEL_TYPES_STRING
#define FUZZY_MODEL_TYPES_STRING

#include <string>

#include "./Base.h"

namespace model {
	namespace types {
		class String : public Base {
		private: 
			const std::string _value;
		public:

			String(const std::string value) : _value(value), Base(100) {}
			String(unsigned char confidence, const std::string value) : _value(value), Base(confidence) {}

			std::string value() { return _value; }
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_STRING
