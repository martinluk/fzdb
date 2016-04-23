#ifndef USER_USERATTRIBUTES_H
#define USER_USERATTRIBUTES_H

#include <string>
#include "./permission.h"


/**
 * @brief A struct is used for containing information of a logged in user. 
 */
struct UserAttributes {
    /*! User name for the user */
    std::string userName;
    /*! Hash of the password for the user */
    std::string passwordHash;
    /*! Salt of the password for the user */
    std::string salt;
    /*! Usergroup for the user */
    Permission::UserGroup userGroup;
    /*! User id for the user */
    unsigned int id;
};

#endif    // USER_USERATTRIBUTES_H
