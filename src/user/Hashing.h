#ifndef USER_HASHING_H
#define USER_HASHING_H

#include <stdexcept>
#include <string>

namespace Hashing
{
	std::string hashPassword(const std::string &userName, const std::string &salt, const std::string &password);
	std::string sha1hash(const std::string &password);
	std::string genSalt();
}

#endif	// USER_HASHING_H
