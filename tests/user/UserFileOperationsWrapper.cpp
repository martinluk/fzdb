#include "UserFileOperationsWrapper.h"
#include "rapidjson/filereadstream.h"

void UserFileOperationsWrapper::addUser(const UserAttributes &userAttributes) {
	UserFileOperations::loadCacheFromFile();
	UserFileOperations::addUser(userAttributes);
	UserFileOperations::saveCacheToFile();
}
void UserFileOperationsWrapper::removeUser(const std::string &userName) {
	UserFileOperations::loadCacheFromFile();
	UserFileOperations::removeUser(userName);
	UserFileOperations::saveCacheToFile();

}
void UserFileOperationsWrapper::updateUser(const std::string &userName, const UserAttributes &newAttributes) {
	UserFileOperations::loadCacheFromFile();
	UserFileOperations::updateUser(userName,newAttributes);
	UserFileOperations::saveCacheToFile();
}

UserAttributes UserFileOperationsWrapper::getUserAttributes(const std::string &userName) {
	UserFileOperations::loadCacheFromFile();
	UserFileOperations::getUserAttributes(userName);
	UserFileOperations::saveCacheToFile();
}

rapidjson::Document getJsonDoc() {
	rapidjson::Document jsonDoc;
	//TODO(Martin) Get address from super, read the file.
	return jsonDoc;
}
