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


class UserFileOperations {
	protected: 
		void addUser(UserAttributes userAttributes);
		void removeUser(std::string userName);
		void updateUser(std::string userName,UserAttributes newAttributes);
	private:
		UserFileOperations() {};
		static void loadCacheFromFile();
		static void saveCacheToFile();
		static std::string pathToLoginFile();
		static std::map<std::string, UserAttributes> userFileCache;
};
//Initialise cache map
std::map<std::string, UserAttributes> UserFileOperations::userFileCache;


class UserLogin : public UserFileOperations { 
	public : 
		static void login(ISession* session, std::string userName, std::string password);
};

class UserAdmin : public UserFileOperations {
	public:
		static void addUser(ISession* session,std::string userName, std::string password, UserGroup userGroup);
		static void removeUser(ISession* session,std::string userName);
		static void changeUserGroup(ISession* session,std::string userName, UserGroup newUserGroup);
	protected:
};
