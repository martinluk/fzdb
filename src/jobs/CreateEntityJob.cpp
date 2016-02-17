#include "./CreateEntityJob.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

CreateEntityJob::CreateEntityJob(std::shared_ptr<ISession> session, const std::string &type) : Job(session), _type(type)
{
}

QueryResult CreateEntityJob::execute()
{
	std::shared_ptr<Entity> e = _database->entityManager().createEntity(_type);

	std::string id = std::to_string(e->getHandle());

  QueryResult result;
  result.setValue("type", "string");
  result.setValue(std::string("response"), "Entity created with handle " + id);
  return result;
}
