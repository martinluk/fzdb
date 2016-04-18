#ifndef USER_USERFILEOPERATIONS_H
#define USER_USERFILEOPERATIONS_H

#include <map>
#include "./permission.h"
#include "./user_exceptions.h"
#include "./user_attributes.h"

#include <rapidjson/document.h>

class UserFileOperations {
    public: 
        UserFileOperations();
        
public:
        void addUser(const UserAttributes &userAttributes);
        void removeUser(const std::string &userName);
        void updateUser(const std::string &userName, const UserAttributes &newAttributes);
        UserAttributes getUserAttributes(const std::string &userName) const;
		
		const std::map<std::string, UserAttributes>& UserFileCache() const;

        void loadCacheFromFile();
        void saveCacheToFile() const;
		bool contains(const std::string& name) const;
        static std::string pathToLoginFile();   

private:
		std::map<std::string, UserAttributes> _userFileCache;
};

#endif    // USER_USERFILEOPERATIONS_H
