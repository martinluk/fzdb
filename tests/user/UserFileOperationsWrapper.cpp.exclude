#include "UserFileOperationsWrapper.h"
#include "rapidjson/filereadstream.h"

void UserFileOperationsWrapper::addUser(const UserAttributes &userAttributes) {
	UserFileOperations::addUser(userAttributes);
}
void UserFileOperationsWrapper::removeUser(const std::string &userName) {
	UserFileOperations::removeUser(userName);

}
void UserFileOperationsWrapper::updateUser(const std::string &userName, const UserAttributes &newAttributes) {
	UserFileOperations::updateUser(userName,newAttributes);
}

UserAttributes UserFileOperationsWrapper::getUserAttributes(const std::string &userName) {
	UserFileOperations::getUserAttributes(userName);
}

