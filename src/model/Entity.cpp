#include "./Entity.h"

#include <stdexcept>
#include <string>

const Entity::EHandle_t Entity::INVALID_EHANDLE = 0;

Entity::Entity(unsigned int type) : handle_(Entity::INVALID_EHANDLE), _type(type), _linkStatus(Entity::LinkStatus::None)
{
	initMemberSerialiser();
}

Entity::Entity(unsigned int type, EHandle_t handle) : handle_(handle), _type(type), _linkStatus(Entity::LinkStatus::None)
{
	initMemberSerialiser();
}

void Entity::initMemberSerialiser()
{
	_memberSerialiser.addPrimitive(&handle_, sizeof(handle_));
	_memberSerialiser.addPrimitive(&_type, sizeof(_type));
}

Entity::~Entity()
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
	try
	{
		//delete _propertyTable.at(key);
		_propertyTable.erase(key);
	}
	catch (const std::out_of_range &ex)
	{

	}
}

bool Entity::hasProperty(const unsigned int& key)
{
	return _propertyTable.find(key) != _propertyTable.cend();
}

void Entity::clearProperties()
{
	_propertyTable.clear();
}

Entity::EHandle_t Entity::getHandle() const
{
	return handle_;
}

unsigned int Entity::getType() const
{
	return _type;
}

std::string Entity::logString() const
{
	return std::string("Entity(t=")
		+ std::to_string(_type)
		+ std::string(", h=")
		+ std::to_string(handle_)
		+ std::string(", [")
		+ std::to_string(_propertyTable.size())
		+ std::string("])");
}