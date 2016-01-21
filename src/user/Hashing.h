#include <stdexcept>
#include <string>

class Hashing {
	public:
		//Also expecting **password** is a hash and salted client side too
		static std::string hashPassword(std::string userName, std::string salt, std::string password);
		static std::string sha1hash(std::string password);
		static std::string genSalt();
	private: 
};

