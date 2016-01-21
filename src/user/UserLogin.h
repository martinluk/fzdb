#include <stdexcept>
#include "./ISession.h"
#include "./session.h"
#include <string>
#include <map>
#include <user/Permission.h>

struct UserAttributes {
	std::string userName;
	std::string passwordHash;
	std::string salt;
	UserGroup userGroup;
};

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


class UserFileOperations {
	protected: 
		static void addUser(UserAttributes userAttributes);
		static void removeUser(std::string userName);
		static void updateUser(std::string userName,UserAttributes newAttributes);
		static UserAttributes getUserAttributes(std::string userName);
	private:
		UserFileOperations() {};
		static void loadCacheFromFile();
		static void saveCacheToFile();
		static std::string pathToLoginFile();
		static std::map<std::string, UserAttributes> userFileCache;
};
//Initialise cache map
std::map<std::string, UserAttributes> UserFileOperations::userFileCache;


class UserCommonOperation : public UserFileOperations { 
	public : 
		static UserGroup login(std::string userName, std::string password);
};

class UserAdmin : public UserFileOperations {
	public:
		static void addUser(UserGroup currentUserGroup, std::string userName, std::string password, UserGroup userGroup);
		static void removeUser(UserGroup currentUserGroup,std::string userName);
		static void changeUserGroup(UserGroup currentUserGroup,std::string userName, UserGroup newUserGroup);
	private:
		typedef UserFileOperations super;
};
