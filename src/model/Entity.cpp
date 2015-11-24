#include "./Entity.h"

const Entity::EHandle_t Entity::INVALID_EHANDLE = 0;

Entity::Entity() : handle_(Entity::INVALID_EHANDLE)
{
}

Entity::Entity(EHandle_t handle) : handle_(handle)
{
}

bool Entity::isNull() const
{
	return handle_ == INVALID_EHANDLE;
}

EntityProperty Entity::getProperty(const std::string &key) const
{
	PropertyTable::const_iterator it = propertyTable_.find(key);
	return it == propertyTable_.cend() ? EntityProperty() : it->second;
}

void Entity::insertProperty(const EntityProperty &prop)
{
	// Erase the property if it exists (If not, this will do nothing).
	propertyTable_.erase(prop.key());

	// Insert the new one.
	propertyTable_.insert(std::make_pair<std::string, EntityProperty>(prop.key(), EntityProperty(prop)));
}

int Entity::propertyCount() const
{
	return propertyTable_.size();
}

void Entity::removeProperty(const std::string &key)
{
	propertyTable_.erase(key);
}

void Entity::clearProperties()
{
	propertyTable_.clear();
}