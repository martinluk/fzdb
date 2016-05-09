/**
 * EntityManager.h - defines the entity manager class
 */

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
#include <gtest/gtest_prod.h>

namespace jobs {
class DebugSaveFile;
}

/**
 * @brief Manages Entities
 * @details This class maintains a list of entities and contains the definitions of all
 * Create, Select and Delete operations possible in the database.
 */
class EntityManager {
  friend class GraphSerialiser;
  friend class Entity;
  friend class jobs::DebugSaveFile;
  FRIEND_TEST(SerialisationTest, testSerialiseEntityManager);
  FRIEND_TEST(SerialisationTest, testWriteReadFiles);
 public:
  /**
   * @brief Constructor for the entity manager
   *
   * @param db A pointer to the database object this entity manager is a part of
   */
  explicit EntityManager(Database* db);

  /**
   * @brief Destroys the entity manager
   */
  ~EntityManager();

  /**
   * @brief Performs basic graph processing on the database using the given triples
   * @details Selects all triples in the database that match the given pattern.
   *
   * @param triplesBlock The triples to evaluate
   * @param settings Additional settings for the query
   *
   * @return The triples selected from the database
   */
  VariableSet BGP(TriplesBlock triplesBlock, const QuerySettings&& settings) const;

  std::tuple<int,int,int> Delete(std::vector<std::string> selectLine, TriplesBlock&& whereBlock, const QuerySettings&& settings);

  /**
   * @brief Inserts the given triples into the datastore
   * @details Evaluates the whereblock and then iterates over the given triples inserting any
   * generated entities, properties or records into the database.
   *
   * @param triples Triples representing the new data to insert
   * @param whereBlock Triples representing queries to run before inserting the new triples
   * @param settings Additional settings for the query
   * @return A map from the variables that were supplied in the query to the ids given to the entities they generated
   * by the database.
   */
  std::map<std::string, Entity::EHandle_t> Insert(TriplesBlock&& triples, TriplesBlock&& whereBlock, const QuerySettings&& settings);

  /**
   * @brief Tests whether an entity exists
   * @details Tests whether the entity with the given handle exists in the
   * datbase
   *
   * @param handle The entity handle to test
   * @return Boolean representing whether the entity exists in the database or n
   */
  bool EntityExists(Entity::EHandle_t handle) const {
    return _entities.find(handle) != _entities.cend();
  }

  std::shared_ptr<model::types::Base> dereference(Entity::EHandle_t entity, unsigned int prop, unsigned int val) const;

  /**
   * @brief Gets a list of all entities
   * @details Transforms the map of entities into a vector and returns it
   * @return A vector of all entities in the database
   */
  std::vector<std::shared_ptr<Entity>> entityList() const;

  /**
   * @brief Gets a pointer to the entity with the given id
   * @details Gets a pointer to the entity with the given id. Throws an error if the entity
   * id does not exist
   *
   * @param entity The id of the entity to retrieve
   * @return A shared pointer to the entity
   */
  std::shared_ptr<Entity> getEntity(Entity::EHandle_t entity) const;

  /**
   * @brief Gets the number of entities
   * @details Retrieves the number of entities stored by this entity manager
   * @return The number of entities
   */
  std::size_t entityCount() const;

  /**
   * @brief Clears the database back to its default state
   * @details Deletes all data in the database and reseeds the built-in types. Writes to
   * the output file on completion.
   */
  void clearAll();

  std::string dumpContents() const;

  /**
   * @brief Saves the database to disk
   * @details Serialiases the database and writes it to the specified file
   *
   * @param filename The filename to write to, relative paths are from the exe location
   * @return Boolean indicating whether the save was succesful
   */
  bool saveToFile(const std::string &filename);

  /**
   * @brief Loads the database from disk
   * @details Reads the specified files and unserialises the database
   *
   * @param filename The file to read from, relative paths are from the exe location
   * @return Boolean indicating whether the load was succesful
   */
  bool loadFromFile(const std::string &filename);

  /**
   * @brief Links two entities
   * @details Links the entities with the given ids. The link graph that this creates is then
   * evaluated and link properties set on the affected entities. The entity with the lowest id
   * is assigned LinkMaster, the others LinkSlave.
   *
   * @param entityId The id of one of the entities to link
   * @param entityId2 The id of the other entity to link
   */
  void linkEntities(const Entity::EHandle_t entityId, const Entity::EHandle_t entityId2);

  /**
   * @brief Unlinks two entities
   * @details Unlinks the entities with the given ids. If either entity is still part of a link
   * graph those graphs are recalculated and link properties reassigned.
   *
   * @param entityId The id of one of the entities to unlink
   * @param entityId2 The id of the other entity to unlink
   */
  void unlinkEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2);

  /**
   * @brief Merges two entities
   * @details Copies all properties from the entity with the higher id to the entity with the lower
   * id and then deletes the entity with the higher id
   *
   * @param entityId The id of one of the entities to merge
   * @param entityId2 The id of the other entity to merge
   */
  void mergeEntities(Entity::EHandle_t entityId, Entity::EHandle_t entityId2);

  /**
   * @brief Gets the name of the property with the given id
   * @details Retrieves the name of the property with the given id. Throws an error
   * if the id is not present in the name registry.
   *
   * @param int The id of the property to find the name of
   * @return The name of the property with id `propertyId`
   */
  const std::string getPropertyName(unsigned int propertyId) const;

  /**
   * @brief Gets the property name manager
   * @details Returns a read-only reference to the property name manager
   * @return Read-only reference to the property name manager
   */
  const NameManager& propertyNameMap() const;

  /**
   * @brief Gets the name of the type with the given id
   * @details Retrieves the name of the entity type with the given id. Throws
   * an error if the id is not present in the name registry.
   *
   * @param int The id of the entity type to find the name of
   * @return The name of the entity type with id `id`
   */
  std::string getTypeName(const unsigned int id) const {
    return _entityTypeNames.get(id);
  }

  /**
   * @brief Tests if two EntityManagers are equal
   * @details Tests the equality of this entity manager with the given entity manager
   * by comparing all of the members.
   *
   * @param other The entity manager to compare this entity manager with
   * @return Whether the two managers are equal
   */
  bool memberwiseEqual(const EntityManager &other) const;

 private:

  std::shared_ptr<Entity> createEntity(const std::string &type);

  std::set<Entity::EHandle_t> getLinkGraph(const Entity::EHandle_t start, std::set<Entity::EHandle_t>&& visited) const;

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

  unsigned int getTypeID(const std::string &str);
  unsigned int getTypeID(const std::string &str) const;

  void insertEntity(std::shared_ptr<Entity> ent);

  // Pass Undefined to skip type checking.
  unsigned int getPropertyName(const std::string &str, model::types::SubType type) const;

  unsigned int getPropertyId(const std::string str) const {
    return _propertyNames.get(str);
  }

  // Basic Graph Processing - returns a list of the variables in conditions

  void Scan1(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  void Scan2(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;

  void Scan3(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  void Scan4(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;

  void Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const;

  void Scan6(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  void Scan7(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;

  ///// SUB-SCAN FUNCTIONS : V = Variable, U = Used-Variable, M = Meta-Variable, E = Entity, P = Property, R = Record

  // $a $b $c
  void ScanVVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanVVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanVUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanVUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;

  void ScanMVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanMVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanMUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanMUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const;

  // $a $b Record
  void ScanVVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanVUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  void ScanMVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanMUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  // $a Property $c
  void ScanVPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanVPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;

  void ScanMPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanMPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;

  // $a Property Value
  void ScanVPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanUPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  void ScanMPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  // Entity $b $c
  void ScanEVV(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanEVU(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanEUV(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanEUU(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const;

  // Entity $b Value
  void ScanEVR(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanEUR(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  // Entity Property $c
  void ScanEPV(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const;
  void ScanEPU(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const;

  // Entity Property Value
  void ScanEPR(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  // Scan Helpers
  std::vector<unsigned int> ScanHelp1(VariableSet&& variableSet, const std::shared_ptr<Entity>&& entity,
                                      const std::map<unsigned int, std::shared_ptr<EntityProperty>>&& properties, unsigned int variableId,
                                      const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const;

  std::vector<unsigned int> ScanHelp2(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const;
};

#endif    // MODEL_ENTITY_MANAGER_H
