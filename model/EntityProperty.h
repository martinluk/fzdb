#ifndef MODEL_ENTITYPROPERTY_H
#define MODEL_ENTITYPROPERTY_H

#include <string>

// An entity property is a key-values property that can be aggregated by an entity.
// Each property has a string key which acts as its identifier.
// Multiple values can be added, each with a confidence quantifier between 0 and 1,
// where 0 is impossible and 1 is certain.
// A property with a single value and no confidence quantifier is treated as a
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
		
	private:
		std::string	key_;
};

#endif	// MODEL_ENTITYPROPERTY_H
