#ifndef USER_USERFILEOPERATIONS_H
#define USER_USERFILEOPERATIONS_H

#include <map>
#include "./permission.h"
#include "./user_exceptions.h"
#include "./user_attributes.h"

#include <rapidjson/document.h>

/** Manager between user file and the user information. **/

class UserFileOperations {
    public: 
        /** Constructor of the class. **/
        UserFileOperations();
        
public:
        /** Add user to the user file. **/
        void addUser(const UserAttributes &userAttributes);
        /** Remove user to the user file. **/
        void removeUser(const std::string &userName);
        /** Update user to the user file. **/
        void updateUser(const std::string &userName, const UserAttributes &newAttributes);
        /** Get a user from the user file. **/
        UserAttributes getUserAttributes(const std::string &userName) const;
		
        /** Get the collection of users. **/
		const std::map<std::string, UserAttributes>& UserFileCache() const;

        /** Force the class to renew the cache. **/
        void loadCacheFromFile();

        /** Save the cache to the user file. **/
        void saveCacheToFile() const;

        /** Check whether the given user exist. **/
		bool contains(const std::string& name) const;

        /** Provides the path to the user file. **/
        static std::string pathToLoginFile();   

private:
        /** The current cache of user file. **/
		std::map<std::string, UserAttributes> _userFileCache;
};

#endif    // USER_USERFILEOPERATIONS_H
