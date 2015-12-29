#include "./Entity.h"

const Entity::EHandle_t Entity::INVALID_EHANDLE = 0;

Entity::Entity(unsigned int type) : handle_(Entity::INVALID_EHANDLE), _type(type)
{
}

Entity::Entity(unsigned int type, EHandle_t handle) : handle_(handle), _type(type)
{
}

bool Entity::isNull() const
{
	return handle_ == INVALID_EHANDLE;
}

int Entity::propertyCount() const
{
	return _propertyTable.size();
}

void Entity::removeProperty(const unsigned int &key)
{
	_propertyTable.erase(key);
}

bool Entity::hasProperty(const unsigned int& key)
{
	if (_propertyTable.find(key) == _propertyTable.cend()) {
		return false;
	}
	return true;
}

void Entity::clearProperties()
{
	_propertyTable.clear();
}

Entity::EHandle_t Entity::getHandle() const
{
	return handle_;
}