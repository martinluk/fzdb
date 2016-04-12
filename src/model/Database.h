#ifndef MODEL_DATABASE_H
#define MODEL_DATABASE_H

#include "EntityManager.h"
#include "../user/UserOperation.h"

// Convenience class that encapsulates the main data within the database application.
class Database
{
public:
    Database();
    ~Database();

	void init();
    
    // Entities
    EntityManager& entityManager();
    const EntityManager& entityManager() const;
    
    // Users
    UserOperation& users();
    const UserOperation& users() const;
    
private:
    EntityManager _entityManager;
    UserOperation _users;
};

#endif // MODEL_DATABASE_H
