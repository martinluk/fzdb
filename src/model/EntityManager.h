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

	std::map<std::string, unsigned int> _entityTypeNames;
	std::map<std::string, unsigned int> _propertyNames;

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
};

#endif	// MODEL_ENTITY_MANAGER_H