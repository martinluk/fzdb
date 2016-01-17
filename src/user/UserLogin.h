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

class UserFileOperations {
	protected: 
		static std::string pathToLoginFile();
	private:
		UserFileOperations();
		static std::set<UserAttributes> userFileCache;
		static void loadCacheFromFile();
		static void saveCacheToFile();
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
};
