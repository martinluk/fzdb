#include "./create_entity.h"
#include "../singletons.h"
#include "../model/entity_manager.h"

CreateEntityJob::CreateEntityJob(std::shared_ptr<ISession> session, const std::string &type) : Job(session, PermType::UserOp), _type(type)
{
}

QueryResult CreateEntityJob::executeNonConst()
{
    std::shared_ptr<Entity> e = _database->entityManager().createEntity(_type);

    std::string id = std::to_string(e->getHandle());

  QueryResult result;
  result.setResultDataText(std::string("Entity created with handle") + id);
  return result;
}
