#ifndef USER_USEREXCEPTIONS_H
#define USER_USEREXCEPTIONS_H

#include <stdexcept>

class UserNotExistException : public std::runtime_error {
	public:
		   UserNotExistException(): runtime_error("user not exist") {}
};

class UserAlreadyExistException : public std::runtime_error {
	public:
		   UserAlreadyExistException(): runtime_error("user already exist") {}
};

class LoginUnsuccessfulException : public std::runtime_error { 
	public:
		LoginUnsuccessfulException(): runtime_error("Login-name/password combination not correct") {}
};

class AccessDeniedException : public std::runtime_error {
	public:
		AccessDeniedException(std::string action): runtime_error("Current user group does not clear for action '"+action+"'.") {} 
};

#endif	// USER_USEREXCEPTIONS_H
