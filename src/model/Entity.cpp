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
    _memberSerialiser.addPrimitive(&_locked, sizeof(_locked));
    _memberSerialiser.addPrimitive(&handle_, sizeof(handle_));
    _memberSerialiser.addPrimitive(&_type, sizeof(_type));
    _memberSerialiser.addPrimitive(&_linkStatus, sizeof(_linkStatus));

    _memberSerialiser.setInitialised();
}

Entity::~Entity()
{
}

bool Entity::isNull() const
{
    return handle_ == INVALID_EHANDLE;
}

Entity::EHandle_t Entity::getHandle() const
{
    return handle_;
}

unsigned int Entity::getType() const
{
    return _type;
}

std::string Entity::logString(const Database* db) const
{
    std::string typeStr = std::to_string(_type);

    return std::string("Entity(t=")
        + typeStr
        + std::string(", h=")
        + std::to_string(handle_)
        + std::string(", [")
        + std::to_string(_propertyTable.size())
        + std::string("])");
}

bool Entity::memberwiseEqual(const Entity *other) const
{
    if (_locked != other->_locked ||
            handle_ != other->handle_ ||
            _type != other->_type ||
            _linkStatus != other->_linkStatus ||
            _propertyTable.size() != other->_propertyTable.size() )
        return false;


    for ( auto it = _propertyTable.begin(); it != _propertyTable.end(); ++it )
    {
        const EntityProperty* thisProp = it->second.get();

        auto otherIt = other->_propertyTable.find(it->first);
        if ( otherIt == other->_propertyTable.end() )
            return false;

        const EntityProperty* otherProp = otherIt->second.get();
        if ( !thisProp->memberwiseEqual(otherProp) )
            return false;
    }

    return true;
}
