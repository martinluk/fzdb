#include <user/UserLogin.h>
#include <boost/filesystem.hpp>

std::string UserFileOperations::pathToLoginFile() {
	//XXX Using current path of solution to put login file
	using namespace boost::filesystem;
	return boost::filesystem::current_path().string();
}
void UserFileOperations::addUser(UserAttributes userAttributes) {
}
void UserFileOperations::removeUser(std::string userName) {
}
void UserFileOperations::updateUser(std::string userName, UserAttributes newAttributes) {
}
void UserFileOperations::loadCacheFromFile() {
}
void UserFileOperations::saveCacheToFile() {
}

void UserLogin::login(std::string userName, std::string password) {
	//Login 
	//Throw exception if username cant find, combo does not match.
}

void UserAdmin::removeUser(std::string userName) {

}

void UserAdmin::changeUserGroup(std::string userName, UserGroup newUserGroup) {

}
