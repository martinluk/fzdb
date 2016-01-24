#ifndef SINGLETONS_H
#define SINGLETONS_H

#include <string>

class EntityManager;

namespace Singletons
{
	void initialise();
	void shutdown();

        void setDataFilePath(const std::string &path);
	EntityManager* entityManager();
}

#endif	// SINGLETONS_H
