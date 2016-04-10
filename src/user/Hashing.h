#ifndef USER_HASHING_H
#define USER_HASHING_H

#include <stdexcept>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <boost/uuid/sha1.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Hashing
{
    std::string hashPassword(const std::string &userName, const std::string &salt, const std::string &password);
    std::string sha1hash(const std::string &password);
    std::string genSalt();
}

#endif    // USER_HASHING_H
