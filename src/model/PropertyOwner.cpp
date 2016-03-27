#include "./PropertyOwner.h"

#include "./types/Base.h"

PropertyOwner::~PropertyOwner() {}

// Getters:

//std::shared_ptr<EntityProperty> PropertyOwner::getProperty(const unsigned int &key) const {
//	auto it = _propertyTable.find(key);
//	return it->second;
//}

// Setters:

// Removes the property with the given key.
void PropertyOwner::removeProperty(const unsigned int &key) {

	checkLock();

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
const std::map<unsigned int, std::shared_ptr<EntityProperty>>& PropertyOwner::properties() const {
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
	checkLock();
	_propertyTable.clear();
}

// Returns the number of properties present.
int PropertyOwner::propertyCount() const {
	return _propertyTable.size();
}

std::shared_ptr<EntityProperty> PropertyOwner::getProperty(const unsigned int &key) const {
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

void PropertyOwner::insertProperty(std::shared_ptr<EntityProperty> prop) {

	checkLock();

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

void PropertyOwner::insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object) {

	checkLock();

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