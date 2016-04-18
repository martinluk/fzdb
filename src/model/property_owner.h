#ifndef MODEL_PROPERTY_OWNER_H
#define MODEL_PROPERTY_OWNER_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "./triple.h"
#include "./entity_property.h"
#include "./types/subtype.h"
#include "./member_serialiser.h"

class EntityManager;

enum class MatchState {
	None,
	Linked,
	UpTree,
	DownTree
};

// This is basically a stripped-down Entity class.
// TODO: (Jonathan) Tim, you're probably better at explaining this one.
class PropertyOwner
{
    friend class EntityManager;
	friend class PropertyOwnerSerialiser;
public:
    PropertyOwner();
    virtual ~PropertyOwner();

	// Getters:

	// Returns the property with the given key, or a null property if this is not found.
	virtual std::shared_ptr<EntityProperty> getProperty(const unsigned int &key) const;

	virtual void insertProperty(std::shared_ptr<EntityProperty> prop, MatchState state = MatchState::None);

    virtual void insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object, MatchState state = MatchState::None,
		EntityProperty::Type propType = EntityProperty::Type::FUZZY);

    // Removes the property with the given key.
    void removeProperty(const unsigned int &key);

    // Tests if the entity has a property
    virtual bool hasProperty(const unsigned int &key, MatchState state = MatchState::None) const;

    // Returns read only reference to the property table
    const std::map<unsigned int, std::shared_ptr<EntityProperty>>& properties() const;

    // Tests if the entity meets the condition
    virtual std::vector<std::shared_ptr<model::types::Base>> meetsCondition(unsigned int propertyId, const model::Object&& obj, MatchState state = MatchState::None);
	virtual std::vector<std::shared_ptr<model::types::Base>> meetsCondition(unsigned int propertyId, const std::shared_ptr<model::types::Base>&& value, MatchState state = MatchState::None);

    // Clears all properties on the entity.
    void clearProperties();

    // Returns the number of properties present.
    int propertyCount() const;

	void lock() { 
		_locked = true; 
	}

protected:

	std::map<unsigned int, std::shared_ptr<EntityProperty>> _propertyTable;
	bool _locked;
    const EntityManager* _manager;	

	inline void checkLock() {
		if (_locked) {
			throw std::runtime_error("This item is read only");
		}
	}

private:
	void initMemberSerialiser();
	MemberSerialiser& memberSerialiser();
	MemberSerialiser _memberSerialiser;
};

#endif    // MODEL_PROPERTY_OWNER_H
