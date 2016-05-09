#include "./user_operation.h"
#include "./user_file_operations.h"
#include "./user_attributes.h"
#include "./user_exceptions.h"
#include "./hashing.h"
#include "../model/database.h"
#include <spdlog/spdlog.h>

#include "../isession.h"

UserOperation::UserOperation(Database* database)
	: _database(database), _fileOperations(database) {
  _idGen = IdGenerator();
}

void UserOperation::init() {
  if (ADD_ADMIN_ON_INIT) {
    //Add admin into cache
    addUser(ADMIN_USERNAME, ADMIN_PASSWORD, Permission::UserGroup::ADMIN);
  } else {
    //Load from json
    _fileOperations.loadCacheFromFile();
  }
}

Permission::UserGroup UserOperation::getUserGroup(const std::string &userName) const {
  if (userName.empty())
    return Permission::UserGroup::GUEST;
  UserAttributes currUserAttr = _fileOperations.getUserAttributes(userName);
  return currUserAttr.userGroup;
}

//Throws LoginUnsuccessfulException if unsuccessfull.
Permission::UserGroup UserOperation::login(std::shared_ptr<ISession>&& session, const std::string &userName, const std::string &password) {
  UserAttributes currUserAttr;
  //See if user exist
  try {
    currUserAttr = _fileOperations.getUserAttributes(userName);
  } catch (const UserNotExistException &) {
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
  session->userId(_fileOperations.UserFileCache().at(userName).id);
  session->setCurrentUserName(userName);
  session->setUserGroup(currUserAttr.userGroup);

  spdlog::get("main")->warn("[{:<}] {} {}", _uuid, userName , " logged in successfully.");
  return currUserAttr.userGroup;
}

void UserOperation::logout(const std::shared_ptr<ISession>&& session) const {
  session->clearCurrentUserName();
  session->_userId = 0;
  session->setUserGroup(Permission::UserGroup::GUEST);
}

void UserOperation::addUser(const std::string &userName, const std::string &password, Permission::UserGroup userGroup) {
  assert(!userName.empty());
  UserAttributes a;
  a.userName=userName;
  a.salt=Hashing::genSalt();
  a.passwordHash=Hashing::hashPassword(userName,a.salt,password);
  a.userGroup=userGroup;
  a.id = _idGen.getId();
  _fileOperations.addUser(a); //
}

void UserOperation::changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup) {
  assert(newUserGroup!=Permission::UserGroup::GUEST);
  UserAttributes a = _fileOperations.getUserAttributes(userName);
  a.userGroup = newUserGroup;
  _fileOperations.updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist
}

void UserOperation::removeUser(const std::string &userName) {
  if (!_fileOperations.contains(userName)) {
      throw UserNotExistException();
  }
  unsigned int oldUserId = _fileOperations.UserFileCache().at(userName).id;
  _idGen.addDeleted(oldUserId);
  _fileOperations.removeUser(userName);
}

void UserOperation::changeUserPassword(const std::string &username,  const std::string &newpassword) {
  //Get user attributes
  UserAttributes a = _fileOperations.getUserAttributes(username);
  //Get new salt
  a.salt=Hashing::genSalt();
  //gen new hash
  a.passwordHash=Hashing::hashPassword(username,a.salt,newpassword);
  //hash new password
  _fileOperations.updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist

}

void UserOperation::changeUserPassword(const std::shared_ptr<ISession>&& session,
                                       const std::string &oldpassword, const std::string &newpassword) {

  if (session->getUserGroup() == Permission::UserGroup::GUEST) {
    throw LoginUnsuccessfulException();
  }

  std::string userName = session->username();
  UserAttributes a = _fileOperations.getUserAttributes(userName);
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
  _fileOperations.updateUser(a.userName,a); //Super will throw UserNotExistException if user not already exist
}


std::string UserOperation::getUserName(const unsigned int id) const {
  for (auto iter = _fileOperations.UserFileCache().begin(); iter != _fileOperations.UserFileCache().end(); iter++) {
    if (iter->second.id == id)return iter->first;
  }
  return "User not found - account deleted";
}
