#ifndef MODEL_ENTITYPROPERTY_H
#define MODEL_ENTITYPROPERTY_H

#include <string>
#include <vector>
#include <queue>
#include <typeinfo>
#include <algorithm>
#include <forward_list>

#include "./types/String.h"
#include "./types/EntityRef.h"
#include "./types/Int.h"
#include "./types/Date.h"
#include "ILogString.h"

using BasePointer = std::shared_ptr<model::types::Base>;

class IEntityProperty : public ILogString
{
public:
	virtual ~IEntityProperty() {}
	virtual unsigned int count() const = 0;
	virtual unsigned int key() const = 0;
	virtual BasePointer baseValue(int index) const = 0;
	virtual std::vector<BasePointer> baseValues() const = 0;
	virtual model::types::Base::Subtype subtype() const = 0;
	virtual BasePointer baseTop() const = 0;
};

// An entity property is a key-values property that can be aggregated by an entity.
// Each property has a string key which acts as its identifier.
// Multiple values can be added, each with a confidence quantifier between 0 and 100,
// where 0 is impossible and 100 is certain.
// A property with a single value and a confidence quantifier of 1 is treated as a
// concrete property.
// Property keys cannot be changed once the property has been instanciated,
// as this simplifies property management within entities.

// 14/01/16 - Changed this so that it holds values on the heap,
// otherwise unserialisation is nigh impossible because it needs to be polymorphic.
// The object owns all of its values.

// TODO: We may want this class to be implicitly shared, so that we can return
// properties without having to perform deep copies.
template <typename T>
class EntityProperty : public IEntityProperty
{
	friend class EntitySerialiser;
public:
	// Constructs a null property. This can be used for returning 'null',
	// for example if no property matches a given search.
	// isNull() will return true.
	EntityProperty();
	EntityProperty(const unsigned int &key);
	~EntityProperty();
	EntityProperty(const unsigned int &key,
		const std::vector<std::shared_ptr<T> > &values);

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
	const unsigned int& keyRef() const;

	virtual unsigned int count() const;

	std::vector<std::shared_ptr<T>> values() const;
	std::shared_ptr<T> const& value(int index) const;

	virtual BasePointer baseValue(int index) const;
	virtual std::vector<BasePointer> baseValues() const;

	virtual std::shared_ptr<T> top() const;
	virtual BasePointer baseTop() const;
	
	virtual unsigned int key() const;
	virtual model::types::Base::Subtype subtype() const { return _subtype; }

	// Setters:

	// Appends a value to the value list.
	void append(std::shared_ptr<T> value);

	// Appends a list of values.
	void append(const std::vector<std::shared_ptr<T>> &list);

	// Makes this property concrete, with the given variant value.
	//void setConcrete(const Variant &value);

	// Clears this property of any values.
	void clear();
	
	// Removes any entries whose value matches.
	// This ignores the confidence of the value.
	void remove(const T &value);

	virtual std::string logString() const override;

private:
	void initSubtype();

	unsigned int _key;
	unsigned int _count;
	model::types::Base::Subtype _subtype;

	std::forward_list<std::shared_ptr<T>> _valuesList;
};

#endif	// MODEL_ENTITYPROPERTY_H
