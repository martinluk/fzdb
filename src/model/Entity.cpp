#include "./Entity.h"
#include "./EntityManager.h"
#include "../singletons.h"
#include "./types/String.h"

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

std::vector<std::shared_ptr<model::types::Base>> Entity::meetsCondition(unsigned int propertyId, const model::Object && obj)
{
	//handle type comparison
	if (propertyId == 2) {
		if (obj.type == model::Object::Type::STRING && Singletons::database()->entityManager().getTypeID(obj.value) == _type) {
			return std::vector<std::shared_ptr<model::types::Base>>{
				std::make_shared<model::types::String>(obj.value, 0)
			};
		}
		else {
			return std::vector<std::shared_ptr<model::types::Base>>();
		}
	}
	return PropertyOwner::meetsCondition(propertyId, std::move(obj));
}

bool Entity::hasProperty(const unsigned int & key) const
{
	if (key == 2) return true; //return true if it's type
	return PropertyOwner::hasProperty(key);
}

std::shared_ptr<EntityProperty> Entity::getProperty(const unsigned int & key) const
{
	if (key == 2) {
		auto output = std::make_shared<EntityProperty>(2, model::types::SubType::TypeString);
		output->append(std::make_shared<model::types::String>(Singletons::database()->entityManager().getTypeName(_type), 0));
		return output;
	}
	return PropertyOwner::getProperty(key);
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