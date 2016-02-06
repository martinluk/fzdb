#include <user/UserOperation.h>
#include <user/UserFileOperations.h>
#include <user/UserAttributes.h>
#include <user/UserExceptions.h>
#include <user/Hashing.h>

UserGroup UserOperation::getUserGroup(std::string userName) {
	if (userName.empty())
		return UserGroup::GUEST;
	UserAttributes currUserAttr = getUserAttributes(userName);
	return currUserAttr.userGroup;
}

UserGroup UserOperation::login(std::string userName, std::string password) {
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

void UserOperation::addUser(UserGroup currentUserGroup, std::string userName, std::string password, UserGroup userGroup) {
	Permission::assertUserOpPermission(currentUserGroup);
	UserAttributes a;
	a.userName=userName;
	a.salt=Hashing::genSalt();
	a.passwordHash=Hashing::hashPassword(userName,a.salt,password);
	a.userGroup=userGroup;
	super::addUser(a);
}
void UserOperation::removeUser(UserGroup currentUserGroup, std::string userName) {
	Permission::assertUserOpPermission(currentUserGroup);
	super::removeUser(userName);
}

void UserOperation::changeUserGroup(std::string userName, UserGroup newUserGroup) {
	UserAttributes a = super::getUserAttributes(userName);
	a.userGroup=newUserGroup;
	super::updateUser(a.userName,a);
}

