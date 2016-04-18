#ifndef MODEL_ENTITY_H
#define MODEL_ENTITY_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "./triple.h"
#include "./property_owner.h"
#include "./member_serialiser.h"
#include "./ilog_string.h"

// Represents an entity in the graph database.
// Each entity has a handle, which is a unique identifier.
// An entity's handle cannot be changed once it is instanciated.
class Entity : public ILogString, public PropertyOwner
{
    friend class EntitySerialiser;
    friend class EntityManager;
public:
    // Typedef for an entity handle. This should uniquely identify an entity.
    typedef unsigned long long EHandle_t;

    // Invalid entity handle. This implies that the entity is null.
    static const EHandle_t INVALID_EHANDLE;

    // Constructs a null entity.
    Entity(unsigned int type);

    // Constructs an entity with the given handle.
    Entity(unsigned int type, EHandle_t handle);
	Entity(unsigned int type, EHandle_t handle, const PropertyOwner&& base);

    ~Entity();

    // Returns whether this entity is null, ie. whether it has an invalid handle.
    bool isNull() const;

    // Returns this entity's handle.
    EHandle_t getHandle() const;

    unsigned int getType() const;    

    virtual std::string logString(const Database* db = NULL) const override;

    enum class LinkStatus {
        None,
        Master,
        Slave
    };

    LinkStatus linkStatus() const {
        return _linkStatus;
    }

	std::vector<std::shared_ptr<model::types::Base>> meetsCondition(unsigned int propertyId, const model::Object&& obj, MatchState state = MatchState::None) override;
	std::vector<std::shared_ptr<model::types::Base>> meetsCondition(unsigned int propertyId, const std::shared_ptr<model::types::Base>&& value, MatchState state = MatchState::None) override;

    bool memberwiseEqual(const Entity* other) const;


	bool hasProperty(const unsigned int &key, MatchState state = MatchState::None) const override;
	std::shared_ptr<EntityProperty> getProperty(const unsigned int &key) const override;

	void insertProperty(std::shared_ptr<EntityProperty> prop, MatchState state = MatchState::None) override;

	void insertProperty(unsigned int key, std::shared_ptr<model::types::Base> object, MatchState state = MatchState::None,
		EntityProperty::Type propType = EntityProperty::Type::FUZZY) override;

    void removeProperty(const unsigned int & key) ;
private:
    void initMemberSerialiser();

    void linkStatus(const LinkStatus linkStatus) {
        _linkStatus = linkStatus;
    }

	EHandle_t _handle;
    LinkStatus _linkStatus;

    MemberSerialiser _memberSerialiser;

};

#endif    // MODEL_ENTITY_H
