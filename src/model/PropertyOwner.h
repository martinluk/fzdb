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
	std::shared_ptr<EntityProperty> getProperty(const unsigned int &key) const;

	void insertProperty(std::shared_ptr<EntityProperty> prop);

	void insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object);

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

	void lock() { _locked = true; }

protected:

	std::map<unsigned int, std::shared_ptr<EntityProperty>> _propertyTable;
	bool _locked;

	inline void checkLock() {
		if (_locked) {
			throw std::runtime_error("This item is read only");
		}
	}
};

#endif	// MODEL_PROPERTY_OWNER_H