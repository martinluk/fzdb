#ifndef USER_USERFILEOPERATIONS_H
#define USER_USERFILEOPERATIONS_H

#include <map>
#include "Permission.h"
#include "UserExceptions.h"
#include "UserAttributes.h"

class UserFileOperations {
	public: 
		/*
		 * Either load from file, 
		 * Or (under debugging mode) add admin into cache and then save to file
		 */
		UserFileOperations();
		
	protected: 
		void addUser(const UserAttributes &userAttributes);
		void removeUser(const std::string &userName);
		void updateUser(const std::string &userName, const UserAttributes &newAttributes);
		UserAttributes getUserAttributes(const std::string &userName) const;
		
	private:
		void loadCacheFromFile();
		void saveCacheToFile() const;
		static std::string pathToLoginFile();
		
		std::map<std::string, UserAttributes> _userFileCache;
};

#endif	// USER_USERFILEOPERATIONS_H
