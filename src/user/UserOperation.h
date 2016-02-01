#pragma once

#include <user/UserFileOperations.h>
#include <user/UserExceptions.h>
#include <user/UserAttributes.h>

class UserOperation : public UserFileOperations { 
	public : 
		static UserGroup login(std::string userName, std::string password);
		static void addUser(UserGroup currentUserGroup, std::string userName, std::string password, UserGroup userGroup);
		static void removeUser(UserGroup currentUserGroup,std::string userName);
		static void changeUserGroup(UserGroup currentUserGroup,std::string userName, UserGroup newUserGroup);
	private:
		typedef UserFileOperations super;
};

