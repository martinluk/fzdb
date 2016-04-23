#include "./database.h"

Database::Database(const std::string&& dataFilePath) : _entityManager(this), _users(), _dataFilePath(dataFilePath)
{
	
}

Database::~Database()
{
}

void Database::init()
{
	_users.init();
}

EntityManager& Database::entityManager()
{
    return _entityManager;    
}

const EntityManager& Database::entityManager() const
{
    return _entityManager;
}

UserOperation& Database::users()
{
    return _users;
}

const UserOperation& Database::users() const
{
    return _users;
}

const std::string Database::dataFilePath() const
{
	return _dataFilePath;
}
