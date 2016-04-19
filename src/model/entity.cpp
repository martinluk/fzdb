#include "./entity.h"
#include "./entity_manager.h"
#include "../singletons.h"
#include "./types/string.h"
#include "./types/uint.h"
#include "./types/entity_ref.h"
#include "./types/type_id.h"

#include <spdlog/spdlog.h>

#include <stdexcept>
#include <string>

const Entity::EHandle_t Entity::INVALID_EHANDLE = 0;

Entity::Entity(unsigned int type, EHandle_t handle) : _handle(handle), _linkStatus(Entity::LinkStatus::None)
{
	auto newRecord = std::make_shared<model::types::TypeID>(type);
	newRecord->Init(100);
	insertProperty(2, newRecord, MatchState::None, EntityProperty::Type::CONCRETESINGLE);
    initMemberSerialiser();
}

Entity::Entity(unsigned int type, EHandle_t handle, const PropertyOwner && base) :  PropertyOwner(base)
{
	_handle = handle;
	auto newRecord = std::make_shared<model::types::TypeID>(type);
	newRecord->Init(100);
	insertProperty(2, newRecord, MatchState::None, EntityProperty::Type::CONCRETESINGLE);
	initMemberSerialiser();
}

Entity::Entity(unsigned int type) : Entity(type, Entity::INVALID_EHANDLE)
{
}

std::vector<std::shared_ptr<model::types::Base>> Entity::meetsCondition(unsigned int propertyId, const model::Object && obj, MatchState state)
{
	//handle linking
	switch (_linkStatus) {
	case LinkStatus::Master: {
		std::vector<std::shared_ptr<model::types::Base>> results = PropertyOwner::meetsCondition(propertyId, std::move(obj));
		auto graph = Singletons::database()->entityManager().getLinkGraph(_handle, std::set<Entity::EHandle_t>());
		for (auto entId : graph) {
			if (entId == _handle) continue;
			auto subResult = Singletons::database()->entityManager().getEntity(entId)->meetsCondition(propertyId, std::move(obj), MatchState::Linked);
			results.insert(results.end(), subResult.begin(), subResult.end());
		}
		return results;
	}
	case LinkStatus::Slave:
		if(state == MatchState::None) return std::vector<std::shared_ptr<model::types::Base>>();
	case LinkStatus::None:
		return PropertyOwner::meetsCondition(propertyId, std::move(obj), state);
	default:
		// link status must be equal to one of the above values
		assert(false);
		return std::vector<std::shared_ptr<model::types::Base>>();
	}	
}

std::vector<std::shared_ptr<model::types::Base>> Entity::meetsCondition(unsigned int propertyId, const std::shared_ptr<model::types::Base>&& value, MatchState state)
{
	if (value->subtype() == model::types::SubType::EntityRef) return meetsCondition(propertyId, model::Object(model::Object::Type::ENTITYREF, value->toString()), state);
	return meetsCondition(propertyId, model::Object(model::Object::Type::STRING, value->toString()), state);
}

bool Entity::hasProperty(const unsigned int & key, MatchState state) const
{
	return PropertyOwner::hasProperty(key);
}

std::shared_ptr<EntityProperty> Entity::getProperty(const unsigned int & key) const
{
	//this needs to be linkified too
	return PropertyOwner::getProperty(key);	
}

void Entity::removeProperty(const unsigned int & key)
{
	PropertyOwner::removeProperty(key);	
}

void Entity::insertProperty(std::shared_ptr<EntityProperty> prop, MatchState state)
{
	switch (prop->key()) {
	case 3: //subset
		if(prop->subtype() != model::types::SubType::EntityRef) {
			throw std::runtime_error("subset must be an entity");
		}

		if (state == MatchState::None) {
			std::shared_ptr<model::types::EntityRef> entityRef = std::dynamic_pointer_cast<model::types::EntityRef>(prop->baseTop());
			std::shared_ptr<Entity> otherEntity = Singletons::database()->entityManager().getEntity(entityRef->value());
			auto newRecord = std::make_shared<model::types::EntityRef>(_handle);
			newRecord->Init(prop->baseTop()->confidence());
			otherEntity->insertProperty(4, newRecord, MatchState::Linked, EntityProperty::Type::CONCRETEMULTI);
		}
		PropertyOwner::insertProperty(prop, state);
		break;
	case 4: //superset
		if (prop->subtype() != model::types::SubType::EntityRef) {
			throw std::runtime_error("superset must be an entity");
		}
		if (state == MatchState::None) {
			std::shared_ptr<model::types::EntityRef> entityRef = std::dynamic_pointer_cast<model::types::EntityRef>(prop->baseTop());
			std::shared_ptr<Entity> otherEntity = Singletons::database()->entityManager().getEntity(entityRef->value());
			auto newRecord = std::make_shared<model::types::EntityRef>(_handle);
			newRecord->Init(prop->baseTop()->confidence());
			otherEntity->insertProperty(3, newRecord, MatchState::Linked, EntityProperty::Type::CONCRETESINGLE);
		}
		PropertyOwner::insertProperty(prop, state);
		break;
	default:
		PropertyOwner::insertProperty(prop, state);
	}	
}

void Entity::insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object, MatchState state, EntityProperty::Type propType)
{
	switch (key) {
	case 3: { //subset 
		auto entityRef = std::dynamic_pointer_cast<model::types::EntityRef>(object);
		if (!entityRef) {
			throw std::runtime_error("subset must be an entity");
		}

		if (state == MatchState::None) {
			std::shared_ptr<model::types::EntityRef> entityRef = std::dynamic_pointer_cast<model::types::EntityRef>(object);
			std::shared_ptr<Entity> otherEntity = Singletons::database()->entityManager().getEntity(entityRef->value());
			auto newRecord = std::make_shared <model::types::EntityRef>(_handle);
			newRecord->Init(object->confidence());
			otherEntity->insertProperty(4, newRecord, MatchState::Linked, EntityProperty::Type::CONCRETEMULTI);
		}
		PropertyOwner::insertProperty(key, object, state, EntityProperty::Type::CONCRETESINGLE);

		break;
	}
	case 4: { //superset
		auto entityRef = std::dynamic_pointer_cast<model::types::EntityRef>(object);
		if (!entityRef) {
			throw std::runtime_error("superset must be an entity");
		}

		if (state == MatchState::None) {
			std::shared_ptr<model::types::EntityRef> entityRef = std::dynamic_pointer_cast<model::types::EntityRef>(object);
			std::shared_ptr<Entity> otherEntity = Singletons::database()->entityManager().getEntity(entityRef->value());
			auto newRecord = std::make_shared<model::types::EntityRef>(_handle);
			newRecord->Init(object->confidence());
			otherEntity->insertProperty(3, newRecord, MatchState::Linked, EntityProperty::Type::CONCRETESINGLE);
		}
		PropertyOwner::insertProperty(key, object, state, EntityProperty::Type::CONCRETEMULTI);

		break;
	}
	default:
		PropertyOwner::insertProperty(key, object, state, propType);
	}	
}

void Entity::initMemberSerialiser()
{
    _memberSerialiser.addPrimitive(&_handle, sizeof(_handle));

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
	return std::dynamic_pointer_cast<model::types::TypeID>(getProperty(2)->baseTop())->value();
}

std::string Entity::logString(const Database* db) const
{
	//todo - fix this
	std::string typeStr = "type";// std::to_string("type");

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
