#ifndef USER_USERFILEOPERATIONSWRAPPER_H
#define USER_USERFILEOPERATIONSWRAPPER_H

#include "user/Permission.h"
#include "user/UserExceptions.h"
#include "user/UserAttributes.h"
#include "user/UserFileOperations.h"
#include <string>
#include "rapidjson/document.h"

/*
 * A wrapper class of user file operations so we can test the protected methods inside it.
 */

class UserFileOperationsWrapper : public UserFileOperations
{
	friend class UserFileOperations;
	public:
		UserFileOperationsWrapper() : UserFileOperations() { };
		void addUser(const UserAttributes &userAttributes);
		void removeUser(const std::string &userName);
		void updateUser(const std::string &userName, const UserAttributes &newAttributes);
		UserAttributes getUserAttributes(const std::string &userName) ;
};

#endif //USER_USERFILEOPERATIONSWRAPPER_H

