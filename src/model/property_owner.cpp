#include "./property_owner.h"

#include "./types/base.h"
#include "./types/confidence.h"
#include "./entity_manager.h"
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
	  //TODO : catch this?
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
	std::vector<BasePointer> results(values.size());
	std::vector<BasePointer>::iterator endIter = results.begin();

	if (obj.type == model::Object::Type::ENTITYREF) {

		for (auto iter = values.cbegin(); iter != values.cend(); iter++) {
			
			// if it is an exact match, return don't remove
			if ((*iter)->Equals(obj) > 0) {
				*endIter = *iter;
				endIter++;
			}
			
			auto entity = Singletons::cDatabase()->entityManager().getEntity(std::stoull(obj.value));

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
					*endIter = *iter;
					endIter++;
				}
			}
			
			if (state != MatchState::DownTree && entity->hasProperty(3)) {
				auto subSet = entity->getProperty(3)->baseTop();
				auto result = this->meetsCondition(propertyId, std::move(subSet), MatchState::UpTree);

				if (result.size() > 0) {
					auto matchedEntity = Singletons::cDatabase()->entityManager().getEntity(std::stoull(result[0]->toString()));
					unsigned int newConf = result[0]->confidence() / matchedEntity->getProperty(4)->count();
					*endIter = (*iter)->Clone();
					auto confidenceRecord = std::dynamic_pointer_cast<model::types::Confidence>((*endIter)->getProperty(8)->baseTop()->Clone());
					confidenceRecord->value(newConf);
					(*endIter)->insertProperty(8, confidenceRecord, MatchState::None, EntityProperty::Type::CONCRETESINGLE);
					endIter++;
				}
			}
		}
	}
	else {
		endIter = std::copy_if(values.cbegin(), values.cend(), results.begin(), [obj](BasePointer ptr) {
			return ptr->Equals(obj) > 0; 
		});
	}    
	results.resize(std::distance(results.begin(), endIter));
    return results;
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
