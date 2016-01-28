#include "./CreateEntityJob.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

CreateEntityJob::CreateEntityJob(ISession* session) : Job(session)
{
}

QueryResult CreateEntityJob::execute()
{
	EntityManager* m = Singletons::entityManager();
	Entity* e = m->createEntity(std::string());	// TODO: ADD TYPE

	std::string id = std::to_string(e->getHandle());

  QueryResult result;
  result.setValue("type", "string");
  result.setValue(std::string("response"), "Entity created with handle " + id);
  return result;
}
