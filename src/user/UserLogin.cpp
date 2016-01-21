#include <user/UserLogin.h>
#include <boost/filesystem.hpp>


std::string UserFileOperations::pathToLoginFile() {
	//XXX Using current path of solution to put login file
	return boost::filesystem::current_path().string();
}
void UserFileOperations::addUser(UserAttributes userAttributes) {
	//load cache from file
	loadCacheFromFile();
	//Assert that no such user already exist, otherwise throw exception
	std::string newUserName=userAttributes.userName;
	if (this->userFileCache.count(newUserName)==0) {
		throw new UserAlreadyExistException;
	}
	//Add into cache
	userFileCache[newUserName]=userAttributes;
	//save cache from file
	saveCacheToFile();
}
void UserFileOperations::removeUser(std::string userName) {
	//load cache from file?
	loadCacheFromFile();
	if((this->userFileCache.count(userName))>0) {
		throw new UserNotExistException;
	}
	//TODO Remove the entry
	//save cache from file?
	saveCacheToFile();
}
void UserFileOperations::updateUser(std::string userName, UserAttributes newAttributes) {
	//load cache from file?
	loadCacheFromFile();
	//TODO Update user attribute at 
	//save cache from file?
	saveCacheToFile();
}
void UserFileOperations::loadCacheFromFile() {
}
void UserFileOperations::saveCacheToFile() {
}

void UserLogin::login(ISession* session, std::string userName, std::string password) {
	//Login 
	//assert is currently guest
	//Throw exception if username cant find, combo does not match.
}

