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
	PropertyTable::const_iterator it = propertyTable_.find(key);
	return it == propertyTable_.cend() ? EntityProperty() : it->second;
}

void Entity::insertProperty(const EntityProperty &prop)
{
	// Erase the property if it exists (If not, this will do nothing).
	propertyTable_.erase(prop.key());

	// Insert the new one.
	propertyTable_.insert(std::make_pair<unsigned int, EntityProperty>(prop.key(), EntityProperty(prop)));
}

int Entity::propertyCount() const
{
	return propertyTable_.size();
}

void Entity::removeProperty(const unsigned int &key)
{
	propertyTable_.erase(key);
}

void Entity::clearProperties()
{
	propertyTable_.clear();
}

Entity::EHandle_t Entity::getHandle() const
{
	return handle_;
}
