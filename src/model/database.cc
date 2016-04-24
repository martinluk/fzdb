#include "./database.h"

Database::Database(const std::string&& dataFilePath, const std::string&& userFilePath)
	: _dataFilePath(dataFilePath), _userFilePath(userFilePath), _entityManager(this), _users(this) {

}

Database::~Database() {
}

void Database::init() {
  _users.init();
}

EntityManager& Database::entityManager() {
  return _entityManager;
}

const EntityManager& Database::entityManager() const {
  return _entityManager;
}

UserOperation& Database::users() {
  return _users;
}

const UserOperation& Database::users() const {
  return _users;
}

const std::string Database::dataFilePath() const {
  return _dataFilePath;
}

const std::string Database::userFilePath() const {
  return _userFilePath;
}
