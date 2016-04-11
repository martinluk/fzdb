#include "UserOperation.h"
#include "UserFileOperations.h"
#include "UserAttributes.h"
#include "UserExceptions.h"
#include "Hashing.h"
#include "spdlog/spdlog.h"

#include "../ISession.h"

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
    boost::uuids::uuid _uuid= session->uuid();
    if (ourHash != actualHash) {
        spdlog::get("main")->warn("[{:<}] {} {}", _uuid, userName , " logged in unsuccessfully.");
        throw LoginUnsuccessfulException();
    }
    session->userId(_userFileCache[userName].id);
    session->setCurrentUserName(userName);
    session->setUserGroup(currUserAttr.userGroup);

    spdlog::get("main")->warn("[{:<}] {} {}", _uuid, userName , " logged in successfully.");
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
    a.id = _idGen.getId();
    UserFileOperations::addUser(a); //Super will throw UserAlreadyExistedException if user already exist
}

void UserOperation::changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup)
{
    assert(newUserGroup!=Permission::UserGroup::GUEST);
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

void UserOperation::changeUserPassword(const std::shared_ptr<ISession>&& session,
        const std::string &oldpassword, const std::string &newpassword) {

	if (session->getUserGroup() == Permission::UserGroup::GUEST) {
		throw LoginUnsuccessfulException();
	}

    std::string userName = session->username();
    UserAttributes a = getUserAttributes(userName);
    //Check old password
    std::string actualHash = a.passwordHash;
    std::string currSalt = a.salt;
    std::string ourHash =
        Hashing::hashPassword(userName, currSalt , oldpassword);
    if (ourHash != actualHash) {
        throw LoginUnsuccessfulException();
    }
    //Get new salt
    a.salt=Hashing::genSalt();
    //gen new hash
    a.passwordHash=Hashing::hashPassword(userName,a.salt,newpassword);
    //hash new password
    updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist
}
