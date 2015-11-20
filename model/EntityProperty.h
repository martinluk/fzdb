#ifndef MODEL_ENTITYPROPERTY_H
#define MODEL_ENTITYPROPERTY_H

#include <string>
#include <vector>
#include "./PropertyValue.h"
#include "./Variant.h"

// An entity property is a key-values property that can be aggregated by an entity.
// Each property has a string key which acts as its identifier.
// Multiple values can be added, each with a confidence quantifier between 0 and 1,
// where 0 is impossible and 1 is certain.
// A property with a single value and a confidence quantifier of 1 is treated as a
// concrete property.
// Property keys cannot be changed once the property has been instanciated,
// as this simplifies property management within entities.
class EntityProperty
{
	public:
		// Constructs a null property. This can be used for returning 'null',
		// for example if no property matches a given search.
		// isNull() will return true.
		EntityProperty();

		// Returns true if this is a null property (ie. default-constructed).
		// Internally, a property is null if its key is an empty string.
		bool isNull() const;
		
		// Returns true if this property is concrete.
		// A property is concrete if it has one value of confidence 1.
		bool isConcrete() const;

		// Returns true if the property is empty.
		// A property is empty if it has no values.
		bool isEmpty() const;

		// Getters
		std::string key() const;
		std::vector<PropertyValue> values() const;
		PropertyValue value(int index) const;
		int count() const;

		// If this property is concrete, returns its value only.
		// Otherwise, returns a null Variant.
		Variant concreteValue() const;

		// Setters:

		// Appends a value to the value list.
		void append(const PropertyValue &value);

		// Appends a list of values.
		void append(const std::vector<PropertyValue> &list);

		// Makes this property concrete, with the given variant value.
		void setConcrete(const Variant &value);

		// Clears this property of any values.
		void clear();

	private:
		std::string	key_;
		std::vector<PropertyValue> values_;
};

#endif	// MODEL_ENTITYPROPERTY_H
