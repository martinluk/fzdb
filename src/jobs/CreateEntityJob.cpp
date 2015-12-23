#include "./CreateEntityJob.h"
#include "../singletons.h"
#include "../model/EntityManager.h"

CreateEntityJob::CreateEntityJob(ISession* session) : Job(session)
{
}

void CreateEntityJob::execute()
{
	EntityManager* m = Singletons::entityManager();
	Entity* e = m->createEntity();

	std::string id = std::to_string(e->getHandle());
	_session->respond("Entity created with handle " + id);
}
