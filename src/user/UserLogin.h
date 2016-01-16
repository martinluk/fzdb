#include <stdexcept>
#include <string>
#include <user/Permission.h>

struct UserAttributes {
	std::string userName;
	std::string passwordHash;
	std::string salt;
	UserGroup userGroup;
};

//Make this a singleton?, have std::vector<UserAttribute>
class UserOperations {
	protected: 
		static std::string pathToLoginFile();
		static UserAttributes getUser(std::string userName);
};

class UserLogin : public UserOperations { 
	public : 
		static void login(std::string userName, std::string password);
};

//XXX Do I want to check admin permission Session object, maybe in constructor?
class UserAdmin : public UserOperations {
	public:
		static void addUser(std::string userName, std::string password, UserGroup userGroup);
		static void removeUser(std::string userName);
		static void changeUserGroup(std::string userName, UserGroup newUserGroup);
};
