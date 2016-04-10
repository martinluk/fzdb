#ifndef USER_USERATTRIBUTES_H
#define USER_USERATTRIBUTES_H

#include <string>
#include "Permission.h"

struct UserAttributes {
    std::string userName;
    std::string passwordHash;
    std::string salt;
    Permission::UserGroup userGroup;
    unsigned int id;
};

#endif    // USER_USERATTRIBUTES_H
