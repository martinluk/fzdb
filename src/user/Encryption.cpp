#include <sstream>
#include <string>
#include "./Encryption.h"
#include <iostream>
#include <boost/uuid/sha1.hpp>

std::string Encryption::sha1hash(std::string password) {
	boost::uuids::detail::sha1 sha1;
	sha1.process_bytes(password.c_str(), password.size());
	unsigned int digest[5];
	sha1.get_digest(digest);
	//Digest is now a 20 bytes, transforming to hex
	std::stringstream hexHash;
    for(int i=0; i<sizeof(digest)/sizeof(int); ++i) {
		hexHash << std::hex <<digest[i];
    }
	return hexHash.str();
}
