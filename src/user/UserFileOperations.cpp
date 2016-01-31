#include <user/UserFileOperations.h>
#include <user/Hashing.h>

#include "boost/assign.hpp"
#include <boost/filesystem.hpp>

#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"

#include <cstdio>
#include <map>
#include <cassert>

#define ADD_ADMIN_ON_INIT true

#define JSONFILENAME "userFile.json"

#define USERNAME "username"
#define HASH "passwordHash"
#define SALT "salt"
#define USERGROUPINT "userGroupInt"
#define USERCOLLECTION "users"

//Initialise cache map
std::map<std::string, UserAttributes> UserFileOperations::userFileCache;

void UserFileOperations::initialize() { //TODO
	//Empty file cache
	if (ADD_ADMIN_ON_INIT) {
		//Add admin into cache
		UserAttributes admin;
		admin.userName = "admin";
		admin.salt = "TODO" ;//TODO
		admin.passwordHash = "TODO" ; //TODO 
		admin.userGroup = UserGroup::ADMIN;
		addUser(admin);
		//Save to json
		saveCacheToFile();
	} else { 
		//Load from json
		loadCacheFromFile();
	}
}

std::string UserFileOperations::pathToLoginFile() {
	//XXX Using current path of solution to put login file
	boost::filesystem::path dir = boost::filesystem::current_path();
	dir /= JSONFILENAME;
	return dir.string();
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

 
void UserFileOperations::loadCacheFromFile() {
	using namespace rapidjson;
	//XXX Window system should use rb?
	FILE* fp = fopen(pathToLoginFile().c_str(),"r");
	char readBuffer[65536];
	//Reading file using rapidjson reader
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	rapidjson::Document jsonDoc;
	jsonDoc.ParseStream(is);
	fclose(fp);

	//Clearing userfile cache, ready for reloading
	userFileCache.clear();

	//Assert json is valid
	assert(jsonDoc.IsObject());

	//Assert has user array
	assert(jsonDoc.HasMember(USERCOLLECTION));
	const Value& userArray=jsonDoc[USERCOLLECTION];
	assert(userArray.IsArray());

	for (SizeType i=0; i<userArray.Size(); i++) {
		const Value& userObject=userArray[i];
		assert(userObject.IsObject());
		using namespace std;
		//'FindMember' checks existence of member and obtain member at once
		//TODO Use pre-compiler for json names
		Value::ConstMemberIterator itrUser = userObject.FindMember(USERNAME);
		Value::ConstMemberIterator itrHash = userObject.FindMember(HASH);
		Value::ConstMemberIterator itrSalt = userObject.FindMember(SALT);
		Value::ConstMemberIterator itrGroupI = userObject.FindMember(USERGROUPINT);
		assert( itrUser != userObject.MemberEnd() &&
				itrHash != userObject.MemberEnd() &&
				itrSalt != userObject.MemberEnd() &&
				itrGroupI != userObject.MemberEnd());

		//Casting user group int to user group
		UserGroup group;
		unsigned int groupNumber = itrHash->value.GetInt();
		if (groupNumber==1) 
			group=UserGroup::EDITOR;
		else if (groupNumber==0)
			group=UserGroup::ADMIN;
		else
			assert(false/*Invalid values in groupNumber in json user file*/);

		//Interogating other string values
		string username = itrUser->value.GetString();
		string hash = itrHash->value.GetString();
		string salt = itrSalt->value.GetString();
		//Add into a user attribute struct
		UserAttributes attr;
		attr.userName = username;
		attr.passwordHash = hash;
		attr.salt = salt;
		//Add into cache
		userFileCache[username]=attr;
	}
}

void UserFileOperations::saveCacheToFile() {
	//Using iterator to iterate through the elements in cache
	std::map<std::string, UserAttributes>::iterator iter = userFileCache.begin();
	std::map<std::string, UserAttributes>::iterator eiter = userFileCache.end();
	using namespace rapidjson;
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
		userOV.AddMember(USERNAME,usernameVal,jsonDoc.GetAllocator());

		Value passwordHashVal;
		passwordHashVal.SetString(StringRef(attr.passwordHash.c_str(),attr.passwordHash.length()));
		userOV.AddMember(HASH,passwordHashVal,jsonDoc.GetAllocator());

		Value saltVal;
		saltVal.SetString(StringRef(attr.salt.c_str(),attr.salt.length()));
		userOV.AddMember(SALT,saltVal,jsonDoc.GetAllocator());

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
		userOV.AddMember(USERGROUPINT,userGroupInt,allocator);

		//Add the user object userCollections array
		userCollections.PushBack(userOV, allocator);
	}

	//Add the userCollections array into main Json
	
	jsonDoc.AddMember(USERCOLLECTION,userCollections,allocator);

	//Using rapidJson FileWriteStream to write to user file.
	char writeBuffer[65536];
	//XXX Window system should use wb?
	FILE* fp = fopen(pathToLoginFile().c_str(),"w");
	FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	Writer<FileWriteStream> writer(os);
	jsonDoc.Accept(writer);
	fclose(fp);
}
