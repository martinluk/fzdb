#ifndef MODEL_ENTITY_MANAGER_H
#define MODEL_ENTITY_MANAGER_H

#include <map>
#include <functional>
#include <algorithm> 

#include "./Entity.h"

#include "../QueryResult.h"
#include "./Triple.h"
#include "./Parser.h"
#include "../VariableSet.h"
#include "../Exceptions.h"

// TODO: We need smart pointers! We need to have the manager own the entity
// and entities should not have delete called on them externally.
// We also need mutexes!
class EntityManager
{
    friend class GraphSerialiser;
public:
	EntityManager();
	~EntityManager();

	// Creates an entity on the heap and returns a pointer to it.
	Entity* createEntity();

	void AddProperty(std::string name, unsigned int val) {
		_propertyNames[name] = val;
	}	

	VariableSet BGP(TriplesBlock triplesBlock);

	void Insert(std::vector<model::Triple> triples);

	bool EntityExists(Entity::EHandle_t handle) {
		return _entities.find(handle) != _entities.cend();
	}
    
    std::vector<Entity*> entityList() const;
    std::size_t entityCount() const;
    
    void clearAll();
    
    std::string dumpContents() const;

    bool saveToFile(const std::string &filename);
    bool loadFromFile(const std::string &filename);

private:	

	// TODO: This could be an unordered map, but we may want to utilise the
	// numerical nature of the entity handles. O(log n) is still pretty good.
	typedef std::map<Entity::EHandle_t, Entity*> EntityMap;

	EntityMap _entities;
	Entity::EHandle_t _lastHandle;
	unsigned int _lastProperty;

	std::map<std::string, unsigned int> _entityTypeNames;
	std::map<std::string, unsigned int> _propertyNames;
	std::map<unsigned int, model::types::Base::Subtype> _propertyTypes;
    
    void insertEntity(Entity* ent);

	//TODO: Add more type checking
	unsigned int getPropertyName(std::string str, model::types::Base::Subtype type, bool addIfMissing) {
		auto iter = _propertyNames.find(str);
		if (iter == _propertyNames.cend()) {
			if (addIfMissing) {
				_propertyNames[str] = ++_lastProperty;
				_propertyTypes[_lastProperty] = type;
				return _lastProperty;
			}
			else {
				return 0;
			}
		}

		if (_propertyTypes[iter->second] != type) {
			throw MismatchedTypeException("mismatched types!");
		}

		return iter->second;
	}

	unsigned int getPropertyName(std::string str) {
		auto iter = _propertyNames.find(str);
		if (iter == _propertyNames.cend()) {
			return 0;
		}
		return iter->second;
	}

	// Basic Graph Processing - returns a list of the variables in conditions
	QueryResult SeparateTriples(std::vector<model::Triple> conditions);

	template<typename T>
	void addToEntity(Entity* currentEntity, unsigned int propertyId, model::Object&& object) {
		unsigned char confidence = object.hasCertainty ? object.certainty : 100;

		if (currentEntity->hasProperty(propertyId)) {
			currentEntity->getProperty<T>(propertyId)->append(new T(object.value, confidence));
		}
		else {
			currentEntity->insertProperty<T>(new EntityProperty<T>(propertyId, std::vector <T*> {
				new T(object.value, confidence)
			}));
		}
	}

	void Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object) {

		//get the property id
		unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString, false);

		//the variable has been used before, we only need to iterate over valid values from before
		if (variableSet.used(variableName)) {
			if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

				unsigned char varIndex = variableSet.indexOf(variableName);

				variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(), [&, this, varIndex](std::vector<std::string> row) {
					Entity* currentEntity = _entities[std::stoll(row[varIndex])];
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

			Entity* currentEntity = iter->second;
			if (currentEntity->meetsCondition(propertyId, std::move(object))) {
				variableSet.add(std::move(variableName), std::to_string(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
			}

		}
	}

	void Scan2(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2) {

		//get the property id
		const unsigned int propertyId = this->getPropertyName(predicate.value);

		//TODO: consider the case where variableName2 is already in variableSet

		//the variable has been used before, we only need to iterate over valid values from before
		if (variableSet.used(variableName)) {
			if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

				unsigned char varIndex = variableSet.indexOf(variableName),
					varIndex2 = variableSet.indexOf(variableName2);

				variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(), [&, this, varIndex](std::vector<std::string> row) {
					Entity* currentEntity = _entities[std::stoll(row[varIndex])];
					return !currentEntity->hasProperty(propertyId);
				}), variableSet.getData()->end());

				//TODO: but what about the type of the new data being added? :/
				for (auto iter = variableSet.getData()->begin(); iter != variableSet.getData()->end(); iter++) {
					Entity* currentEntity = _entities[std::stoll((*iter)[varIndex])];
					(*iter)[varIndex2] = currentEntity->getProperty(propertyId)->baseValues()[0]->toString();
				}				
			}
			else {
				//TODO: TypeException
			}
			return;
		}

		//the variable has not been used, add all the values found!
		for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

			Entity* currentEntity = iter->second;
			if (currentEntity->hasProperty(propertyId)) {
				auto rowId = variableSet.add(std::move(variableName), std::to_string(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
				variableSet.add(std::move(variableName2), 
					currentEntity->getProperty(propertyId)->baseValues()[0]->toString(),
					std::move(_propertyTypes[propertyId]), rowId);
			}

		}
	}

	void Scan4(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3) {

		//TODO: Check variable types

		//TODO: Cover cases where the variables have been used before :/
		if (variableSet.contains(variableName) || variableSet.contains(variableName2) || variableSet.contains(variableName3)) {
			return;
		}
		
		//If no variables have been used so far, get EVERYTHING!

		for (auto entity : _entities) {
			for (auto prop : entity.second->properties()) {

				variableSet.add(std::move(variableName), std::to_string(entity.first), model::types::Base::Subtype::TypeEntityRef);
				variableSet.add(std::move(variableName2), std::to_string(prop.first), model::types::Base::Subtype::PropertyReference);

				auto type = _propertyTypes[prop.first];
				std::string val = entity.second->getProperty(prop.first)->baseValues()[0]->toString();
				variableSet.add(std::move(variableName3), std::move(val), std::move(type));
			}
		}
	}

	void Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName) {

		//TODO: Check variable types

		//get the entity handle
		Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

		//get the property id
		const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString, false);

		if (EntityExists(entityRef)) {
			Entity* entity = _entities[entityRef];
			if (entity->hasProperty(propertyId)) {
				variableSet.add(std::move(variableName),
					entity->getProperty(propertyId)->baseValues()[0]->toString(),
					std::move(_propertyTypes[propertyId]));
			}
		}		
	}
};

#endif	// MODEL_ENTITY_MANAGER_H
