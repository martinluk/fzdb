#pragma once
#include <stdexcept>
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <boost/uuid/sha1.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class Hashing {
	public:
		//Also expecting **password** is a hash and salted client side too
		static std::string hashPassword(std::string userName, std::string salt, std::string password);
		static std::string sha1hash(std::string password);
		static std::string genSalt();
	private: 
};

