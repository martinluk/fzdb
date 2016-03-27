#include "Database.h"

Database::Database()
{
}

Database::~Database()
{
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
