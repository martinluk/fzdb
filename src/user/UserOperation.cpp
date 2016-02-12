#include <user/UserOperation.h>
#include <user/UserFileOperations.h>
#include <user/UserAttributes.h>
#include <user/UserExceptions.h>
#include <user/Hashing.h>

UserGroup UserOperation::getUserGroup(std::string userName) {
	UserAttributes currUserAttr = getUserAttributes(userName);
	return currUserAttr.userGroup;
}

//Throws LoginUnsuccessfulException if unsuccessfull.
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

void UserOperation::addUser(std::string userName, std::string password, UserGroup userGroup) {
	UserAttributes a;
	a.userName=userName;
	a.salt=Hashing::genSalt();
	a.passwordHash=Hashing::hashPassword(userName,a.salt,password);
	a.userGroup=userGroup;
    super::addUser(a); //Super will throw UserAlreadyExistedException if user already exist
}
void UserOperation::removeUser(std::string userName) {
    super::removeUser(userName); //Super will throw UserNotExistException if user not already exist
}

void UserOperation::changeUserGroup(std::string userName, UserGroup newUserGroup) {
	UserAttributes a = super::getUserAttributes(userName);
	a.userGroup=newUserGroup;
    super::updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist
}

