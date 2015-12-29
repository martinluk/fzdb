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

EntityProperty Entity::getProperty(const unsigned int &key) const
{
	auto it = _propertyTable.find(key);
	return it == _propertyTable.cend() ? EntityProperty() : it->second;
}

void Entity::insertProperty(EntityProperty&& prop)
{
	// Erase the property if it exists (If not, this will do nothing).
	//propertyTable_.erase(prop.key());

	// Insert the new one.
	auto pair = std::make_pair<unsigned int, EntityProperty>(std::move(prop.key()), std::move(prop));
	_propertyTable.insert(pair);
}

int Entity::propertyCount() const
{
	return _propertyTable.size();
}

void Entity::removeProperty(const unsigned int &key)
{
	_propertyTable.erase(key);
}

void Entity::clearProperties()
{
	_propertyTable.clear();
}

Entity::EHandle_t Entity::getHandle() const
{
	return handle_;
}