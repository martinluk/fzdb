#ifndef MODEL_ENTITY_MANAGER_H
#define MODEL_ENTITY_MANAGER_H

#include <map>
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

	// Basic Graph Processing - returns a list of the variables in conditions
	QueryResult BGP(std::vector<model::Triple> conditions);

	VariableSet BGP2(std::vector <model::Triple> conditions);

private:	

	// TODO: This could be an unordered map, but we may want to utilise the
	// numerical nature of the entity handles. O(log n) is still pretty good.
	typedef std::map<Entity::EHandle_t, Entity*> EntityMap;

	EntityMap entities_;
	Entity::EHandle_t lastHandle_;

	std::map<std::string, unsigned int> _entityTypeNames;
	std::map<std::string, unsigned int> _propertyNames;
};

#endif	// MODEL_ENTITY_MANAGER_H