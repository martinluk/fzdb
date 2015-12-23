#include "./singletons.h"
#include "./model/EntityManager.h"

namespace Singletons
{
	EntityManager* entityManager_ = NULL;

	void initialise()
	{
		entityManager_ = new EntityManager();
	}

	void shutdown()
	{
		delete entityManager_;
	}

	EntityManager* entityManager()
	{
		return entityManager_;
	}
}