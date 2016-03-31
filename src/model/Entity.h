#ifndef MODEL_ENTITY_H
#define MODEL_ENTITY_H

#include <map>
#include <vector>
#include <memory>
#include <algorithm>

#include "./Triple.h"
#include "./PropertyOwner.h"
#include "./MemberSerialiser.h"
#include "ILogString.h"

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

	std::vector<std::shared_ptr<model::types::Base>> meetsCondition(unsigned int propertyId, const model::Object&& obj, bool linked = false) override;

	bool hasProperty(const unsigned int &key, bool linked = false) const override;
	std::shared_ptr<EntityProperty> getProperty(const unsigned int &key) const override;

private:
    void initMemberSerialiser();

    void linkStatus(const LinkStatus linkStatus) {
        _linkStatus = linkStatus;
    }

private:
    EHandle_t _handle;
    unsigned int _type;
    LinkStatus _linkStatus;

    MemberSerialiser _memberSerialiser;

};

#endif    // MODEL_ENTITY_H
