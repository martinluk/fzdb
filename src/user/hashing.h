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

/*! 
 * Contains utility function that is used for hashing and salting the user password
 */
namespace Hashing
{
    /** Given the user name, salt, and password, the function produces a password hash **/
    std::string hashPassword(const std::string &userName, const std::string &salt, const std::string &password);
    /** Produces a SHA1 hash of a sting **/
    std::string sha1hash(const std::string &password);
    /** Generates a pseudo randomly long string as salt **/
    std::string genSalt();
}

#endif    // USER_HASHING_H
