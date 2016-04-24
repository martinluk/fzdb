#include "./singletons.h"
#include "./model/database.h"
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/thread/shared_mutex.hpp>

namespace Singletons {
Database* _database = NULL;
boost::shared_mutex _databaseMutex;

std::string _dataFilePath;
std::string _userFilePath;

void initialise() {
  _database = new Database(std::move(_dataFilePath), std::move(_userFilePath));
  if ( boost::filesystem::exists(_dataFilePath)) {
    if ( !_database->entityManager().loadFromFile(_dataFilePath)) {
      std::cout << "Could not load file: " << _dataFilePath << std::endl;
	  boost::filesystem::rename(_dataFilePath, _dataFilePath + ".failed");
	  _database->entityManager().clearAll();
    } else {
      std::cout << "Loaded file " << _dataFilePath << " successfully." << std::endl;
    }
  } else {
    _database->entityManager().clearAll();
  }
  _database->init();
}

void shutdown() {
  delete _database;
}

void setDataFilePath(const std::string &path) {
  _dataFilePath = path;
}

void setUserFilePath(const std::string& userPath) {
	_userFilePath = userPath;
}

Database* database() {
  return _database;
}

const Database* cDatabase() {
  return _database;
}

boost::shared_mutex& databaseMutex() {
  return _databaseMutex;
}
}
