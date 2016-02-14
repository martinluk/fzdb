#include "UserOperation.h"
#include "UserFileOperations.h"
#include "UserAttributes.h"
#include "UserExceptions.h"
#include "Hashing.h"

UserOperation::UserOperation() : UserFileOperations()
{
}

Permission::UserGroup UserOperation::getUserGroup(const std::string &userName) const
{
	if (userName.empty())
		return Permission::UserGroup::GUEST;
	UserAttributes currUserAttr = getUserAttributes(userName);
	return currUserAttr.userGroup;
}

//Throws LoginUnsuccessfulException if unsuccessfull.
Permission::UserGroup UserOperation::login(const std::string &userName, const std::string &password)
{
	UserAttributes currUserAttr;
	//See if user exist
	try {
		currUserAttr = getUserAttributes(userName);
	} catch (const UserNotExistException &){
		throw LoginUnsuccessfulException();
	}
	std::string actualHash = currUserAttr.passwordHash;
	std::string currSalt = currUserAttr.salt;
	std::string ourHash = Hashing::hashPassword(userName, currSalt , password);
	if (ourHash != actualHash) {
		throw LoginUnsuccessfulException();
	}
	return currUserAttr.userGroup;
}

void UserOperation::addUser(const std::string &userName, const std::string &password, Permission::UserGroup userGroup)
{
	assert(!userName.empty());
	UserAttributes a;
	a.userName=userName;
	a.salt=Hashing::genSalt();
	a.passwordHash=Hashing::hashPassword(userName,a.salt,password);
	a.userGroup=userGroup;
    UserFileOperations::addUser(a); //Super will throw UserAlreadyExistedException if user already exist
}

void UserOperation::changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup)
{
	assert(newUserGroup!=Permission::UserGroup::GUEST);
	UserAttributes a = getUserAttributes(userName);
	a.userGroup=newUserGroup;
    updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist
}

void UserOperation::removeUser(const std::string &userName)
{
	UserFileOperations::removeUser(userName);
}
