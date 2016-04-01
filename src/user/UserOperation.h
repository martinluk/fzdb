#ifndef USER_USEROPERATION_H
#define USER_USEROPERATION_H

#include "../util/IdGenerator.h"

#include <user/UserFileOperations.h>
#include <user/UserExceptions.h>
#include <user/UserAttributes.h>
#include <user/UserExceptions.h>
#include <user/Hashing.h>

class ISession;

class UserOperation : public UserFileOperations
{ 
public: 
    UserOperation();
    
    Permission::UserGroup login(std::shared_ptr<ISession>&& session, const std::string &userName, const std::string &password);
    void addUser(const std::string &userName, const std::string &password, Permission::UserGroup userGroup);
    void removeUser(const std::string &userName);
    void changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup);
    Permission::UserGroup getUserGroup(const std::string &userName) const;

private:
    IdGenerator _idGen;

};

#endif    // USER_USEROPERATION_H
