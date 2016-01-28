#include <map>
#include <cassert>
#include "boost/assign.hpp"
#include <user/UserLogin.h>
#include <boost/filesystem.hpp>
#include <user/Hashing.h>
#include "rapidjson/filewritestream.h"
#include <cstdio>

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
	using namespace rapidjson;
	//TODO Add default user, maybe using static boolean to see if init-ed
	//Writing cache to string
	Document jsonDoc;
	Document::AllocatorType& allocator = jsonDoc.GetAllocator();

	//Refer to example http://bit.ly/1KakUqr
	//Also serializing struct http://bit.ly/23sQ4jd

	jsonDoc.SetObject();

	Value userCollections(kArrayType);
	for(;iter!=eiter;++iter) {
		//Create User Json Object Value
		Value userOV;
		userOV.SetObject();

		//Getting user from cache
		UserAttributes attr = iter->second;

		//Adding attributes has time of string
		Value usernameVal;
		usernameVal.SetString(StringRef(attr.userName.c_str(),attr.userName.length()));
		userOV.AddMember("username",usernameVal,jsonDoc.GetAllocator());

		Value passwordHashVal;
		passwordHashVal.SetString(StringRef(attr.passwordHash.c_str(),attr.passwordHash.length()));
		userOV.AddMember("passwordHash",passwordHashVal,jsonDoc.GetAllocator());

		Value saltVal;
		saltVal.SetString(StringRef(attr.salt.c_str(),attr.salt.length()));
		userOV.AddMember("salt",saltVal,jsonDoc.GetAllocator());

		//Casting usergroup to char
		using namespace std;
		using namespace boost::assign;

		map<UserGroup, unsigned int> groupIntMap; 
		insert(groupIntMap) (UserGroup::ADMIN,0) (UserGroup::EDITOR,1) ;

		//Logins cannot have user group guest.
		UserGroup group = attr.userGroup;
		assert(group != UserGroup::GUEST);
		
		Value userGroupInt;
		userGroupInt.SetInt(groupIntMap[group]);
		userOV.AddMember("userGroupInt",userGroupInt,allocator);

		//Add the user object userCollections array
		userCollections.PushBack(userOV, allocator);
	}

	//Add the userCollections array into main Json
	
	jsonDoc.AddMember("users",userCollections,allocator);

	//Using rapidJson FileWriteStream to write to user file.
	//
	char writeBuffer[65536];
	FILE* fp = fopen(pathToLoginFile().c_str(),"w");
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);
	jsonDoc.Accept(writer);
	fclose(fp);
}
 
