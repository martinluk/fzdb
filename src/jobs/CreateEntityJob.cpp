#include "./CreateEntityJob.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

CreateEntityJob::CreateEntityJob(ISession* session) : Job(session)
{
}

QueryResult CreateEntityJob::execute()
{
	EntityManager* m = Singletons::entityManager();
	std::shared_ptr<Entity> e = m->createEntity();

	std::string id = std::to_string(e->getHandle());

  QueryResult result;
  result.setValue("type", "string");
  result.setValue(std::string("response"), "Entity created with handle " + id);
  return result;
}
