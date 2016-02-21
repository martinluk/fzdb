#ifndef USER_USERFILEOPERATIONS_H
#define USER_USERFILEOPERATIONS_H

#include <map>
#include "Permission.h"
#include "UserExceptions.h"
#include "UserAttributes.h"

#include "rapidjson/document.h"

class UserFileOperations {
	public: 
		UserFileOperations();
		
	protected: 
		void addUser(const UserAttributes &userAttributes);
		void removeUser(const std::string &userName);
		void updateUser(const std::string &userName, const UserAttributes &newAttributes);
		UserAttributes getUserAttributes(const std::string &userName) const;
		void loadCacheFromFile();
		void saveCacheToFile() const;
		static std::string pathToLoginFile();

		rapidjson::Document getUserFile();
	private:
		std::map<std::string, UserAttributes> _userFileCache;
};

#endif	// USER_USERFILEOPERATIONS_H
