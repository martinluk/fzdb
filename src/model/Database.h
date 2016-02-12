#ifndef MODEL_DATABASE_H
#define MODEL_DATABASE_H

#include "EntityManager.h"

class Database
{
public:
	Database();
	~Database();
	
	EntityManager& entityManager();
	const EntityManager& entityManager() const;
	
private:
	EntityManager _entityManager;
};

#endif // MODEL_DATABASE_H
