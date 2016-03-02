#include "UserOperation.h"
#include "UserFileOperations.h"
#include "UserAttributes.h"
#include "UserExceptions.h"
#include "Hashing.h"

UserOperation::UserOperation() : UserFileOperations()
{
	_idGen = IdGenerator(0);
}

Permission::UserGroup UserOperation::getUserGroup(const std::string &userName) const
{
	if (userName.empty())
		return Permission::UserGroup::GUEST;
	UserAttributes currUserAttr = getUserAttributes(userName);
	return currUserAttr.userGroup;
}

//Throws LoginUnsuccessfulException if unsuccessfull.
Permission::UserGroup UserOperation::login(std::shared_ptr<ISession>&& session, const std::string &userName, const std::string &password)
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

	session->userId(_userFileCache[userName].id);
	session->setCurrentUserName(userName);

	return currUserAttr.userGroup;
}

void UserOperation::addUser(const std::string &userName, const std::string &password, Permission::UserGroup userGroup)
{
	UserAttributes a;
	a.userName=userName;
	a.salt=Hashing::genSalt();
	a.passwordHash=Hashing::hashPassword(userName,a.salt,password);
	a.userGroup=userGroup;
	a.id = _idGen.getId();
    UserFileOperations::addUser(a); //Super will throw UserAlreadyExistedException if user already exist
}

void UserOperation::changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup)
{
	UserAttributes a = getUserAttributes(userName);
	a.userGroup = newUserGroup;
    updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist
}

void UserOperation::removeUser(const std::string &userName)
{
	unsigned int oldUserId = _userFileCache[userName].id;
	_idGen.addDeleted(oldUserId);
	UserFileOperations::removeUser(userName);
}
