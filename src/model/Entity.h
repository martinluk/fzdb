#ifndef MODEL_ENTITY_H
#define MODEL_ENTITY_H

#include <map>
#include <vector>

#include "./Triple.h"
#include "./EntityProperty.h"
#include "ILogString.h"

// Represents an entity in the graph database.
// Each entity has a handle, which is a unique identifier.
// An entity's handle cannot be changed once it is instanciated.
// An entity owns all of its EntityProperties and will delete them as appropriate.
class Entity : public ILogString
{
	friend class EntitySerialiser;
public:
	// Typedef for an entity handle. This should uniquely identify an entity.
	typedef unsigned long long EHandle_t;

	// Invalid entity handle. This implies that the entity is null.
	static const EHandle_t INVALID_EHANDLE;

	// Constructs a null entity.
	Entity(unsigned int type);

	// Constructs an entity with the given handle.
	Entity(unsigned int type, EHandle_t handle);

	~Entity();

	// Returns whether this entity is null, ie. whether it has an invalid handle.
	bool isNull() const;

	// Getters:

	// Returns the property with the given key, or a null property if this is not found.
	template<typename T>
	EntityProperty<T>* getProperty(const unsigned int &key) const {
		auto it = _propertyTable.find(key);
		if (it == _propertyTable.cend()) {
			return new EntityProperty<T>();
		}

		// TODO: Add error messages
		try {
			EntityProperty<T>* prop = dynamic_cast<EntityProperty<T>*>(it->second);
			return prop;
		}
		catch (std::bad_cast ex) {
			return new EntityProperty<T>();
		}
		return new EntityProperty<T>();
	}

	IEntityProperty* getProperty(const unsigned int &key) const {
		auto it = _propertyTable.find(key);
		return it->second;
	}

	// Returns this entity's handle.
	EHandle_t getHandle() const;

	unsigned int getType() const;

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

	void insertProperty(IEntityProperty* prop) {
		// Erase the property if it exists (If not, this will do nothing).
		//propertyTable_.erase(prop.key());

		// Insert the new one.
		auto pair = std::make_pair<unsigned int, IEntityProperty*>(std::move(prop->key()), std::move(prop));
		_propertyTable.insert(pair);
	}

	// Removes the property with the given key.
	void removeProperty(const unsigned int &key);

	// Tests if the entity has a property
	bool hasProperty(const unsigned int &key);

	// Returns read only reference to the property table
	const std::map<unsigned int, IEntityProperty*>& properties() const {
		return _propertyTable;
	}

	// Tests if the entity meets the condition
	bool meetsCondition(unsigned int propertyId, const model::Object&& obj) {
		if (!hasProperty(propertyId)) return false;
		auto val = getProperty(propertyId)->baseValue(0);
		return val->Equals(obj.value);

		/*switch (obj.type) {
		case model::Object::Type::STRING: {
			auto val = getProperty<model::types::String>(propertyId)->values();
			return val[0]->Equals(obj.value);
		}
		case model::Object::Type::INT: {
			auto val = getProperty<model::types::Int>(propertyId)->values();
			return val[0]->Equals(obj.value);
		}
		case model::Object::Type::ENTITYREF: {
			auto val = getProperty<model::types::EntityRef>(propertyId)->values();
			return val[0]->Equals(obj.value);
		}
		default:
			return false;
		}*/
	}

	// Clears all properties on the entity.
	void clearProperties();

	// Returns the number of properties present.
	int propertyCount() const;

	virtual std::string logString() const override;

private:
	void deleteAllProperties();

	EHandle_t	handle_;
	unsigned int _type;
	bool _active;

	std::map<unsigned int, IEntityProperty*> _propertyTable;
};

#endif	// MODEL_ENTITY_H
