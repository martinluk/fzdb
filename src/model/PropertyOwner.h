#ifndef MODEL_PROPERTY_OWNER_H
#define MODEL_PROPERTY_OWNER_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "./Triple.h"
#include "./EntityProperty.h"
#include "./types/SubType.h"

// Represents an entity in the graph database.
// Each entity has a handle, which is a unique identifier.
// An entity's handle cannot be changed once it is instanciated.
// An entity owns all of its EntityProperties and will delete them as appropriate.
class PropertyOwner
{
public:

	virtual ~PropertyOwner();

	// Getters:

	// Returns the property with the given key, or a null property if this is not found.
	std::shared_ptr<EntityProperty> getProperty(const unsigned int &key) const {
		auto it = _propertyTable.find(key);
		if (it == _propertyTable.cend()) {
			return std::shared_ptr<EntityProperty>();
		}

		// TODO: Add error messages
		try {
			std::shared_ptr<EntityProperty> prop = std::dynamic_pointer_cast<EntityProperty, EntityProperty>(it->second);
			return prop;
		}
		catch (std::bad_cast ex) {
			return std::shared_ptr<EntityProperty>();
		}
		return std::shared_ptr<EntityProperty>();
	}

	//std::shared_ptr<EntityProperty> getProperty(const unsigned int &key) const;

	// Setters:

	// Inserts the given property into this entity.

	void insertProperty(std::shared_ptr<EntityProperty> prop) {
		if (hasProperty(prop->key())) {
			auto existingProp = getProperty(prop->key());
			for (auto value : prop->baseValues()) {
				existingProp->append(value);
			}
		}
		else {
			auto pair = std::make_pair<unsigned int, std::shared_ptr<EntityProperty>>(std::move(prop->key()), std::move(prop));
			_propertyTable.insert(pair);
		}
	}

	void insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object) {
		// Erase the property if it exists (If not, this will do nothing).
		//propertyTable_.erase(prop.key());
		if (!hasProperty(key)) {
			auto pair = std::make_pair<unsigned int, std::shared_ptr<EntityProperty>>(std::move(key), std::make_shared<EntityProperty>(key));
			pair.second->append(object);
			_propertyTable.insert(pair);
		}
		else {
			auto prop = getProperty(key);
			prop->append(object);
		}
	}

	// Removes the property with the given key.
	void removeProperty(const unsigned int &key);

	// Tests if the entity has a property
	bool hasProperty(const unsigned int &key);

	// Returns read only reference to the property table
	const std::map<unsigned int, std::shared_ptr<EntityProperty>>& properties() const;

	// Tests if the entity meets the condition
	std::vector<std::shared_ptr<model::types::Base>> meetsCondition(unsigned int propertyId, const model::Object&& obj);

	// Clears all properties on the entity.
	void clearProperties();

	// Returns the number of properties present.
	int propertyCount() const;

protected:

	std::map<unsigned int, std::shared_ptr<EntityProperty>> _propertyTable;
};

#endif	// MODEL_PROPERTY_OWNER_H