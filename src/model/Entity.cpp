#include "./Entity.h"
#include "./EntityManager.h"
#include "../singletons.h"
#include "./types/String.h"

#include <stdexcept>
#include <string>

const Entity::EHandle_t Entity::INVALID_EHANDLE = 0;

Entity::Entity(unsigned int type) : _handle(Entity::INVALID_EHANDLE), _type(type), _linkStatus(Entity::LinkStatus::None)
{
    initMemberSerialiser();
}

Entity::Entity(unsigned int type, EHandle_t handle) : _handle(handle), _type(type), _linkStatus(Entity::LinkStatus::None)
{
    initMemberSerialiser();
}

std::vector<std::shared_ptr<model::types::Base>> Entity::meetsCondition(unsigned int propertyId, const model::Object && obj, bool linked)
{
	//handle type comparison
	if (propertyId == 2) {
        if (obj.type == model::Object::Type::STRING)
        {
            if ( !_manager )
                throw std::runtime_error("Cannot return entity type string: manager does not exist.");

            if ( _manager->getTypeID(obj.value) == _type )
            {
                return std::vector<std::shared_ptr<model::types::Base>>{
                    std::make_shared<model::types::String>(obj.value, 0)
                };
            }
		}
		else {
			return std::vector<std::shared_ptr<model::types::Base>>();
		}
	}

	//handle linking
	switch (_linkStatus) {
	case LinkStatus::Master: {
		std::vector<std::shared_ptr<model::types::Base>> results = PropertyOwner::meetsCondition(propertyId, std::move(obj));
		auto graph = Singletons::database()->entityManager().getLinkGraph(_handle, std::set<Entity::EHandle_t>());
		for (auto entId : graph) {
			if (entId == _handle) continue;
			auto subResult = Singletons::database()->entityManager().getEntity(entId)->meetsCondition(propertyId, std::move(obj), true);
			results.insert(results.end(), subResult.begin(), subResult.end());
		}
		return results;
	}
	case LinkStatus::Slave:
		if(!linked) return std::vector<std::shared_ptr<model::types::Base>>();
	case LinkStatus::None:
		return PropertyOwner::meetsCondition(propertyId, std::move(obj));
	}	
}

bool Entity::hasProperty(const unsigned int & key, bool linked) const
{
	if (key == 2) return true; //return true if it's type
							   //handle linking
	/*switch (_linkStatus) {
	case LinkStatus::Master: {
		if (PropertyOwner::hasProperty(key)) return true;
		auto graph = Singletons::database()->entityManager().getLinkGraph(_handle, std::set<Entity::EHandle_t>());
		for (auto entId : graph) {
			if (entId == _handle) continue;
			if (Singletons::database()->entityManager().getEntity(entId)->hasProperty(key, true)) return true;
		}
		return false;
	}
	case LinkStatus::Slave:
		if (!linked) return false;
	case LinkStatus::None:
		return PropertyOwner::hasProperty(key);
	}*/
	return PropertyOwner::hasProperty(key);
}

std::shared_ptr<EntityProperty> Entity::getProperty(const unsigned int & key) const
{
	if (key == 2) {
		auto output = std::make_shared<EntityProperty>(2, model::types::SubType::TypeString);

        if ( !_manager )
            throw std::runtime_error("Cannot return entity type string: manager does not exist.");

        output->append(std::make_shared<model::types::String>(_manager->getTypeName(_type), 0));

		return output;
	}

	//this needs to be linkified too
	return PropertyOwner::getProperty(key);	
}

void Entity::initMemberSerialiser()
{
    _memberSerialiser.addPrimitive(&_handle, sizeof(_handle));
    _memberSerialiser.addPrimitive(&_locked, sizeof(_locked));

    _memberSerialiser.addPrimitive(&_type, sizeof(_type));
    _memberSerialiser.addPrimitive(&_linkStatus, sizeof(_linkStatus));

    _memberSerialiser.setInitialised();
}

Entity::~Entity()
{
}

bool Entity::isNull() const
{
    return _handle == INVALID_EHANDLE;
}

Entity::EHandle_t Entity::getHandle() const
{
    return _handle;
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
        + std::to_string(_handle)
        + std::string(", [")
        + std::to_string(_propertyTable.size())
        + std::string("])");
}

bool Entity::memberwiseEqual(const Entity *other) const
{
    if (_locked != other->_locked ||
            _handle != other->_handle ||
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
