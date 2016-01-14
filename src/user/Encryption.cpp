#include <sstream>
#include <string>
#include "./Encryption.h"
#include <iostream>
#include <boost/uuid/sha1.hpp>

//For reference: http://pragmaticjoe.blogspot.co.uk/2015/02/how-to-generate-sha1-hash-in-c.html
std::string Encryption::hash(std::string password, std::string salt) {
	boost::uuids::detail::sha1 sha1;
	std::string strToHash = password.append(salt);
	sha1.process_bytes(strToHash.c_str(), strToHash.size());
	unsigned int digest[5];
	sha1.get_digest(digest);
	//Digest is now a 20 bytes, transforming to hex
	std::stringstream hexHash;
    for(int i=0; i<sizeof(digest)/sizeof(int); ++i) {
		hexHash << std::hex <<digest[i];
    }
	return hexHash.str();
}
