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
