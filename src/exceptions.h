#ifndef FUZZY_EXCEPTIONS
#define FUZZY_EXCEPTIONS

#include <stdexcept>

// Specifies that this particular functionality is not yet implemented.
class NotImplementedException : public std::logic_error {
 public:
  NotImplementedException(const char* message) : logic_error(message) {

  }
  const char* what() const noexcept override {
    return std::logic_error::what();
  }
};

// Specifies that the received type is incorrect for the given function.
class MismatchedTypeException : public std::logic_error {
 public:
  MismatchedTypeException(const char* message) : logic_error(message) {

  }
  const char* what() const noexcept override {
    return std::logic_error::what();
  }
};

#endif // !FUZZY_EXCEPTIONS
