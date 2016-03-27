#ifndef MODEL_DATABASE_H
#define MODEL_DATABASE_H

#include "EntityManager.h"
#include "../user/UserOperation.h"

class Database
{
public:
    Database();
    ~Database();
    
    EntityManager& entityManager();
    const EntityManager& entityManager() const;
    
    UserOperation& users();
    const UserOperation& users() const;
    
private:
    EntityManager _entityManager;
    UserOperation _users;
};

#endif // MODEL_DATABASE_H
