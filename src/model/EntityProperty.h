#ifndef MODEL_ENTITYPROPERTY_H
#define MODEL_ENTITYPROPERTY_H

#include <string>
#include <vector>
#include <queue>
#include <typeinfo>

#include "./ISerialisable.h"

#include "./types/String.h"
#include "./types/EntityRef.h"
#include "./types/Int.h"

class IEntityProperty {
public:
	virtual ~IEntityProperty() {
		bool a = true;
	}
	virtual int count() const = 0;
};

// An entity property is a key-values property that can be aggregated by an entity.
// Each property has a string key which acts as its identifier.
// Multiple values can be added, each with a confidence quantifier between 0 and 1,
// where 0 is impossible and 1 is certain.
// A property with a single value and a confidence quantifier of 1 is treated as a
// concrete property.
// Property keys cannot be changed once the property has been instanciated,
// as this simplifies property management within entities.

// TODO: We may want this class to be implicitly shared, so that we can return
// properties without having to perform deep copies.
template <typename T>
class EntityProperty : public IEntityProperty
{
	public:
		// Constructs a null property. This can be used for returning 'null',
		// for example if no property matches a given search.
		// isNull() will return true.
		EntityProperty();
		EntityProperty(const unsigned int &key);
		EntityProperty(const unsigned int &key,
			const std::vector<T> &values);

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
		unsigned int key() const;
		const unsigned int& keyRef() const;
		std::vector<T> values() const;
		T value(int index) const;
		int count() const;

		//T getValue(unsigned int index);

		//bool containsValue(T value);

		// If this property is concrete, returns its value only.
		// Otherwise, returns a null Variant.
		//Variant concreteValue() const;

		// Setters:

		// Appends a value to the value list.
		void append(const T &value);

		// Appends a list of values.
		void append(const std::vector<T> &list);

		// Makes this property concrete, with the given variant value.
		//void setConcrete(const Variant &value);

		// Clears this property of any values.
		void clear();

		// Implementation of ISerislisable
		virtual void serialise(Serialiser &serialiser) const;// override;
		static EntityProperty unserialise(const char* data);

	private:
		//// Header for serialisation.
		//// This is the first data item, followed by valueCount number of
		//// ValueHeaderItems.
		//struct SerialHeader
		//{
		//	std::size_t keySize;		// Length of the key string in bytes.
		//	std::size_t valueCount;		// How many ValueHeaderItems to expect after this header.
		//	std::size_t totalSize;		// Size of the header and all the data, in bytes.
		//};

		//// Represents header information for a PropertyValue.
		//// Here we store the serialised length of the given value.
		//struct ValueHeaderItem
		//{
		//	std::size_t size;			// Serialised length of this value.
		//};

		unsigned int _key;
		std::vector<T> _values;
		//std::priority_queue<PropertyValue, std::vector<PropertyValue>, PropertyValueCompare> _valuesQueue;
};

#endif	// MODEL_ENTITYPROPERTY_H
