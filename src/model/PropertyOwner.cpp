#include "./PropertyOwner.h"

#include "./types/Base.h"
#include "./EntityManager.h"
#include "../singletons.h"

PropertyOwner::PropertyOwner() : _locked(false), _manager(NULL)
{
	initMemberSerialiser();
}

PropertyOwner::~PropertyOwner()
{
}

// Getters:

//std::shared_ptr<EntityProperty> PropertyOwner::getProperty(const unsigned int &key) const {
//    auto it = _propertyTable.find(key);
//    return it->second;
//}

// Setters:

// Removes the property with the given key.
void PropertyOwner::removeProperty(const unsigned int &key) {

  checkLock();

  try
  {
    //delete _propertyTable.at(key);
    _propertyTable.erase(key);
  }
  catch (const std::out_of_range &ex)
  {
  }
}

// Tests if the entity has a property
bool PropertyOwner::hasProperty(const unsigned int &key, MatchState state) const {
    return _propertyTable.find(key) != _propertyTable.cend();
}

// Returns read only reference to the property table
const std::map<unsigned int, std::shared_ptr<EntityProperty>>& PropertyOwner::properties() const {
    return _propertyTable;
}

// Tests if the entity meets the condition
std::vector<BasePointer> PropertyOwner::meetsCondition(unsigned int propertyId, const model::Object&& obj, MatchState state) {
    if (!hasProperty(propertyId)) return std::vector<BasePointer>();
    std::vector<BasePointer> values = getProperty(propertyId)->baseValues();
	if (obj.type == model::Object::Type::ENTITYREF) {

		for (auto iter = values.begin(); iter != values.end(); ) {
			
			// if it is an exact match, return don't remove
			if ((*iter)->Equals(obj)) {
				iter++;
				continue;
			}
			
			auto entity = Singletons::cDatabase()->entityManager().getEntity(std::stoul(obj.value));

			if (state != MatchState::UpTree && entity->hasProperty(4)) {
				auto subSets = entity->getProperty(4)->baseValues();
				bool met = false;
				for (auto subSet : subSets) {
					if (this->meetsCondition(propertyId, std::move(subSet), MatchState::DownTree).size() > 0) {
						met = true;
						break;
					}
				}
				if (met) {
					iter++;
					continue;
				}
			}
			
			if (state != MatchState::DownTree && entity->hasProperty(3)) {
				auto subSet = entity->getProperty(3)->baseTop();
				if (this->meetsCondition(propertyId, std::move(subSet), MatchState::UpTree).size() > 0) {
					iter++;
					continue;
				}
			}

			iter = values.erase(iter);
		}
	}
	else {
		values.erase(std::remove_if(values.begin(), values.end(), [obj](BasePointer ptr) { return !ptr->Equals(obj); }), values.end());
	}    
    return values;
}

std::vector<std::shared_ptr<model::types::Base>> PropertyOwner::meetsCondition(unsigned int propertyId, const std::shared_ptr<model::types::Base>&& value, MatchState state)
{
	if(value->subtype() == model::types::SubType::EntityRef) return meetsCondition(propertyId, model::Object(model::Object::Type::ENTITYREF, value->toString()), state);
	return meetsCondition(propertyId, model::Object(model::Object::Type::STRING, value->toString()), state);
}

// Clears all properties on the entity.
void PropertyOwner::clearProperties() {
  checkLock();
  _propertyTable.clear();
}

// Returns the number of properties present.
int PropertyOwner::propertyCount() const {
    return _propertyTable.size();
}

void PropertyOwner::initMemberSerialiser()
{
	_memberSerialiser.addPrimitive(&_locked, sizeof(_locked));
	_memberSerialiser.setInitialised();
}

MemberSerialiser & PropertyOwner::memberSerialiser()
{
	return _memberSerialiser;
}

std::shared_ptr<EntityProperty> PropertyOwner::getProperty(const unsigned int &key) const {
  return _propertyTable.at(key);
}

void PropertyOwner::insertProperty(std::shared_ptr<EntityProperty> prop, MatchState state) {

  checkLock();

  for (auto value : prop->baseValues())
  {
    value->_manager = _manager;
  }

  if (hasProperty(prop->key())) {
    auto existingProp = getProperty(prop->key());
    for (auto value : prop->baseValues()) {
      existingProp->append(value);
    }
  }
  else {
    auto pair = std::make_pair<unsigned int, std::shared_ptr<EntityProperty>>(std::move(prop->key()), std::move(prop));
    _propertyTable.insert(pair);
  }
}

void PropertyOwner::insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object, MatchState state, EntityProperty::Type propType) {

  checkLock();
  object->_manager = _manager;
  //should this be 100?
 // object->Init(object->confidence());

  if (!hasProperty(key)) {

	  if (propType == EntityProperty::Type::LOCKED) {
		  auto pair = std::make_pair<unsigned int, std::shared_ptr<EntityProperty>>(std::move(key), std::make_shared<EntityProperty>(EntityProperty::Type::CONCRETESINGLE, key, object->subtype()));
		  pair.second->append(object);
		  pair.second->lock();
		  _propertyTable.insert(pair);
	  }
	  else {
		  auto pair = std::make_pair<unsigned int, std::shared_ptr<EntityProperty>>(std::move(key), std::make_shared<EntityProperty>(propType, key, object->subtype()));
		  pair.second->append(object);
		  _propertyTable.insert(pair);
	  }   
  }
  else {
    auto prop = getProperty(key);
    prop->append(object);
  }
}
