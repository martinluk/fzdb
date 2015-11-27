#ifndef SINGLETONS_H
#define SINGLETONS_H

class EntityManager;

namespace Singletons
{
	void initialise();
	void shutdown();

	EntityManager* entityManager();
}

#endif	// SINGLETONS_H