#ifndef USER_USEROPERATION_H
#define USER_USEROPERATION_H

#include "UserFileOperations.h"
#include "UserExceptions.h"
#include "UserAttributes.h"

class UserOperation : public UserFileOperations { 
	public : 
		static Permission::UserGroup login(std::string userName, std::string password);
        static void addUser(std::string userName, std::string password, Permission::UserGroup userGroup);
        static void removeUser(std::string userName);
        static void changeUserGroup(std::string userName, Permission::UserGroup newUserGroup);
		static Permission::UserGroup getUserGroup(std::string userName);
	private:
		typedef UserFileOperations super;
};

#endif	// USER_USEROPERATION_H
