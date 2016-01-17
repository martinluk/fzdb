#include <stdexcept>
#include <string>
#include <set>
#include <user/Permission.h>

struct UserAttributes {
	std::string userName;
	std::string passwordHash;
	std::string salt;
	UserGroup userGroup;
};

class UserNotExistException : public runtime_error {
	public:
		   UserPermissionException(std::string username): runtime_error("Given user name does not exist") {}
};

class UserFileOperations {
	protected: 
		static void addUser(UserAttributes userAttributes);
		static void removeUser(std::string userName);
		static void updateUser(std::string userName,UserAttributes newAttributes);
	private:
		UserFileOperations() {};
		static void loadCacheFromFile();
		static void saveCacheToFile();
		static std::string pathToLoginFile();
		static std::set<UserAttributes> userFileCache;
};

class UserLogin : public UserFileOperations { 
	public : 
		static void login(std::string userName, std::string password);
};

class UserAdmin : public UserFileOperations {
	public:
		static void addUser(std::string userName, std::string password, UserGroup userGroup);
		static void removeUser(std::string userName);
		static void changeUserGroup(std::string userName, UserGroup newUserGroup);
	protected:
};
