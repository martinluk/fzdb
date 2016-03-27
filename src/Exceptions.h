#ifndef FUZZY_EXCEPTIONS
#define FUZZY_EXCEPTIONS

#include <stdexcept>

class NotImplementedException : public std::logic_error {
public:
    NotImplementedException(const char* message) : logic_error(message) {

    }
    const char* what() const noexcept override { return std::logic_error::what(); }
};

class MismatchedTypeException : public std::logic_error {
public:
    MismatchedTypeException(const char* message) : logic_error(message) {

    }
    const char* what() const noexcept override { return std::logic_error::what(); }
};

#endif // !FUZZY_EXCEPTIONS
