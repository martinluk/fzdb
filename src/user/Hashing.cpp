#include <sstream>
#include <string>
#include "user/Hashing.h"
#include <iostream>
#include <boost/uuid/sha1.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

std::string Hashing::sha1hash(std::string password) {
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

std::string Hashing::genSalt() {
	//Referenced http://pragmaticjoe.blogspot.co.uk/2015/02/how-to-generate-sha1-hash-in-c.html
	boost::uuids::random_generator gen;
	boost::uuids::uuid salt = gen();
	std::string ssalt = boost::uuids::to_string(salt);
	ssalt.erase(std::remove(ssalt.begin(), ssalt.end(), '-'), ssalt.end());
	return ssalt;
}
