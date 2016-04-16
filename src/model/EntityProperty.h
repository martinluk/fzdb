#ifndef MODEL_ENTITYPROPERTY_H
#define MODEL_ENTITYPROPERTY_H

#include <string>
#include <vector>
#include <queue>
#include <typeinfo>
#include <algorithm>
#include <forward_list>
#include <memory>

#include "ILogString.h"
#include "./types/SubType.h"
//#include "./types/Base.h"

using BasePointer = std::shared_ptr<model::types::Base>;

// An entity property is a key-values property that can be aggregated by an entity.
// Each property has a string key which acts as its identifier.
// Multiple values can be added, each with a confidence quantifier between 0 and 100.
// A property with a single value and a confidence quantifier of 100 is treated as a
// concrete property.
// Property keys cannot be changed once the property has been instanciated,
// as this simplifies property management within entities.

// (Jonathan) 14/01/16 - Changed this so that it holds values on the heap,
// otherwise unserialisation is nigh impossible because it needs to be polymorphic.
// The object owns all of its values.

class EntityProperty : public ILogString
{
    friend class EntitySerialiser;
public:

	enum class Type {
		FUZZY,
		LOCKED,
		CONCRETESINGLE,
		CONCRETEMULTI
	};
    // Constructs a null property. This can be used for returning 'null',
    // for example if no property matches a given search.
    // isNull() will return true.
    EntityProperty(Type type);
    EntityProperty(Type type, const unsigned int &key, model::types::SubType subtype);
	EntityProperty(Type type, const unsigned int &key, model::types::SubType subtype, const std::vector<BasePointer> &values);

    ~EntityProperty();   

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

    //std::vector<std::shared_ptr<T>> values() const;
    //std::shared_ptr<T> const& value(int index) const;

    virtual BasePointer baseValue(int index) const;
    virtual std::vector<BasePointer> baseValues() const;

    //virtual std::shared_ptr<T> top() const;
    virtual BasePointer baseTop() const;
    
    virtual unsigned int key() const;
    virtual model::types::SubType subtype() const { return _subtype; }

    // Setters:

    // Appends a value to the value list.
    void append(BasePointer value);

    // Appends a list of values.
    void append(const std::vector<BasePointer> &list);

    // Clears this property of any values.
    void clear();

    // Removes any entries whose value matches.
    // This ignores the confidence of the value.
    void remove(const model::types::Base &value);
    
    //Removes entries whose orderingId matches the one in parameter.
    void remove(const int orderingId);

    virtual std::string logString(const Database* db = NULL) const override;

    bool memberwiseEqual(const EntityProperty* other) const;

	void lock();

	std::shared_ptr<EntityProperty> Clone() const;

	Type type() const;

private:
	Type _type;
    unsigned int _key;
    model::types::SubType _subtype;

    std::vector<BasePointer> _valuesList;
};

#endif    // MODEL_ENTITYPROPERTY_H
