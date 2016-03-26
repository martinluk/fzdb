#include "./EntityManager.h"
#include <cassert>
#include <set>
#include <sstream>
#include "../FileSystem.h"
#include "GraphSerialiser.h"
#include "../Util.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include "../Parser.h"
#include "spdlog/spdlog.h"
#include "../Util.h"

#include "./types/Base.h"
#include "./types/ValueRef.h"
#include "./types/String.h"
#include "./types/EntityRef.h"
#include "./types/Int.h"
#include "./types/Date.h"
#include "singletons.h"

static const unsigned int ENTITY_TYPE_GENERIC = 0;

EntityManager::EntityManager()
{
	_lastHandle = Entity::INVALID_EHANDLE;
	_lastProperty = 0;
	_lastTypeID = 0;
}

EntityManager::~EntityManager()
{
}

std::shared_ptr<Entity> EntityManager::createEntity(const std::string &type)
{
	unsigned int typeID = getTypeID(type);

	_lastHandle++;
	assert(_lastHandle != Entity::INVALID_EHANDLE);

	std::shared_ptr<Entity> e = std::make_shared<Entity>(typeID, _lastHandle);
	_entities.insert(std::pair<Entity::EHandle_t, std::shared_ptr<Entity> >(_lastHandle, e));

	return e;
}

// Basic Graph Pattern
// When presented with a sanatised list of triples finds values for variables that satisfy that condition
VariableSet EntityManager::BGP(TriplesBlock triplesBlock, const QuerySettings settings) const
{
	VariableSet result(triplesBlock.variables);

	//sort by entropy
	triplesBlock.Sort();
	auto conditions = triplesBlock.triples;

	//iterate over conditions
	for (auto conditionsIter = conditions.cbegin(); conditionsIter != conditions.end(); conditionsIter++) {

		//There are 7 valid combinations

		if (conditionsIter->subject.type == model::Subject::Type::VARIABLE) {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 1 - $a <prop> value
					this->Scan1(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), std::move(conditionsIter->object), std::move(conditionsIter->meta_variable));
				}
				else {
					//option 2 - $a <prop> $b
					this->Scan2(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), conditionsIter->object.value, std::move(conditionsIter->meta_variable));
				}
			}
			else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option3 - $a $b value
					this->Scan3(std::move(result), conditionsIter->subject.value, conditionsIter->predicate.value, std::move(conditionsIter->object), std::move(conditionsIter->meta_variable));
				}
				else {
					//option 4 - $a $b $c
					this->Scan4(std::move(result), conditionsIter->subject.value, conditionsIter->predicate.value, conditionsIter->object.value, std::move(conditionsIter->meta_variable));
				}
			}
		}
		else {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//doesn't contain any variables.. is meaningless
					//TODO: UNLESS IN A META BLOCK
				}
				else {
					//option 5 - entity <prop> $c
					this->Scan5(std::move(result), std::move(conditionsIter->subject), std::move(conditionsIter->predicate), conditionsIter->object.value, std::move(conditionsIter->meta_variable));
				}
			}
	 		else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 7 - entity $b value
					this->Scan6(std::move(result), std::move(conditionsIter->subject), conditionsIter->predicate.value, std::move(conditionsIter->object), std::move(conditionsIter->meta_variable));
				}
				else {
					//option 8 - entity $b $c
					this->Scan7(std::move(result), std::move(conditionsIter->subject), conditionsIter->predicate.value, conditionsIter->object.value, std::move(conditionsIter->meta_variable));
				}
			}
		}
	}

	return result;
}

bool EntityManager::handleSpecialInsertOperations(Entity *entity, const model::Triple &triple, unsigned int author, const std::string &comment)
{
    // If we're setting the type
    if ( triple.predicate.value == ReservedProperties::TYPE )
    {
	    changeEntityType(std::stoll(triple.subject.value), triple.object.value);
	    return true;
    }
    
    enforceTypeHasBeenSet(entity);
    
    // If we're setting a superset or subset, we need to make sure the target (value) exists.
    // The object will have been created earlier.
    if ( triple.predicate.value == ReservedProperties::ORDER_SUPERSET_OF )
    {
	long long target = std::stoll(triple.object.value);
	if ( _entities.find(target) == _entities.end() )
	    throw std::runtime_error("'supersetOf' target with ID " + triple.object.value + " does not exist");
	
	spdlog::get("main")->info("Setting entity {} as superset of entity {})", entity->getHandle(), target);
	createHierarchy(entity->getHandle(), target, author, comment);
	return true;
    }
    if ( triple.predicate.value == ReservedProperties::ORDER_SUBSET_OF )
    {
	long long target = std::stoll(triple.object.value);
	if ( _entities.find(target) == _entities.end() )
	    throw std::runtime_error("'subsetOf' target with ID " + triple.object.value + " does not exist");
	
	spdlog::get("main")->info("Setting entity {} as subset of entity {})", entity->getHandle(), target);
	createHierarchy(target, entity->getHandle(), author, comment);
	return true;
    }
    
    return false;
}

void EntityManager::enforceTypeHasBeenSet(const Entity *entity)
{
    if ( entity->getType() == ENTITY_TYPE_GENERIC )
	throw std::runtime_error("Attempted operation on entity " + std::to_string(entity->getHandle())
                                 + " before assigning it a type!");
}

void EntityManager::enforceTypeHasBeenSet(const std::set<const Entity *> &ents)
{
    for ( const Entity* e : ents )
    {
	if ( e->getType() == ENTITY_TYPE_GENERIC )
	    throw std::runtime_error("Entity " + std::to_string(e->getHandle())
				     + " was not assigned a type!");
    }
}

//Inserts new data into the data store
void EntityManager::Insert(TriplesBlock&& block)
{
	//sort by entropy
	block.Sort();

	auto iter = block.triples.cbegin();
	auto end = block.triples.cend();
	VariableSet variableSet(block.metaVariables);

	// Keep track of any entity we've modified.
	// We use this later to make sure all entities have a type.
	std::set<const Entity *> modifiedEntities;

	for (; iter != end; iter++) {
		auto triple = *iter;

		unsigned char confidence = triple.object.hasCertainty ? triple.object.certainty : 100;
		std::shared_ptr<model::types::Base> newRecord;
		model::types::SubType newRecordType;

		switch (triple.object.type) {
		case model::Object::Type::STRING:
			newRecordType = model::types::SubType::TypeString;
			newRecord = std::make_shared<model::types::String>(triple.object.value, 0, confidence);
			break;
		case model::Object::Type::ENTITYREF:
			newRecordType = model::types::SubType::TypeEntityRef;
			newRecord = std::make_shared<model::types::String>(triple.object.value, 0, confidence);
			break;
		case model::Object::Type::INT:
			newRecordType = model::types::SubType::TypeInt32;
			newRecord = std::make_shared<model::types::String>(triple.object.value, 0, confidence);
			break;
		}

		unsigned int propertyId = this->getPropertyName(triple.predicate.value, newRecordType, true);
		
		switch (triple.subject.type) {

			case model::Subject::Type::ENTITYREF: {

				auto entity_id = std::stoll(triple.subject.value);

				//create the entity if it doesn't exist
				if (_entities.find(entity_id) == _entities.end())
				{
					_entities[entity_id] = std::make_shared<Entity>(ENTITY_TYPE_GENERIC, entity_id);
				}

				std::shared_ptr<Entity> currentEntity = _entities[entity_id];
				modifiedEntities.insert(currentEntity.get());

				// If we performed any special operations, skip to the next.
				// TODO: We haven't handled the author or comment at all here!
				if (handleSpecialInsertOperations(currentEntity.get(), triple, 0, std::string()))
					continue;

				enforceTypeHasBeenSet(currentEntity.get());

				//note that the OrderingId attribute of a record is assigned as part of insertion
				currentEntity->insertProperty(propertyId, newRecord);
				if (triple.meta_variable != "") {
					variableSet.add(std::move(triple.meta_variable),
						VariableSetValue(std::make_shared<model::types::ValueRef>(entity_id, propertyId, newRecord->OrderingId()), propertyId, entity_id),
						model::types::SubType::ValueReference);
				}
				break;
			} // END: case model::Subject::Type::ENTITYREF

			case model::Subject::Type::VARIABLE: {
				if (block.metaVariables.find(triple.subject.value) == block.metaVariables.end()) {
					throw std::runtime_error("Only meta-variables are allowed in INSERT statements at this time");
				}

				auto values = variableSet.getData(triple.subject.value);

				for (auto val : values) {
					std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(val.dataPointer());
					auto record = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
					record->insertProperty(propertyId, newRecord);
				}
				break;
			}
		
		}

		
	}
	
	// Ensure all the entities we processed have a type.
	enforceTypeHasBeenSet(modifiedEntities);
}

void EntityManager::changeEntityType(Entity::EHandle_t id, const std::string &type)
{
	unsigned int typeID = getTypeID(type);
	spdlog::get("main")->info("Setting entity {} type to {} (numeric id {})", id, type, typeID);
	
	auto it = _entities.find(id);
	if (it == _entities.end())
	{
		_entities[id] = std::make_shared<Entity>(typeID, id);
	}
	else
	{
		it->second->_type = typeID;
	}
}

std::shared_ptr<model::types::Base> EntityManager::dereference(Entity::EHandle_t entity, unsigned int prop, unsigned int val) const
{
	return _entities.at(entity)->getProperty(prop)->baseValue(val);
}

std::vector<std::shared_ptr<Entity>> EntityManager::entityList() const
{
    std::vector<std::shared_ptr<Entity>> list;
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        list.push_back(it->second);
    }
    
    return list;
}

// TODO: Do we need some sort of handle renumber function too?
void EntityManager::insertEntity(std::shared_ptr<Entity> ent)
{
    assert(ent->getHandle() != Entity::INVALID_EHANDLE);
    assert(_entities.find(ent->getHandle()) == _entities.end());
    
    if ( _lastHandle < ent->getHandle() )
        _lastHandle = ent->getHandle();
    
    _entities.insert(std::pair<Entity::EHandle_t, std::shared_ptr<Entity>>(ent->getHandle(), ent));
}

void EntityManager::clearAll()
{
    _entities.clear();
    _lastHandle = Entity::INVALID_EHANDLE;
    _lastProperty = 0;
	_lastTypeID = 0;
    _entityTypeNames.clear();
    _propertyNames.clear();
    _propertyTypes.clear();
	_links.clear();
}

std::size_t EntityManager::entityCount() const
{
    return _entities.size();
}

std::string EntityManager::dumpContents() const
{
    std::stringstream str;
    str << "Number of entities: " << _entities.size() << "\n";
	const Database* db = Singletons::database();
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        const std::shared_ptr<Entity> ent = it->second;
        str << ent->logString(db) << "\n";
        if ( ent->propertyCount() > 0 )
        {
            str << "{\n";
            
			const std::map<unsigned int, std::shared_ptr<EntityProperty>> &propMap = ent->properties();
            for ( auto it2 = propMap.cbegin(); it2 != propMap.cend(); ++it2 )
            {
				std::shared_ptr<EntityProperty> prop = it2->second;
                str << "  " << prop->logString(db) << "\n";
                
                if ( prop->count() > 0 )
                {
                    str << "  {\n";
                    
                    for ( int i = 0; i < prop->count(); i++ )
                    {
                        BasePointer val = prop->baseValue(i);
                        str << "    " << val->logString(db) << "\n";
                    }
                    
                    str << "  }\n";
                }
            }
            
            str << "}\n";
        }
    }
    
    return str.str();
}

bool EntityManager::saveToFile(const std::string &filename)
{
    Serialiser serialiser;
    GraphSerialiser gSer(this);
    gSer.serialise(serialiser);
	
	bool success = true;
	try
	{
		FileSystem::writeFile(filename, serialiser);
	}
	catch (const std::exception &)
	{
		success = false;
	}
    return success;
}

bool EntityManager::loadFromFile(const std::string &filename)
{
    std::size_t size = FileSystem::dataLength(filename);
    if ( size < 1 )
        return false;

    char* buffer = new char[size];
	
	bool success = true;
	try
	{
		FileSystem::readFile(filename, buffer, size);
	}
	catch (const std::exception &)
	{
		success = false;
	}
    
    if ( !success )
    {
        delete[] buffer;
        return false;
    }

    clearAll();
    GraphSerialiser gSer(this);
    gSer.unserialise(buffer, size);
    delete[] buffer;

    return true;
}

unsigned int EntityManager::getTypeID(const std::string &str)
{
	// "Generic" type is an empty string.
	// The ID for this is 0.
	if ( str.size() < 1 )
		return ENTITY_TYPE_GENERIC;

	unsigned int id = 0;
	try
	{
		id = _entityTypeNames.at(str);
	}
	catch (const std::exception&)
	{
		// The string was not in the table yet.
		// Assign it a new ID.
		_lastTypeID++;
		id = _lastTypeID;
		assert(id > 0);
		_entityTypeNames.insert(std::pair<std::string, unsigned int>(str, id));
	}

	return id;
}

#pragma region linkingandmerging

std::set<Entity::EHandle_t> EntityManager::getLinkGraph(const Entity::EHandle_t start, std::set<Entity::EHandle_t>&& visited) const {
	if (_links.find(start) == _links.cend()) {
		//Bad
	}
	
	std::set<Entity::EHandle_t> results;
	std::set_difference(_links.at(start).begin(), _links.at(start).end(), 
		visited.begin(), visited.end(),
		std::inserter(results, results.begin()));

	visited.insert(start);

	if (results.size() == 0)return visited;

	for (auto id : results) {
		this->getLinkGraph(id, std::move(visited));
	}

	return visited;
}

void EntityManager::linkEntities(const Entity::EHandle_t entityId, const Entity::EHandle_t entityId2) {

	//create link sets if they do not exists
	if (_links.find(entityId) == _links.end()) _links[entityId] = std::set<Entity::EHandle_t>();
	if (_links.find(entityId2) == _links.end()) _links[entityId2] = std::set<Entity::EHandle_t>();

	//add the new link
	_links[entityId].insert(entityId2);
	_links[entityId2].insert(entityId);

	//get the full graph of linked entities
	auto allLinked = getLinkGraph(entityId, std::set<Entity::EHandle_t>());
	Entity::EHandle_t lowestId = *std::min_element(allLinked.begin(), allLinked.end());

	//set and/or reset link status values
	for (auto id : allLinked) {
		_entities[id]->linkStatus(Entity::LinkStatus::Slave);
		if (id == lowestId) _entities[id]->linkStatus(Entity::LinkStatus::Master);
	}
}

void EntityManager::unlinkEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2) {

	//if they are not actually linked, return
	if (_links.find(entityId) == _links.end() || _links.find(entityId2) == _links.end() ||
		_links.find(entityId)->second.find(entityId2) == _links.find(entityId)->second.end() ||
		_links.find(entityId2)->second.find(entityId) == _links.find(entityId2)->second.end()) {
		return;
	}

	//remove the old link
	_links[entityId].erase(entityId2);
	_links[entityId2].erase(entityId);

	//get the new link graphs
	auto allLinked = getLinkGraph(entityId, std::set<Entity::EHandle_t>());
	auto allLinked2 = getLinkGraph(entityId2, std::set<Entity::EHandle_t>());

	//check if entities are now not linked at all
	if (allLinked.size() == 1) {
		_links.erase(entityId);
		_entities[entityId]->linkStatus(Entity::LinkStatus::None);
	}
	else {
		Entity::EHandle_t lowestId = *std::min_element(allLinked.begin(), allLinked.end());

		//set and/or reset link status values
		for (auto id : allLinked) {
			_entities[id]->linkStatus(Entity::LinkStatus::Slave);
			if (id == lowestId) _entities[id]->linkStatus(Entity::LinkStatus::Master);
		}
	}

	if (allLinked2.size() == 1) {
		_links.erase(entityId2);
		_entities[entityId2]->linkStatus(Entity::LinkStatus::None);
	}
	else {
		Entity::EHandle_t lowestId = *std::min_element(allLinked2.begin(), allLinked2.end());

		//set and/or reset link status values
		for (auto id : allLinked2) {
			_entities[id]->linkStatus(Entity::LinkStatus::Slave);
			if (id == lowestId) _entities[id]->linkStatus(Entity::LinkStatus::Master);
		}
	}

}

// Merges the entities with the given Ids. The entity with the higher Id number is deleted.
void EntityManager::mergeEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2) {

	//we will keep the entity with the lower id
	Entity::EHandle_t keep, lose;
	auto sorted = std::minmax(entityId, entityId2);
	keep = sorted.first;
	lose = sorted.second;

	//get the entities
	std::shared_ptr<Entity> keepEntity = _entities[keep],
		loseEntity = _entities[lose];

	//check we are merging entities of the same type
	if (keepEntity->getType() != loseEntity->getType()) {
		throw std::runtime_error("Attempted to merge entities of different types");
	}

	//copy the properties of loseEntity to keepEntity
	for (auto prop : loseEntity->properties()) {
		keepEntity->insertProperty(prop.second);
	}

	//delete EntityManager's ownership of loseEntity (will be automatically deleted)
	_entities.erase(lose);
}

#pragma endregion linkingandmerging

void EntityManager::createHierarchy(Entity::EHandle_t high, Entity::EHandle_t low, unsigned int author, const std::string &comment)
{
    using namespace model::types;
    typedef std::shared_ptr<EntityRef> EntRefPtr;
    typedef std::shared_ptr<EntityProperty> EntPropertyPtr;
    typedef std::shared_ptr<Entity> EntPtr;
    
    // Assuming entity handles are valid - do we need to check?
    
	// High is a superset of low.
	// Low is a subset of high.
    EntPtr pHigh = _entities[high];
    EntPtr pLow = _entities[low];
    
    unsigned int superProperty = getPropertyName(ReservedProperties::ORDER_SUPERSET_OF, model::types::SubType::TypeEntityRef, true);
    unsigned int subProperty = getPropertyName(ReservedProperties::ORDER_SUBSET_OF, model::types::SubType::TypeEntityRef, true);
    
    // The superset entity holds the subset property pointing to the subset entity, and vice versa.
    // Lambdas are wonderful for convenience.
    auto propNotPresent = [&] (EntPtr e, Entity::EHandle_t h, unsigned int prop)
    {
		std::vector<BasePointer> vals;
		vals.push_back(EntRefPtr(new EntityRef(h, author, 100, comment)));
		EntPropertyPtr p(new EntityProperty(prop, model::types::SubType::TypeEntityRef, vals));
		e->insertProperty(p);
    };
    
    auto propPresent = [&] (EntPtr e, Entity::EHandle_t h, unsigned int prop)
    {
		EntPropertyPtr p = e->getProperty(prop);
		p->append(BasePointer(new EntityRef(h, author, 100, comment)));
    };
    
	// This sets pHigh <supersetOf> entity:low
    if ( !pHigh->hasProperty(superProperty) )
		propNotPresent(pHigh, low, superProperty);
    else
		propPresent(pHigh, low, superProperty);
    
	// This sets pLow <subsetOf> entity:high
    if ( !pLow->hasProperty(subProperty) )
		propNotPresent(pLow, high, subProperty);
    else
		propPresent(pLow, high, subProperty);
}

void EntityManager::removeHierarchy(Entity::EHandle_t high, Entity::EHandle_t low)
{
    using namespace model::types;
    typedef std::shared_ptr<EntityRef> EntRefPtr;
    typedef std::shared_ptr<EntityProperty> EntPropertyPtr;
    typedef std::shared_ptr<Entity> EntPtr;
    
    // Assuming entity handles are valid - do we need to check?
    
    EntPtr pHigh = _entities[high];
    EntPtr pLow = _entities[low];
    
    unsigned int superProperty = getPropertyName(ReservedProperties::ORDER_SUPERSET_OF, model::types::SubType::TypeEntityRef, true);
    unsigned int subProperty = getPropertyName(ReservedProperties::ORDER_SUBSET_OF, model::types::SubType::TypeEntityRef, true);
    
    // Remove the entity references from the properties if they exist.
    if ( pHigh->hasProperty(superProperty) )
    {
		EntPropertyPtr p = pHigh->getProperty(superProperty);
		p->remove(EntityRef(low, 0));
    }
    
    if ( pHigh->hasProperty(subProperty) )
    {
		EntPropertyPtr p = pLow->getProperty(subProperty);
		p->remove(EntityRef(high, 0));
    }
}

#pragma region scan_functions

//$a <predicate> <object>
void EntityManager::Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const {
	
	//get the property id
	// Jonathan: Changed this to pass an undefined type, which skips type checking.
	// We don't know what the type is in advance!
	unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::TypeUndefined);

	//the variable has been used before, we only need to iterate over valid values from before
	if (variableSet.used(variableName)) {

		unsigned char varIndex = variableSet.indexOf(variableName);

		//in this scan we can only remove variables from variableset
		switch (variableSet.typeOf(variableName)) {
			case model::types::SubType::TypeEntityRef: {				

				variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
					[&, this, varIndex](std::vector<VariableSetValue> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex].dataPointer())->value();
					auto currentEntity = _entities.at(entityHandle);
					auto matches = currentEntity->meetsCondition(propertyId, std::move(object));
					return matches.size() == 0;
				}), variableSet.getData()->end());

				break;
			}
			
			case model::types::SubType::ValueReference: {

				for (auto iter = variableSet.getData()->cbegin(); iter != variableSet.getData()->cend(); iter++) {
					if (!(*iter)[varIndex].dataPointer()) continue;
					auto vsv = (*iter)[varIndex];
					std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
					auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
					//check if val meets the property
					auto matches = val->meetsCondition(propertyId, std::move(object));
					if (matches.size() == 0) {
						//delete associated answers
						variableSet.removeMetaRefs(vsv.metaRef());
					}
				}
				
				break;
			}

			default: {
				throw std::runtime_error("The first element in a triple MUST be an entity or a meta variable!");
			}
		}
		return;
	}

	//the variable has not been used, add all the values found!
	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto currentEntity = iter->second;
		std::vector<std::shared_ptr<model::types::Base>> matches;

		switch (currentEntity->linkStatus()) {

		case Entity::LinkStatus::None:
			matches = currentEntity->meetsCondition(propertyId, std::move(object));
			if (matches.size() > 0) {
				VariableSetValue vsv = VariableSetValue(std::make_shared<model::types::EntityRef>(currentEntity->getHandle(), 0), 0, currentEntity->getHandle());
				
				if (metaVar != "") {

					unsigned int metaRef = variableSet.getMetaRef();
					vsv.metaRef(metaRef);

					for (auto match : matches) {
						//TODO:
						//is it necessery to repeat the entity and property handles?
						auto valueRef = std::make_shared<model::types::ValueRef>(currentEntity->getHandle(), propertyId, match->OrderingId());
						auto vsv2 = VariableSetValue(valueRef, propertyId, currentEntity->getHandle());
						vsv2.metaRef(metaRef);
						variableSet.add(std::move(metaVar), std::move(vsv2), model::types::SubType::ValueReference);
					}					
				}

				variableSet.add(std::move(variableName), std::move(vsv), model::types::SubType::TypeEntityRef);
			}
			break;
		case Entity::LinkStatus::Slave:
			continue;
		case Entity::LinkStatus::Master:
			auto graph = getLinkGraph(currentEntity->handle_, std::set<Entity::EHandle_t>());
			for (auto entId : graph) {
				auto matches = _entities.at(entId)->meetsCondition(propertyId, std::move(object));
				if (matches.size() > 0) {
					variableSet.add(std::move(variableName), 
						VariableSetValue(std::make_shared<model::types::EntityRef>(currentEntity->getHandle(), 0), 0, entId),
						model::types::SubType::TypeEntityRef);
					break;
				}
			}
			
			break;
		}
	}
}


//$a <predicate> $b
void EntityManager::Scan2(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const {
	
	//get the property id
	const unsigned int propertyId = this->getPropertyName(predicate.value);

	//TODO: consider the case where variableName2 is already in variableSet

	//the variable has been used before, we only need to iterate over valid values from before
	if (variableSet.used(variableName)) {

		unsigned char varIndex = variableSet.indexOf(variableName),
			varIndex2 = variableSet.indexOf(variableName2);

		switch (variableSet.typeOf(variableName)) {

			case model::types::SubType::TypeEntityRef: {

				variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
					[&, this, varIndex](std::vector<VariableSetValue> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex].dataPointer())->value();
					auto currentEntityIter = _entities.find(entityHandle);
					std::shared_ptr<Entity> currentEntity;
					if (currentEntityIter != _entities.end()) {
						currentEntity = _entities.at(entityHandle);
					} else {
						throw std::runtime_error("Attempted to look up a non-existent entity");
					}					
					return !currentEntity->hasProperty(propertyId);
				}), variableSet.getData()->end());

				//TODO: but what about the type of the new data being added? :/
				for (auto iter = variableSet.getData()->begin(); iter != variableSet.getData()->end(); iter++) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[varIndex].dataPointer())->value();
					auto currentEntityIter = _entities.find(entityHandle);
					std::shared_ptr<Entity> currentEntity;
					if (currentEntityIter != _entities.end()) {
						currentEntity = _entities.at(entityHandle);
					}
					else {
						throw std::runtime_error("Attempted to look up a non-existent entity");
					}
					(*iter).emplace(iter->begin() + varIndex2, VariableSetValue(currentEntity->getProperty(propertyId)->baseValue(0)->Clone(), 0, 0));
				}
			} // /model::types::SubType::TypeEntityRef

			case model::types::SubType::ValueReference: {

				for (auto iter = variableSet.getData()->cbegin(); iter != variableSet.getData()->cend(); iter++) {
					if (!(*iter)[varIndex].dataPointer()) continue;
					auto vsv = (*iter)[varIndex];
					std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
					auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
					//check if val meets the property
					if (val->hasProperty(propertyId)) {
						auto subprop = val->getProperty(propertyId);
						//TODO: should be all values
						auto valueToStore = subprop->baseTop()->Clone();
						variableSet.addToMetaRefRow(vsv.metaRef(), varIndex, VariableSetValue(subprop->baseTop()->Clone(), propertyId, vsv.entity()));
					}
				}
			}
		
		}


		if (variableSet.typeOf(variableName) == model::types::SubType::TypeEntityRef) {

			
		}
		else {
			//TODO: TypeException
		}
		return;
	}

	//the variable has not been used, add all the values found!
	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto rows = Scan5(std::move(variableSet),
			model::Subject(model::Subject::Type::ENTITYREF, 
				std::to_string(iter->first)), std::move(predicate), variableName2, std::move(metaVar));

		for (auto row : rows) {
			variableSet.add(std::move(variableName),
				VariableSetValue(std::make_shared<model::types::EntityRef>(iter->second->getHandle(), 0), propertyId, iter->second->handle_),
				model::types::SubType::TypeEntityRef, row);
		}

	}
}

//$a $b "Value"
void EntityManager::Scan3(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const {

	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto rows = Scan6(std::move(variableSet),
			model::Subject(model::Subject::Type::ENTITYREF,
				std::to_string(iter->first)), variableName2, std::move(object), std::move(metaVar));

		for (auto row : rows) {
			variableSet.add(std::move(variableName),
				VariableSetValue(std::make_shared<model::types::EntityRef>(iter->second->getHandle(), 0), 0, iter->second->handle_),
				model::types::SubType::TypeEntityRef, row);
		}

	}
}

//$a $b $c
void EntityManager::Scan4(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const {
	
	//TODO: Check variable types

	//TODO: Cover cases where the variables have been used before :/
	if (variableSet.contains(variableName) || variableSet.contains(variableName2) || variableSet.contains(variableName3)) {
		return;
	}

	//If no variables have been used so far, get EVERYTHING!

	for (auto entity : _entities) {
		for (auto prop : entity.second->properties()) {

			variableSet.add(std::move(variableName), 
				VariableSetValue(std::make_shared<model::types::EntityRef>(entity.first, 0), prop.first, entity.first),
				model::types::SubType::TypeEntityRef);

			variableSet.add(std::move(variableName2),
				VariableSetValue(std::make_shared<model::types::Int>(prop.first, 0), prop.first, entity.first),
				model::types::SubType::PropertyReference);

			auto type = _propertyTypes.at(prop.first);
			std::shared_ptr<model::types::Base> val = entity.second->getProperty(prop.first)->baseValue(0)->Clone();
			variableSet.add(std::move(variableName3),
				VariableSetValue(std::move(val), prop.first, entity.first),
				std::move(type));
		}
	}
}

//entity <predicate> $c
std::vector<unsigned int> EntityManager::Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName, const std::string&& metaVar) const {
	
	//TODO: Check variable types
	//TODO : Linkify

	//get the entity handle
	Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
	std::vector<unsigned int> rowsAdded;

	//get the property id
	// Jonathat: Passing TypeUndefined to ckip type checking, since we don't know
	// what the type is yet.
	const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::TypeUndefined);

	if (EntityExists(entityRef)) {
		auto entity = _entities.at(entityRef);
		if (entity->hasProperty(propertyId)) {
			for (auto value : entity->getProperty(propertyId)->baseValues()) {
				rowsAdded.push_back(variableSet.add(std::move(variableName),
					VariableSetValue(value->Clone(), propertyId, entityRef), 
					std::move(_propertyTypes.at(propertyId))));
			}			
		}
	}

	return rowsAdded;
}

std::vector<unsigned int> EntityManager::Scan6(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const model::Object&& object, const std::string&& metaVar) const {

	//TODO: Check variable types
	//TODO : Linkify

	//get the entity handle
	Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
	std::vector<unsigned int> rowsAdded;

	if (EntityExists(entityRef)) {
		auto entity = _entities.at(entityRef);
		//iterate through properties
		for (auto propertyPair : entity->properties()) {
			auto vals = propertyPair.second->baseValues();
			for (auto value : vals) {
				if (value->Equals(object)) {
					rowsAdded.push_back(variableSet.add(std::move(variableName),
						VariableSetValue(std::make_shared<model::types::Int>(propertyPair.first, 0), 0, 0),
						model::types::SubType::PropertyReference));
				}
			}
		}
	}

	return rowsAdded;
}

void EntityManager::Scan7(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2, const std::string&& metaVar) const {
	
	Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

	if (EntityExists(entityRef)) {
		auto entity = _entities.at(entityRef);

		//if variableName is already set
		std::map<unsigned int, std::shared_ptr<EntityProperty>> iterableProperties;
		if (variableSet.used(variableName)) {
			auto data = variableSet.getData(variableName);
			std::transform(data.begin(), data.end(), std::inserter(iterableProperties, iterableProperties.begin()), [&](VariableSetValue b) {
				std::shared_ptr<model::types::Int> prop = std::dynamic_pointer_cast<model::types::Int, model::types::Base>(b.dataPointer());
				return std::pair<unsigned int, std::shared_ptr<EntityProperty>>(prop->value(), entity->getProperty(prop->value()));
			});
		}
		else {
			iterableProperties = entity->properties();
		}

		//iterate through properties
		for (auto propertyPair : iterableProperties) {
			auto vals = propertyPair.second->baseValues();
			for (auto value : vals) {

				//if variablename2 is already set
				if (variableSet.used(variableName2)) {
					auto rows = variableSet.find(variableName2, value->toString());
					for (auto rowId : rows) {
						variableSet.add(std::move(variableName),
							VariableSetValue(std::make_shared<model::types::Int>(propertyPair.first, 0), 0, 0),
							std::move(_propertyTypes.at(propertyPair.first)), rowId);
					}
				}
				else {
					auto rowId = variableSet.add(std::move(variableName), 
						VariableSetValue(std::make_shared<model::types::Int>(propertyPair.first, 0), 0, 0),
						model::types::SubType::PropertyReference);

					variableSet.add(std::move(variableName2),
						VariableSetValue(value->Clone(), 0, 0),
						std::move(_propertyTypes.at(propertyPair.first)), rowId);
				}				
			}
		}
	}

}

#pragma endregion scan_functions

//TODO: Add more type checking
unsigned int EntityManager::getPropertyName(const std::string &str, model::types::SubType type, bool addIfMissing)
{
	if ( addIfMissing && _propertyNames.left.find(str) == _propertyNames.left.end() )
	{
			std::cout << "Inserting property " << str << " with type " << (int)type << std::endl;
			_propertyNames.insert(boost::bimap<std::string, unsigned int>::value_type(str, ++_lastProperty));
			_propertyTypes[_lastProperty] = type;
	}

	return getPropertyName(str, type);
}

unsigned int EntityManager::getPropertyName(const std::string &str, model::types::SubType type) const
{
	auto iter = _propertyNames.left.find(str);
	if (iter == _propertyNames.left.end()) {
			return 0;
	}

	model::types::SubType retrievedType = _propertyTypes.at(iter->second);
	if (type != model::types::SubType::TypeUndefined && retrievedType != type) {
		throw MismatchedTypeException(std::string("Mismatched types when obtaining index for property '" + str
			+ "'. Requested type '" + model::types::getSubTypeString(type) + "' but got '"
			+ model::types::getSubTypeString(retrievedType) + "'.").c_str());
	}

	return iter->second;
}

const boost::bimap<std::string, unsigned int>& EntityManager::propertyNameMap() const
{
	return _propertyNames;
}