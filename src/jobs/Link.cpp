#include "Link.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

#include "../Exceptions.h"

using namespace jobs;

Link::Link(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2) 
    : Job(session), _entity1(entity1), _entity2(entity2)
{}

QueryResult Link::executeNonConst()
{
    _database->entityManager().linkEntities(_entity1, _entity2);
    
    QueryResult result;
    result.setResultDataText(std::string("Entities ") + std::to_string(_entity1) + std::string(" and ") + std::to_string(_entity2) + std::string(" linked successfully."));
    return result;
}

Unlink::Unlink(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2)
    : Link(session, entity1, entity2)
{}

QueryResult Unlink::executeNonConst()
{
    _database->entityManager().unlinkEntities(_entity1, _entity2);
    
    QueryResult result;
    result.setResultDataText(std::string("Entities ") + std::to_string(_entity1) + std::string(" and ") + std::to_string(_entity2) + std::string(" unlinked successfully."));
    return result;
}

Merge::Merge(std::shared_ptr<ISession> session, Entity::EHandle_t entity1, Entity::EHandle_t entity2)
    : Link(session, entity1, entity2)
{}

QueryResult Merge::executeNonConst()
{
    _database->entityManager().mergeEntities(_entity1, _entity2);
    
    QueryResult result;
    result.setResultDataText(std::string("Entities ") + std::to_string(_entity1) + std::string(" and ") + std::to_string(_entity2) + std::string(" merged successfully."));
    return result;
}
