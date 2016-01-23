#ifndef MODEL_ENTITYPROPERTY_H
#define MODEL_ENTITYPROPERTY_H

#include <string>
#include <vector>
#include <queue>
#include <typeinfo>

#include "./types/String.h"
#include "./types/EntityRef.h"
#include "./types/Int.h"
#include "ILogString.h"

class IEntityProperty : public ILogString
{
public:
	virtual ~IEntityProperty() {}
	virtual int count() const = 0;
	virtual std::vector<model::types::Base*> baseValues() const = 0;
	virtual unsigned int key() const = 0;
	virtual model::types::Base* baseValue(int index) const = 0;
        virtual model::types::Base::Subtype subtype() const = 0;
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
			const std::vector<T*> &values);

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
		std::vector<T*> values() const;

		std::vector<model::types::Base*> baseValues() const override {
			std::vector<model::types::Base*> out;
			for (auto vals : _values) out.push_back(vals);
			return out;
		}

		T* value(int index) const;

		//T getValue(unsigned int index);

		//bool containsValue(T value);

                virtual int count() const;
                virtual model::types::Base* baseValue(int index) const;
                virtual unsigned int key() const;
                virtual model::types::Base::Subtype subtype() const { return _subtype; }

		// Setters:

		// Appends a value to the value list.
		void append(T* value);

		// Appends a list of values.
		void append(const std::vector<T*> &list);

		// Makes this property concrete, with the given variant value.
		//void setConcrete(const Variant &value);

		// Clears this property of any values.
		void clear();
        
        virtual std::string logString() const override;

	private:
		void deleteAllValues();
        void initSubtype();

		unsigned int _key;
		std::vector<T*> _values;
                model::types::Base::Subtype _subtype;
		//std::priority_queue<PropertyValue, std::vector<PropertyValue>, PropertyValueCompare> _valuesQueue;
};

#endif	// MODEL_ENTITYPROPERTY_H
