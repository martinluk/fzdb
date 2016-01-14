#include <stdexcept>
#include <string>

class Encryption {
	public:
		//Also expecting **password** is a hash and salted client side too
		std::string validate(std::string userName, std::string password, std::string expectedHash);
	private: 
		std::string hash(std::string password, std::string salt);
};

