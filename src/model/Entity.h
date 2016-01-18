#ifndef MODEL_ENTITY_H
#define MODEL_ENTITY_H

#include <map>
#include <vector>

#include "./Triple.h"

#include "./EntityProperty.h"

// Represents an entity in the graph database.
// Each entity has a handle, which is a unique identifier.
// An entity's handle cannot be changed once it is instanciated.
class Entity
{
public:
	// Typedef for an entity handle. This should uniquely identify an entity.
	typedef unsigned long long EHandle_t;

	// Invalid entity handle. This implies that the entity is null.
	static const EHandle_t INVALID_EHANDLE;

	// Constructs a null entity.
	Entity(unsigned int type);

	// Constructs an entity with the given handle.
	Entity(unsigned int type, EHandle_t handle);

	// Returns whether this entity is null, ie. whether it has an invalid handle.
	bool isNull() const;

	// Getters:

	// Returns the property with the given key, or a null property if this is not found.
	// TODO: This may be slow unless/until properties and their related classes are made
	// to be implicitly shared, as a copy of the property has to be made.
	template<typename T>
	EntityProperty<T>* getProperty(const unsigned int &key) const {
		auto it = _propertyTable.find(key);
		if (it == _propertyTable.cend()) {
			return new EntityProperty<T>();
		}
		
		////TODO: Add error messages
		try {
			EntityProperty<T>* prop = dynamic_cast<EntityProperty<T>*>(it->second);
			return prop;
		}
		catch (std::bad_cast ex) {
			return new EntityProperty<T>();
		}
		return new EntityProperty<T>();
	}

	// Returns this entity's handle.
	EHandle_t getHandle() const;

	// Setters:

	// Inserts the given property into this entity.
	// If a property with this key already exists, it is replaced.
	template<typename T>
	void insertProperty(EntityProperty<T>* prop) {
		// Erase the property if it exists (If not, this will do nothing).
		//propertyTable_.erase(prop.key());

		// Insert the new one.
		auto pair = std::make_pair<unsigned int, IEntityProperty*>(std::move(prop->key()), (IEntityProperty*)prop);
		_propertyTable.insert(pair);
	}

	// Removes the property with the given key.
	void removeProperty(const unsigned int &key);

	// Tests if the entity has a property
	bool hasProperty(const unsigned int &key);

	// Tests if the entity meets the condition
	bool meetsCondition(unsigned int propertyId, const model::Object&& obj) {
		if (!hasProperty(propertyId)) return false;

		switch (obj.type) {
			case model::Object::Type::STRING: {
				auto val = getProperty<model::types::String>(propertyId)->values();
				return val[0].Equals(obj.value);
			}
			case model::Object::Type::INT: {
				auto val = getProperty<model::types::Int>(propertyId)->values();
				return val[0].Equals(obj.value);
			}
			case model::Object::Type::ENTITYREF: {
				auto val = getProperty<model::types::EntityRef>(propertyId)->values();
				return val[0].Equals(obj.value);
			}
			default:
				return false;
		}		
	}

	// Clears all properties on the entity.
	void clearProperties();

	// Returns the number of properties present.
	int propertyCount() const;

	// Permenantly copies all data from entity into this entity
	void merge(Entity& entity);

  // Adds a link to entity 'entity' such that data from entity is returned
  // in queries to this entity
	void link(Entity* entity);

	// Removes the link to entity 'entity'
	void unlink(Entity* entity);

private:
	// Here (if C++ will let us), we use a const string reference as the key.
	// This avoids us from storing duplicate strings.

	const EHandle_t	handle_;
	const unsigned int _type;

	std::map<unsigned int, IEntityProperty*> _propertyTable;
	std::vector<Entity*> _linkedEntities;
};

#endif	// MODEL_ENTITY_H
