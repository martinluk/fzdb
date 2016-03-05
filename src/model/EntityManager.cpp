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

QueryResult EntityManager::SeparateTriples(std::vector<model::Triple> conditions)
{
	std::map<std::string, std::vector<std::string>> variables;

	for (int i = 0; i < conditions.size(); i++) {
		auto vars = conditions[i].variables();

		switch (vars.size()) {
		case 0: 
			//if the triple doesn't have any variables, then it doesn't affect the output
			break;
		case 1:
			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			break;
		case 2:

			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[1]);
			}

			if (variables.find(vars[1]) == variables.end()) {
				variables[vars[1]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
			}

			break;
		case 3:

			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[1]);
				variables[vars[0]].push_back(vars[2]);
			}

			if (variables.find(vars[1]) == variables.end()) {
				variables[vars[1]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
				variables[vars[0]].push_back(vars[2]);
			}

	 		if (variables.find(vars[2]) == variables.end()) {
				variables[vars[2]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
				variables[vars[0]].push_back(vars[1]);
			}

			break;
		}
	}

	std::vector<std::set<std::string>> buckets;
	std::set<std::string> visited;

	for (auto iter = variables.begin(); iter != variables.end(); iter++) {

	}


	return QueryResult();
}

// Basic Graph Pattern
// When presented with a sanatised list of triples finds values for variables that satisfy that condition
VariableSet EntityManager::BGP(TriplesBlock triplesBlock, const QuerySettings settings) const
{
	VariableSet result(triplesBlock.variables);
	auto conditions = triplesBlock.triples;

	//sort by entropy
	std::sort(conditions.begin(), conditions.end(), [](model::Triple t1, model::Triple t2) { return t1.Entropy() < t2.Entropy(); });

	//iterate over conditions
	for (auto conditionsIter = conditions.cbegin(); conditionsIter != conditions.end(); conditionsIter++) {

		//There are 7 valid combinations

		if (conditionsIter->subject.type == model::Subject::Type::VARIABLE) {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 1 - $a <prop> value
					this->Scan1(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), std::move(conditionsIter->object));
				}
				else {
					//option 2 - $a <prop> $b
					this->Scan2(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), conditionsIter->object.value);
				}
			}
			else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option3 - $a $b value
					this->Scan3(std::move(result), conditionsIter->subject.value, conditionsIter->predicate.value, std::move(conditionsIter->object));
				}
				else {
					//option 4 - $a $b $c
					this->Scan4(std::move(result), conditionsIter->subject.value, conditionsIter->predicate.value, conditionsIter->object.value);
				}
			}
		}
		else {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//doesn't contain any variables.. is meaningless
				}
				else {
					//option 5 - entity <prop> $c
					this->Scan5(std::move(result), std::move(conditionsIter->subject), std::move(conditionsIter->predicate), conditionsIter->object.value);
				}
			}
	 		else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 7 - entity $b value
					this->Scan6(std::move(result), std::move(conditionsIter->subject), conditionsIter->predicate.value, std::move(conditionsIter->object));
				}
				else {
					//option 8 - entity $b $c
					this->Scan7(std::move(result), std::move(conditionsIter->subject), conditionsIter->predicate.value, conditionsIter->object.value);
				}
			}
		}
	}

	return result;
}

//Inserts new data into the data store
void EntityManager::Insert(std::vector<model::Triple> triples) {
	auto iter = triples.cbegin();
	auto end = triples.cend();
	for (; iter != end; iter++) {
		auto triple = *iter;
		
		// If we are setting an entity type, do so here.
		if ( triple.predicate.value == ReservedProperties::TYPE )
		{
			changeEntityType(std::stoll(triple.subject.value), triple.object.value);
			continue;
		}

		auto entity_id = std::stoll(triple.subject.value);

		//create the entity if it doesn't exist
		if (_entities.find(entity_id) == _entities.end())
		{	
			_entities[entity_id] = std::make_shared<Entity>(ENTITY_TYPE_GENERIC, entity_id);
		}

		std::shared_ptr<Entity> currentEntity = _entities[entity_id];
		unsigned int propertyId;

		switch (triple.object.type) {
		case model::Object::Type::STRING:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeString, true);
			addToEntity<model::types::String>(currentEntity, propertyId, std::move(triple.object));
			break;
		case model::Object::Type::ENTITYREF:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeEntityRef, true);
			addToEntity<model::types::EntityRef>(currentEntity, propertyId, std::move(triple.object));
			break;
		case model::Object::Type::INT:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeInt32, true);
			addToEntity<model::types::Int>(currentEntity, propertyId, std::move(triple.object));
			break;
		}
	}
}

//XXX Probably wrong
void EntityManager::Delete(std::vector<model::Triple> triples) {
	/*
	 * Given a vector of triples, delete them from entity
	 */
	auto iter = triples.cbegin();
	auto end = triples.cend();
	//Iterating triples in the vector
	for (; iter != end; iter++) {
		auto triple = *iter;
		auto entity_id = std::stoll(triple.subject.value);
		auto tripleIter = _entities.find(entity_id);
		if (tripleIter == _entities.end()){
			//Skip if triple does not exist in entity
			continue;
		} else {
			//Remove entnty from _entities
			_entities.erase(entity_id);
		}
	}
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
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        const std::shared_ptr<Entity> ent = it->second;
        str << ent->logString() << "\n";
        if ( ent->propertyCount() > 0 )
        {
            str << "{\n";
            
			const std::map<unsigned int, std::shared_ptr<IEntityProperty>> &propMap = ent->properties();
            for ( auto it2 = propMap.cbegin(); it2 != propMap.cend(); ++it2 )
            {
				std::shared_ptr<IEntityProperty> prop = it2->second;
                str << "  " << prop->logString() << "\n";
                
                if ( prop->count() > 0 )
                {
                    str << "  {\n";
                    
                    for ( int i = 0; i < prop->count(); i++ )
                    {
                        BasePointer val = prop->baseValue(i);
                        str << "    " << val->logString() << "\n";
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
    gSer.unserialise(buffer);
    delete[] buffer;

    return true;
}

unsigned int EntityManager::getTypeID(const std::string &str)
{
	std::string uppercase = util::toUppercase(str);

	// "Generic" type is an empty string.
	// The ID for this is 0.
	if ( uppercase.size() < 1 )
		return ENTITY_TYPE_GENERIC;

	unsigned int id = 0;
	try
	{
		id = _entityTypeNames.at(uppercase);
	}
	catch (const std::exception&)
	{
		// The string was not in the table yet.
		// Assign it a new ID.
		_lastTypeID++;
		id = _lastTypeID;
		assert(id > 0);
		_entityTypeNames.insert(std::pair<std::string,unsigned int>(uppercase, id));
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
		throw new std::runtime_error("Attempted to merge entities of different types");
	}

	//copy the properties of loseEntity to keepEntity
	for (auto prop : loseEntity->properties()) {
		keepEntity->insertProperty(prop.second);
	}

	//delete EntityManager's ownership of loseEntity (will be automatically deleted)
	_entities.erase(lose);
}

#pragma endregion linkingandmerging

#pragma region scan_functions

void EntityManager::Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object) const {
	
	//get the property id
	unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString);

	//the variable has been used before, we only need to iterate over valid values from before
	if (variableSet.used(variableName)) {
		if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

			unsigned char varIndex = variableSet.indexOf(variableName);

			variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
				[&, this, varIndex](std::vector<std::shared_ptr<model::types::Base>> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex])->value();
					auto currentEntity = _entities.at(entityHandle);
					return !currentEntity->meetsCondition(propertyId, std::move(object));
			}), variableSet.getData()->end());

		}
		else {
			//TODO: TypeException
		}
		return;
	}

	//the variable has not been used, add all the values found!
	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto currentEntity = iter->second;
		switch (currentEntity->linkStatus()) {

		case Entity::LinkStatus::None:
			if (currentEntity->meetsCondition(propertyId, std::move(object))) {
				variableSet.add(std::move(variableName), std::make_shared<model::types::EntityRef>(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
			}
			break;
		case Entity::LinkStatus::Slave:
			continue;
		case Entity::LinkStatus::Master:
			auto graph = getLinkGraph(currentEntity->handle_, std::set<Entity::EHandle_t>());
			for (auto entId : graph) {
				if (_entities.at(entId)->meetsCondition(propertyId, std::move(object))) {
					variableSet.add(std::move(variableName), std::make_shared<model::types::EntityRef>(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
					break;
				}
			}
			
			break;
		}
	}
}

void EntityManager::Scan2(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2) const {
	
	//get the property id
	const unsigned int propertyId = this->getPropertyName(predicate.value);

	//TODO: consider the case where variableName2 is already in variableSet

	//the variable has been used before, we only need to iterate over valid values from before
	if (variableSet.used(variableName)) {
		if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

			unsigned char varIndex = variableSet.indexOf(variableName),
				varIndex2 = variableSet.indexOf(variableName2);

			variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
				[&, this, varIndex](std::vector<std::shared_ptr<model::types::Base>> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex])->value();
					auto currentEntityIter = _entities.find(entityHandle);
					std::shared_ptr<Entity> currentEntity;
					if (currentEntityIter != _entities.end()) {
						currentEntity = _entities.at(entityHandle);
					} else {
						throw new std::runtime_error("Attempted to lookup a non-existent entity");
					}					
					return !currentEntity->hasProperty(propertyId);
			}), variableSet.getData()->end());

			//TODO: but what about the type of the new data being added? :/
			for (auto iter = variableSet.getData()->begin(); iter != variableSet.getData()->end(); iter++) {
				Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[varIndex])->value();
				auto currentEntityIter = _entities.find(entityHandle);
				std::shared_ptr<Entity> currentEntity;
				if (currentEntityIter != _entities.end()) {
					currentEntity = _entities.at(entityHandle);
				}
				else {
					throw new std::runtime_error("Attempted to lookup a non-existent entity");
				}
				(*iter)[varIndex2] = currentEntity->getProperty(propertyId)->baseValue(0)->Clone();
			}
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
				std::to_string(iter->first)), std::move(predicate), variableName2);

		for (auto row : rows) {
			variableSet.add(std::move(variableName),
				std::make_shared<model::types::EntityRef>(iter->second->getHandle()),
				model::types::Base::Subtype::TypeEntityRef, row);
		}

	}
}

void EntityManager::Scan3(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object) const {

	for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

		auto rows = Scan6(std::move(variableSet),
			model::Subject(model::Subject::Type::ENTITYREF,
				std::to_string(iter->first)), variableName2, std::move(object));

		for (auto row : rows) {
			variableSet.add(std::move(variableName),
				std::make_shared<model::types::EntityRef>(iter->second->getHandle()),
				model::types::Base::Subtype::TypeEntityRef, row);
		}

	}
}

void EntityManager::Scan4(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3) const {
	
	//TODO: Check variable types

	//TODO: Cover cases where the variables have been used before :/
	if (variableSet.contains(variableName) || variableSet.contains(variableName2) || variableSet.contains(variableName3)) {
		return;
	}

	//If no variables have been used so far, get EVERYTHING!

	for (auto entity : _entities) {
		for (auto prop : entity.second->properties()) {

			variableSet.add(std::move(variableName), std::make_shared<model::types::EntityRef>(entity.first), model::types::Base::Subtype::TypeEntityRef);
			variableSet.add(std::move(variableName2), std::make_shared<model::types::Int>(prop.first), model::types::Base::Subtype::PropertyReference);

			auto type = _propertyTypes.at(prop.first);
			std::shared_ptr<model::types::Base> val = entity.second->getProperty(prop.first)->baseValue(0)->Clone();
			variableSet.add(std::move(variableName3), std::move(val), std::move(type));
		}
	}
}

std::vector<unsigned int> EntityManager::Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName) const {
	
	//TODO: Check variable types
	//TODO : Linkify

	//get the entity handle
	Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
	std::vector<unsigned int> rowsAdded;

	//get the property id
	const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString);

	if (EntityExists(entityRef)) {
		auto entity = _entities.at(entityRef);
		if (entity->hasProperty(propertyId)) {
			for (auto value : entity->getProperty(propertyId)->baseValues()) {
				rowsAdded.push_back(variableSet.add(std::move(variableName), value->Clone(), std::move(_propertyTypes.at(propertyId))));
			}			
		}
	}

	return rowsAdded;
}

std::vector<unsigned int> EntityManager::Scan6(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const model::Object&& object) const {

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
					rowsAdded.push_back(variableSet.add(std::move(variableName), std::make_shared<model::types::Int>(propertyPair.first), model::types::Base::Subtype::PropertyReference));
				}
			}
		}
	}

	return rowsAdded;
}

void EntityManager::Scan7(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2) const {
	
	Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

	if (EntityExists(entityRef)) {
		auto entity = _entities.at(entityRef);

		//if variableName is already set
		std::map<unsigned int, std::shared_ptr<IEntityProperty>> iterableProperties;
		if (variableSet.used(variableName)) {
			auto data = variableSet.getData(variableName);
			std::transform(data.begin(), data.end(), std::inserter(iterableProperties, iterableProperties.begin()), [&](BasePointer b) {
				std::shared_ptr<model::types::Int> prop = std::dynamic_pointer_cast<model::types::Int, model::types::Base>(b);
				return std::pair<unsigned int, std::shared_ptr<IEntityProperty>>(prop->value(), entity->getProperty(prop->value()));
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
						variableSet.add(std::move(variableName), std::make_shared<model::types::Int>(propertyPair.first), std::move(_propertyTypes.at(propertyPair.first)), rowId);
					}
				}
				else {
					auto rowId = variableSet.add(std::move(variableName), std::make_shared<model::types::Int>(propertyPair.first), model::types::Base::Subtype::PropertyReference);
					variableSet.add(std::move(variableName2), value->Clone(), std::move(_propertyTypes.at(propertyPair.first)), rowId);
				}				
			}
		}
	}

}

#pragma endregion scan_functions
