#include "./user_file_operations.h"
#include "./hashing.h"
#include "./permission.h"
#include "./user_attributes.h"

#include <boost/assign.hpp>
#include <boost/filesystem.hpp>

#include <rapidjson/filewritestream.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include <rapidjson/document.h>

#include <cstdio>
#include <map>
#include <cassert>

#define USERNAME "username"
#define HASH "passwordHash"
#define SALT "salt"
#define ID "id"
#define USERGROUPINT "userGroupInt"
#define USERCOLLECTION "users"
#define JSONFILENAME "userFile.json"

UserFileOperations::UserFileOperations()
{

}

std::string UserFileOperations::pathToLoginFile() {
    //XXX Using current path of solution to put login file
    boost::filesystem::path dir = boost::filesystem::current_path();
    dir /= JSONFILENAME;
    return dir.string();
}

void UserFileOperations::addUser(const UserAttributes &userAttributes)
{
    //Assert that no such user already exist, otherwise throw exception
    std::string newUserName = userAttributes.userName;
    if ( _userFileCache.count(newUserName) > 0 ) {
        throw UserAlreadyExistException();
    }
    
    //Add into cache
    _userFileCache[newUserName] = userAttributes;
    
    //save cache from file
    saveCacheToFile();
}

void UserFileOperations::removeUser(const std::string &userName) {
    
    if( _userFileCache.count(userName) < 1 ) {
        throw UserNotExistException();
    }
    
    //Remove the entry
    _userFileCache.erase(userName);
    
    //save cache from file
    saveCacheToFile();
}

void UserFileOperations::updateUser(const std::string &userName, const UserAttributes &newAttributes) {
    if( _userFileCache.count(userName) < 1 ) {
        throw UserNotExistException();
    }
    
    //Update user attribute at 
    _userFileCache[userName] = newAttributes;
    
    //save cache from file
    saveCacheToFile();
}

UserAttributes UserFileOperations::getUserAttributes(const std::string &userName) const {
    if( _userFileCache.count(userName) < 1 ) {
        throw UserNotExistException();
    }
    return _userFileCache.at(userName);
}

const std::map<std::string, UserAttributes>& UserFileOperations::UserFileCache() const
{
	return _userFileCache;
}

 
void UserFileOperations::loadCacheFromFile()
{
    using namespace rapidjson;
    //XXX Window system should use rb?
    // TODO: Exception checks on file opening!
    FILE* fp = fopen(pathToLoginFile().c_str(),"r");
    char readBuffer[65536];
    //Reading file using rapidjson reader
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    rapidjson::Document jsonDoc;
    jsonDoc.ParseStream(is);
    fclose(fp);

    //Clearing userfile cache, ready for reloading
    _userFileCache.clear();

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
        //Use pre-compiler for json names
        Value::ConstMemberIterator itrUser = userObject.FindMember(USERNAME);
        Value::ConstMemberIterator itrHash = userObject.FindMember(HASH);
        Value::ConstMemberIterator itrSalt = userObject.FindMember(SALT);
        Value::ConstMemberIterator itrId = userObject.FindMember(ID);
        Value::ConstMemberIterator itrGroupI = userObject.FindMember(USERGROUPINT);
        assert( itrUser != userObject.MemberEnd() &&
                itrHash != userObject.MemberEnd() &&
                itrSalt != userObject.MemberEnd() &&
                itrId != userObject.MemberEnd() &&
                itrGroupI != userObject.MemberEnd());

        //Casting user group int to user group
        Permission::UserGroup group;
        unsigned int groupNumber = itrHash->value.GetInt();
        if (groupNumber==1) 
            group = Permission::UserGroup::EDITOR;
        else if (groupNumber==0)
            group = Permission::UserGroup::ADMIN;
        else
            assert(false/*Invalid values in groupNumber in json user file*/);

        //Interogating other string values
        string username = itrUser->value.GetString();
        string hash = itrHash->value.GetString();
        string salt = itrSalt->value.GetString();
        unsigned int id = itrId->value.GetUint();
        
        //Add into a user attribute struct
        UserAttributes attr;
        attr.userName = username;
        attr.passwordHash = hash;
        attr.salt = salt;
        attr.id = id;
        
        //Add into cache
        _userFileCache[username] = attr;
    }
}

void UserFileOperations::saveCacheToFile() const
{
    using namespace rapidjson;
    
    //Using iterator to iterate through the elements in cache
    std::map<std::string, UserAttributes>::const_iterator iter = _userFileCache.cbegin();
    std::map<std::string, UserAttributes>::const_iterator eiter = _userFileCache.cend();
    
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
        userOV.AddMember(USERNAME, usernameVal, jsonDoc.GetAllocator());

        Value passwordHashVal;
        passwordHashVal.SetString(StringRef(attr.passwordHash.c_str(),attr.passwordHash.length()));
        userOV.AddMember(HASH, passwordHashVal, jsonDoc.GetAllocator());

        Value saltVal;
        saltVal.SetString(StringRef(attr.salt.c_str(),attr.salt.length()));
        userOV.AddMember(SALT, saltVal, jsonDoc.GetAllocator());

        Value idVal;
        idVal.SetUint(attr.id); //FIXME This line is potentially causing the program to crash, investigate tomorrow morning.
        userOV.AddMember(ID, idVal, jsonDoc.GetAllocator());

        //Casting usergroup to char
        using namespace std;
        using namespace boost::assign;

        map<Permission::UserGroup, unsigned int> groupIntMap; 
        insert(groupIntMap) (Permission::UserGroup::ADMIN,0) (Permission::UserGroup::EDITOR,1);

        //Logins cannot have user group guest.
        Permission::UserGroup group = attr.userGroup;
        assert(group != Permission::UserGroup::GUEST);
        
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
    // TODO: Exception check for opening/writing file!
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	jsonDoc.Accept(writer);
	std::ofstream outfile(pathToLoginFile());
	std::string json = buffer.GetString();
	outfile << json << std::endl;
	outfile.close();
}

bool UserFileOperations::contains(const std::string & name) const
{
	return _userFileCache.find(name) != _userFileCache.end();
}
