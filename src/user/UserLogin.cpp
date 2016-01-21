#include <user/UserLogin.h>
#include <boost/filesystem.hpp>
#include <user/Hashing.h>
std::string UserFileOperations::pathToLoginFile() {
	//XXX Using current path of solution to put login file
	return boost::filesystem::current_path().string();
}
void UserFileOperations::addUser(UserAttributes userAttributes) {
	//load cache from file
	loadCacheFromFile();
	//Assert that no such user already exist, otherwise throw exception
	std::string newUserName=userAttributes.userName;
	if (userFileCache.count(newUserName)==0) {
		throw new UserAlreadyExistException;
	}
	//Add into cache
	userFileCache[newUserName]=userAttributes;
	//save cache from file
	saveCacheToFile();
}
void UserFileOperations::removeUser(std::string userName) {
	//load cache from file
	loadCacheFromFile();
	if((userFileCache.count(userName))>0) {
		throw new UserNotExistException;
	}
	//Remove the entry
	userFileCache.erase(userName);
	//save cache from file
	saveCacheToFile();
}
void UserFileOperations::updateUser(std::string userName, UserAttributes newAttributes) {
	//load cache from file
	loadCacheFromFile();
	if((userFileCache.count(userName))>0) {
		throw new UserNotExistException;
	}
	//Update user attribute at 
	userFileCache[userName]=newAttributes;
	//save cache from file
	saveCacheToFile();
}
UserAttributes UserFileOperations::getUserAttributes(std::string userName){
	//load cache from file
	loadCacheFromFile();
	if((userFileCache.count(userName))>0) {
		throw new UserNotExistException;
	}
	return userFileCache[userName];
}

UserGroup UserCommonOperation::login(std::string userName, std::string password) {
	UserAttributes currUserAttr;
	//See if user exist
	try {
		currUserAttr = getUserAttributes(userName);
	} catch (UserNotExistException e ){
		throw new LoginUnsuccessfulException;
	}
	std::string actualHash = currUserAttr.passwordHash;
	std::string currSalt = currUserAttr.salt;
	std::string ourHash = Hashing::hashPassword(userName, currSalt , password);
	if (ourHash != actualHash) {
		throw new LoginUnsuccessfulException;
	}
	return currUserAttr.userGroup;
}

void UserAdmin::addUser(UserGroup currentUserGroup, std::string userName, std::string password, UserGroup userGroup) {
	Permission::assertUserOpPermission(currentUserGroup);
	UserAttributes a;
	a.userName=userName;
	a.salt=Hashing::genSalt();
	a.passwordHash=Hashing::hashPassword(userName,a.salt,password);
	a.userGroup=userGroup;
	super::addUser(a);
}
void UserAdmin::removeUser(UserGroup currentUserGroup, std::string userName) {
	Permission::assertUserOpPermission(currentUserGroup);
	super::removeUser(userName);
}

void UserAdmin::changeUserGroup(UserGroup currentUserGroup,std::string userName, UserGroup newUserGroup) {
	Permission::assertUserOpPermission(currentUserGroup);
	UserAttributes a = super::getUserAttributes(userName);
	a.userGroup=newUserGroup;
	super::updateUser(a.userName,a);
}

//TODO I am going to use Boost serialization to do this, hopefully do it tomorrow
void UserFileOperations::loadCacheFromFile() {
}
//TODO I am going to use Boost serialization to do this, hopefully do it tomorrow
void UserFileOperations::saveCacheToFile() {
}

