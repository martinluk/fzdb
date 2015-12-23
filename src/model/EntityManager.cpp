#include "./EntityManager.h"
#include <cassert>

EntityManager::EntityManager()
{
	lastHandle_ = Entity::INVALID_EHANDLE;
}

EntityManager::~EntityManager()
{
	for (EntityMap::iterator it = entities_.begin(); it != entities_.end(); ++it)
	{
		delete it->second;
	}
}

Entity* EntityManager::createEntity()
{
	lastHandle_++;
	assert(lastHandle_ != Entity::INVALID_EHANDLE);

	Entity* e = new Entity(lastHandle_);
	entities_.insert(std::pair<Entity::EHandle_t, Entity*>(lastHandle_, e));

	return e;
}
