#include "./PropertyOwner.h"

PropertyOwner::~PropertyOwner() {}

// Getters:

std::shared_ptr<IEntityProperty> PropertyOwner::getProperty(const unsigned int &key) const {
	auto it = _propertyTable.find(key);
	return it->second;
}

// Setters:


void PropertyOwner::insertProperty(std::shared_ptr<IEntityProperty> prop) {
	// Erase the property if it exists (If not, this will do nothing).
	//propertyTable_.erase(prop.key());

	// Insert the new one.
	auto pair = std::make_pair<unsigned int, std::shared_ptr<IEntityProperty>>(std::move(prop->key()), std::move(prop));
	_propertyTable.insert(pair);
}

// Removes the property with the given key.
void PropertyOwner::removeProperty(const unsigned int &key) {
	try
	{
		//delete _propertyTable.at(key);
		_propertyTable.erase(key);
	}
	catch (const std::out_of_range &ex)
	{

	}
}

// Tests if the entity has a property
bool PropertyOwner::hasProperty(const unsigned int &key) {
	return _propertyTable.find(key) != _propertyTable.cend();
}

// Returns read only reference to the property table
const std::map<unsigned int, std::shared_ptr<IEntityProperty>>& PropertyOwner::properties() const {
	return _propertyTable;
}

// Tests if the entity meets the condition
std::vector<BasePointer> PropertyOwner::meetsCondition(unsigned int propertyId, const model::Object&& obj) {
	if (!hasProperty(propertyId)) return std::vector<BasePointer>();
	std::vector<BasePointer> values = getProperty(propertyId)->baseValues();
	values.erase(std::remove_if(values.begin(), values.end(), [obj](BasePointer ptr) { return !ptr->Equals(obj); }), values.end());
	return values;
}

// Clears all properties on the entity.
void PropertyOwner::clearProperties() {
	_propertyTable.clear();
}

// Returns the number of properties present.
int PropertyOwner::propertyCount() const {
	return _propertyTable.size();
}
