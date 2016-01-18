#ifndef MODEL_ENTITY_MANAGER_H
#define MODEL_ENTITY_MANAGER_H

#include <map>
#include <functional>
#include "./Entity.h"

#include "../QueryResult.h"
#include "./Triple.h"
#include "../VariableSet.h"

// TODO: We need smart pointers! We need to have the manager own the entity
// and entities should not have delete called on them externally.
// We also need mutexes!
class EntityManager
{
public:
	EntityManager();
	~EntityManager();

	// Creates an entity on the heap and returns a pointer to it.
	Entity* createEntity();

	void AddProperty(std::string name, unsigned int val) {
		_propertyNames[name] = val;
	}	

	VariableSet BGP(std::vector <model::Triple> conditions);

	void Insert(std::vector<model::Triple> triples);

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
		return iter->second;
	}

	// Basic Graph Processing - returns a list of the variables in conditions
	QueryResult SeparateTriples(std::vector<model::Triple> conditions);

	template<typename T, typename S>
	void addToEntity(Entity* currentEntity, unsigned int propertyId, std::string str, std::function<S(std::string)> converter) {
		if (currentEntity->hasProperty(propertyId)) {
			currentEntity->getProperty<T>(propertyId)->append(T(80, converter(str)));
		}
		else {
			currentEntity->insertProperty<T>(new EntityProperty<T>(propertyId, std::vector <T> {
				T(80, converter(str))
			}));
		}
	}

	void Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object) {

		//get the property id
		unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString, false);

		//the variable has been used before, we only need to iterate over valid values from before
		if (variableSet.contains(variableName)) {
			if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

				auto entities = variableSet.getValuesFor(variableName);

				entities.erase(std::remove_if(entities.begin(), entities.end(), [&, this] (std::string val) {
					Entity* currentEntity = _entities[std::stoll(val)];
					return !currentEntity->meetsCondition(propertyId, std::move(object));
				}), entities.end());

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
		const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::Base::Subtype::TypeString, false);

		//TODO: consider the case where variableName2 is already in variableSet

		//the variable has been used before, we only need to iterate over valid values from before
		if (variableSet.contains(variableName)) {
			if (variableSet.typeOf(variableName) == model::types::Base::Subtype::TypeEntityRef) {

				auto entities = variableSet.getValuesFor(variableName);

				entities.erase(std::remove_if(entities.begin(), entities.end(), [&, this](std::string val) {
					Entity* currentEntity = _entities[std::stoll(val)];
					return !currentEntity->hasProperty(propertyId);
				}), entities.end());

				for (auto entity : entities) {
					Entity* currentEntity = _entities[std::stoll(entity)];					
					variableSet.add(std::move(variableName2), 
						currentEntity->getProperty<model::types::String>(propertyId)->values()[0].value(), 
						model::types::Base::Subtype::TypeString);
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
				variableSet.add(std::move(variableName), std::to_string(currentEntity->getHandle()), model::types::Base::Subtype::TypeEntityRef);
				variableSet.add(std::move(variableName2), 
					currentEntity->getProperty<model::types::String>(propertyId)->values()[0].value(), 
					model::types::Base::Subtype::TypeString);
			}

		}
	}
};

#endif	// MODEL_ENTITY_MANAGER_H