#pragma once

#include <stdexcept>
class UserNotExistException : public runtime_error {
	public:
		   UserNotExistException(): runtime_error("user not exist") {}
};

class UserAlreadyExistException : public runtime_error {
	public:
		   UserAlreadyExistException(): runtime_error("user already exist") {}
};

class LoginUnsuccessfulException : public runtime_error { 
	public:
		LoginUnsuccessfulException(): runtime_error("Login-name/password combination not correct") {}
};

class AccessDeniedException : public runtime_error {
	public:
		AccessDeniedException(std::string action): runtime_error("Current user group does not clear for action '"+action+"'.") {} 
};

