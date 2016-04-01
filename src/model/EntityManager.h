#ifndef MODEL_ENTITY_MANAGER_H
#define MODEL_ENTITY_MANAGER_H

#include <map>
#include <functional>
#include <algorithm> 
#include <set>

#include <boost/bimap.hpp>

#include "./Entity.h"

#include "../QueryResult.h"
#include "./Triple.h"
#include "./Parser.h"
#include "../VariableSet.h"
#include "../Exceptions.h"
#include "../Util.h"
#include "./QuerySettings.h"

// Management class that keeps track of entities and their associated metadata.
class EntityManager
{
    friend class GraphSerialiser;
    friend class Entity;
public:
    EntityManager();
    ~EntityManager();

    // Creates an entity on the heap and returns a pointer to it.
    std::shared_ptr<Entity> createEntity(const std::string &type);

    void AddProperty(std::string name, unsigned int val) {
        _propertyNames.insert(boost::bimap<std::string, unsigned int>::value_type(name, val));
    }    


    VariableSet BGP(TriplesBlock triplesBlock, const QuerySettings settings) const;

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

    const boost::bimap<std::string, unsigned int>& propertyNameMap() const;

	unsigned int getTypeID(const std::string &str);
	std::string getTypeName(const unsigned int id) const {
		return _entityTypeNames.right.at(id);
	}
private:
    void changeEntityType(Entity::EHandle_t id, const std::string &type);   
   

    // TODO: (Jonathan) This could be an unordered map, but we may want to utilise the
    // numerical nature of the entity handles. O(log n) is still pretty good.
    typedef std::map<Entity::EHandle_t, std::shared_ptr<Entity>> EntityMap;

    EntityMap _entities;
    std::map<Entity::EHandle_t, std::set<Entity::EHandle_t>> _links;
    Entity::EHandle_t _lastHandle;
    unsigned int _lastProperty;
    unsigned int _lastTypeID;

    // This maps string type names to entity type IDs.
	boost::bimap<std::string, unsigned int> _entityTypeNames;

    boost::bimap<std::string, unsigned int> _propertyNames;
    std::map<unsigned int, model::types::SubType> _propertyTypes;
    
    bool handleSpecialInsertOperations(Entity* entity, const model::Triple &triple, unsigned int author, const std::string &comment);
    static void enforceTypeHasBeenSet(const Entity* entity);
    static void enforceTypeHasBeenSet(const std::set<const Entity*> &ents);
    
    void insertEntity(std::shared_ptr<Entity> ent);

    // Pass TypeUndefined to skip type checking.
    unsigned int getPropertyName(const std::string &str, model::types::SubType type) const;
    unsigned int getPropertyName(const std::string &str, model::types::SubType type, bool addIfMissing);

    unsigned int getPropertyId(std::string str) const {
        auto iter = _propertyNames.left.find(str);
        if (iter == _propertyNames.left.end()) {
            return 0;
        }
        return iter->second;
    }

    // Basic Graph Processing - returns a list of the variables in conditions

    void Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

    void Scan2(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;

    void Scan3(VariableSet&& variableSet, const std::string variableName, const std::string variableName2,    const model::Object&& object, const std::string&& metaVar) const;

    void Scan4(VariableSet&& variableSet, const std::string variableName, const std::string variableName2,    const std::string variableName3, const std::string&& metaVar) const;

    std::vector<unsigned int> Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName, const std::string&& metaVar) const;

    std::vector<unsigned int> Scan6(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName,     const model::Object&& object, const std::string&& metaVar) const;

    void Scan7(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName,  const std::string variableName2, const std::string&& metaVar) const;

	///// SUB-SCAN FUNCTIONS : V = Variable, U = Used-Variable, M = Meta-Variable, E = Entity, P = Property, R = Record

	// $a $b $c
	void ScanVVV(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanVVU(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanVUV(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanVUU(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanUVV(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanUVU(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanUUV(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanUUU(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;

	
	void ScanMVV(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanMVU(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanMUV(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	void ScanMUU(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const;
	
	// $a $b Record
	void ScanVVR(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanVUR(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanUVR(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanUUR(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const;

	void ScanMVR(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const;
	void ScanMUR(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const;

	// $a Property $c
	void ScanVPV(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;
	void ScanVPU(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;
	void ScanUPV(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;
	void ScanUPU(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;

	void ScanMPV(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;
	void ScanMPU(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const;

	// $a Property Value
	void ScanVPR(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;
	void ScanUPR(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;
	
	void ScanMPR(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

	// Entity $b $c
	void ScanEVV(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2, const std::string&& metaVar) const;
	void ScanEVU(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2, const std::string&& metaVar) const;
	void ScanEUV(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2, const std::string&& metaVar) const;
	void ScanEUU(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2, const std::string&& metaVar) const;

	// Entity $b Value
	void ScanEVR(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const model::Object&& object, const std::string&& metaVar) const;
	void ScanEUR(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const model::Object&& object, const std::string&& metaVar) const;

	// Entity Property $c
	void ScanEPV(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName, const std::string&& metaVar) const;
	void ScanEPU(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName, const std::string&& metaVar) const;

	// Entity Property Value
	void ScanEPR(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const;

};

#endif    // MODEL_ENTITY_MANAGER_H
