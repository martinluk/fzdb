#include <stdexcept>
#include <string>
#include <user/Permission.h>

struct UserAttributes {
	std::string userName;
	std::string passwordHash;
	std::string salt;
	UserGroup userGroup;
};

class UserOperations {
	protected: 
		std::string pathToLoginFile();
		UserAttributes getUser(std::string userName);
};

class UserLogin : public UserOperations { 
	public : 
		void login(std::string userName, std::string password);
};

//XXX Do I want to check admin permission Session object, maybe in constructor?
class UserAdmin : public UserOperations {
	public:
		void addUser(std::string userName, std::string password, UserGroup userGroup);
		void removeUser(std::string userName);
		void changeUserGroup(std::string userName, UserGroup newUserGroup);
};
