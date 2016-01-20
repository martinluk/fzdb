#ifndef FUZZY_EXCEPTIONS
#define FUZZY_EXCEPTIONS

#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
	NotImplementedException(const char* message) : logic_error(message) {

	}
	virtual char const * what() const { return "Function not yet implemented."; }
};


#endif // !FUZZY_EXCEPTIONS
