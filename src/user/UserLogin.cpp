#include <user/UserLogin.h>
#include <boost/filesystem.hpp>

//Start UserOperations
std::string UserOperations::pathToLoginFile() {
	//XXX Using current path of solution to put login file
	using namespace boost::filesystem;
	return boost::filesystem::current_path().string();
}

UserAttributes UserOperations::getUser(std::string userName) {
	//TODO
	//Do search on CSV to find the row with user name
	//If so, parse the row and return UserAttribute struct
	//Else, throw an exception
};

//End UserOperations

//UserLogin
//Handle login

void UserLogin::login(std::string userName, std::string password) {
	//Login 
	//Throw exception if username cant find, combo does not match.
}

void UserAdmin::addUser(std::string userName, std::string password, UserGroup userGroup) {

}
void UserAdmin::removeUser(std::string userName) {

}

void UserAdmin::changeUserGroup(std::string userName, UserGroup newUserGroup) {

}
