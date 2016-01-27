#include <user/UserLogin.h>
#include <boost/filesystem.hpp>
#include <user/Hashing.h>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

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

void UserFileOperations::loadCacheFromFile() {
}
void UserFileOperations::saveCacheToFile() {
	//Using iterator to iterate through the elements in cache
	std::map<std::string, UserAttributes>::iterator iter = userFileCache.begin();
	std::map<std::string, UserAttributes>::iterator eiter = userFileCache.end();
	//TODO Add default user, maybe using static boolean to see if init-ed
	//Writing cache to string
	rapidjson::StringBuffer s;
	rapidjson::Writer<rapidjson::StringBuffer> writer(s);
	//Refer to example http://bit.ly/1KakUqr
	//Also serializing struct http://bit.ly/23sQ4jd
	//writer.StartObject();

	for(;iter!=eiter;++iter) {
		//writer.
	}

	std::string json = s.GetString();
}
 
