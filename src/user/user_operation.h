#ifndef USER_USEROPERATION_H
#define USER_USEROPERATION_H

#define ADD_ADMIN_ON_INIT true

#include "../util/id_generator.h"

#include "./user_file_operations.h"
#include "./user_exceptions.h"
#include "./user_attributes.h"
#include "./hashing.h"

class ISession;

class UserOperation
{ 
public: 
    UserOperation();
    
    Permission::UserGroup login(std::shared_ptr<ISession>&& session, const std::string &userName, const std::string &password);
	void init();
    void addUser(const std::string &userName, const std::string &password, Permission::UserGroup userGroup);
    void removeUser(const std::string &userName);
    void changeUserGroup(const std::string &userName, Permission::UserGroup newUserGroup);
    Permission::UserGroup getUserGroup(const std::string &userName) const;
    void changeUserPassword(const std::shared_ptr<ISession>&& session , 
            const std::string &oldpassword,
            const std::string &newpassword);
	std::string getUserName(const unsigned int id) const;

private:
    IdGenerator _idGen;
	UserFileOperations _fileOperations;
};

#endif    // USER_USEROPERATION_H
