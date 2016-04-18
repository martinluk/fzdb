#ifndef MODEL_ENTITY_MANAGER_H
#define MODEL_ENTITY_MANAGER_H

#include <map>
#include <functional>
#include <algorithm>
#include <set>

#include <boost/bimap.hpp>

#include "./entity.h"

#include "../query_result.h"
#include "./triple.h"
#include "./parser.h"
#include "../variable_set.h"
#include "../exceptions.h"
#include "../util.h"
#include "./query_settings.h"
#include "./util/name_manager.h"

// Management class that keeps track of entities and their associated metadata.
class EntityManager
{
	friend class GraphSerialiser;
	friend class Entity;
public:
	EntityManager(Database* db);
	~EntityManager();

	// Creates an entity on the heap and returns a pointer to it.
	std::shared_ptr<Entity> createEntity(const std::string &type);

	VariableSet BGP(TriplesBlock triplesBlock, const QuerySettings settings) const;

    std::tuple<int,int,int> Delete(TriplesBlock&& whereBlock, QuerySettings&& settings);

    std::map<std::string, Entity::EHandle_t> Insert(TriplesBlock&& triples, TriplesBlock&& whereBlock, QuerySettings&& settings);

	bool EntityExists(Entity::EHandle_t handle) const {
		return _entities.find(handle) != _entities.cend();
	}

	std::shared_ptr<model::types::Base> dereference(Entity::EHandle_t entity, unsigned int prop, unsigned int val) const;

	std::vector<std::shared_ptr<Entity>> entityList() const;
	std::shared_ptr<Entity> getEntity(Entity::EHandle_t entity) const;
	std::size_t entityCount() const;

	void clearAll();

	std::string dumpContents() const;

	bool saveToFile(const std::string &filename);
	bool loadFromFile(const std::string &filename);

	void linkEntities(const Entity::EHandle_t entityId, const Entity::EHandle_t entityId2);
	void unlinkEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2);
	void mergeEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2);

	void createHierarchy(Entity::EHandle_t superset, Entity::EHandle_t subset, unsigned int author, const std::string &comment);
	void removeHierarchy(Entity::EHandle_t superset, Entity::EHandle_t subset);

	//move to private
	std::set<Entity::EHandle_t> getLinkGraph(const Entity::EHandle_t start, std::set<Entity::EHandle_t>&& visited) const;

	const std::string getPropertyName(unsigned int propertyId) const;

	const NameManager& propertyNameMap() const;

	std::string getTypeName(const unsigned int id) const {
		return _entityTypeNames.get(id);
	}

	bool memberwiseEqual(const EntityManager &other) const;

private:
	// TODO: (Jonathan) This could be an unordered map, but we may want to utilise the
	// numerical nature of the entity handles. O(log n) is still pretty good.
	typedef std::map<Entity::EHandle_t, std::shared_ptr<Entity>> EntityMap;
	Database* _database;

	EntityMap _entities;
	std::map<Entity::EHandle_t, std::set<Entity::EHandle_t>> _links;
	Entity::EHandle_t _lastHandle;
	unsigned int _lastProperty;
	unsigned int _lastTypeID;

	// This maps string type names to entity type IDs.
	NameManager _propertyNames;
	NameManager _entityTypeNames;

	std::map<unsigned int, model::types::SubType> _propertyTypes;

	void changeEntityType(Entity* ent, const std::string &type);
	unsigned int getTypeID(const std::string &str);
	unsigned int getTypeID(const std::string &str) const;
	static void enforceTypeHasBeenSet(const Entity* entity);
	static void enforceTypeHasBeenSet(const std::set<const Entity*> &ents);

	void createHierarchy(const model::Triple &triple, Entity* ent, unsigned int author, const std::string &comment,
		const std::vector<std::shared_ptr<model::types::Base> > &newRecords, model::types::SubType newRecordType);

	void removeHierarchy(const model::Triple &triple);
	bool performSpecialInsertOperations(const model::Triple &triple, Entity* ent, const std::vector<std::shared_ptr<model::types::Base> > &newRecords,
		model::types::SubType newRecordType, unsigned int author, const std::string &comment);

	void insertEntity(std::shared_ptr<Entity> ent);

	// Pass Undefined to skip type checking.
	unsigned int getPropertyName(const std::string &str, model::types::SubType type) const;

	unsigned int getPropertyId(const std::string str) const {
		return _propertyNames.get(str);
	}

	// Basic Graph Processing - returns a list of the variables in conditions

	void Scan1(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

	void Scan2(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;

	void Scan3(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;

	void Scan4(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;

	void Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar) const;

	void Scan6(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar) const;

	void Scan7(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar) const;

	///// SUB-SCAN FUNCTIONS : V = Variable, U = Used-Variable, M = Meta-Variable, E = Entity, P = Property, R = Record

	// $a $b $c
	void ScanVVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanVVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanVUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanVUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanUVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanUVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanUUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanUUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;

	void ScanMVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanMVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanMUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;
	void ScanMUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar) const;

	// $a $b Record
	void ScanVVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanVUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanUVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanUUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;

	void ScanMVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanMUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar) const;

	// $a Property $c
	void ScanVPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanVPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanUPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanUPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;

	void ScanMPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanMPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar) const;

	// $a Property Value
	void ScanVPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;
	void ScanUPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

	void ScanMPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

	// Entity $b $c
	void ScanEVV(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanEVU(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanEUV(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar) const;
	void ScanEUU(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar) const;

	// Entity $b Value
	void ScanEVR(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar) const;
	void ScanEUR(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar) const;

	// Entity Property $c
	void ScanEPV(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar) const;
	void ScanEPU(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar) const;

	// Entity Property Value
	void ScanEPR(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

	// Scan Helpers
	std::vector<unsigned int> ScanHelp1(VariableSet&& variableSet, const std::shared_ptr<Entity>&& entity,
		const std::map<unsigned int, std::shared_ptr<EntityProperty>>&& properties, unsigned int variableId,
		const model::Object&& object, const std::string&& metaVar) const;

	std::vector<unsigned int> ScanHelp2(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar) const;
};

#endif    // MODEL_ENTITY_MANAGER_H
