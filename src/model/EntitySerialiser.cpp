#include "EntitySerialiser.h"
#include "Entity.h"
#include <vector>
#include "TypeSerialiser.h"
#include "EntityProperty.h"
#include <cassert>
#include "MemberSerialiser.h"
#include "spdlog/spdlog.h"

// This should be incremented whenever a change is made to the format!
#define SERIAL_HEADER_CURRENT_VERSION 1

struct SerialHeader
{
	unsigned short	version;			// Version identifier.
	std::size_t     size;				// Total size of this serialised entity in bytes.
	std::size_t     propertyCount;		// How many property headers to expect after this header.

	std::size_t		memberDataOffset;	// Offset from beginning of data that the member data begins.
	std::size_t		memberDataLength;	// Length of member data in bytes.

	std::size_t		propertyDataOffset;	// Offset from beginning of data that the property data begins.
	std::size_t		propertyDataLength;	// Length of all property data in bytes.
};

struct PropertyHeader
{
	std::size_t                 offset;     // Offset of property from beginning of property data chunk.
    std::size_t                 size;       // Size of serialised property data in bytes. This includes all values.
    unsigned int                key;        // Property key.
    model::types::Base::Subtype subtype;    // Type identifier for the values this property contains.
    std::size_t                 valueCount; // How many values this property contains.
};

EntitySerialiser::EntitySerialiser(const std::shared_ptr<Entity> ent) : _entity(ent)
{

}

std::size_t EntitySerialiser::serialise(Serialiser &serialiser) const
{
    typedef std::map<unsigned int, IEntityProperty*> PropertyTable;

    // Create the initial header.
	SerialHeader header;
	Serialiser::zeroBuffer(&header, sizeof(SerialHeader));
	std::size_t indexBegin = serialiser.size();

    // Initialise header values.
	header.version = SERIAL_HEADER_CURRENT_VERSION;
    header.propertyCount = _entity->_propertyTable.size();

	// We'll fill these in later.
	header.size = 0;
	header.memberDataOffset = 0;
	header.memberDataLength = 0;
	header.propertyDataOffset = 0;
	header.propertyDataLength = 0;

	// Create a default PropertyHeader that we'll serialise as many times as needed.
	// We set the values individually afterwards.
    PropertyHeader phPlaceholder;
	Serialiser::zeroBuffer(&phPlaceholder, sizeof(PropertyHeader));

    // Add all the headers to a property list.
    std::vector<Serialiser::SerialProperty> propList;
    propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

    for ( int i = 0; i < header.propertyCount; i++ )
    {
        propList.push_back(Serialiser::SerialProperty(&phPlaceholder, sizeof(PropertyHeader)));
    }

    // Serialise all the headers and record where the actual data starts.
    serialiser.serialise(propList);
	std::size_t indexDataBegin = serialiser.size();
	std::size_t indexPropHeadersBegin = indexBegin + sizeof(SerialHeader);

	// Serialise the data members for this entity.
	std::size_t membersSerialisedLength = _entity->_memberSerialiser.serialiseAll(serialiser);

	// Update the header for this serialisation.
	{
		SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(indexBegin);
		pHeader->memberDataOffset = indexDataBegin - indexBegin;
		pHeader->memberDataLength = membersSerialisedLength;
	}

	std::size_t propertiesOffset = serialiser.size();

    // Serialise each property.
    int propNumber = 0;
    std::size_t propDataOffset = 0;
    for ( PropertyTable::const_iterator it = _entity->_propertyTable.cbegin(); it != _entity->_propertyTable.cend(); ++it )
    {
        using namespace model::types;

        // Keep track of how many bytes all of the values occupy.
        std::size_t propSerialisedSize = 0;

        IEntityProperty* prop = it->second;

        // Serialise each value.
        for ( int i = 0; i < prop->count(); i++ )
        {
            TypeSerialiser typeSerialiser(prop->baseValue(i));
            propSerialisedSize += typeSerialiser.serialise(serialiser);
        }

		// Record results in the header.
		{
			PropertyHeader* pPropHeader = serialiser.reinterpretCast<PropertyHeader*>(indexPropHeadersBegin + (propNumber * sizeof(PropertyHeader)));
			pPropHeader->offset = propDataOffset;
			pPropHeader->size = propSerialisedSize;
			pPropHeader->key = prop->key();
			pPropHeader->subtype = prop->subtype();
			pPropHeader->valueCount = prop->count();
		}

		// Increment our counters.
        propDataOffset += propSerialisedSize;
        propNumber++;
    }

    // Record the total serialised size.
	SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(indexBegin);
	pHeader->propertyDataOffset = propertiesOffset - indexBegin;
	pHeader->propertyDataLength = propDataOffset;
	pHeader->size = serialiser.size() - indexBegin;
	return pHeader->size;
}

template <typename T>
void populate(std::shared_ptr<Entity> ent, const PropertyHeader* header, const char* data)
{
    using namespace model::types;

    std::vector<T*> values;
    for ( int i = 0; i < header->valueCount; i++ )
    {
        std::size_t advance = 0;
        T* val = dynamic_cast<T*>(TypeSerialiser::unserialise(data, &advance));
        assert(val);
        values.push_back(val);
        data += advance;
    }

    ent->insertProperty<T>(new EntityProperty<T>(header->key, values));
}

std::shared_ptr<Entity> EntitySerialiser::unserialise(const char *serialData)
{
    using namespace model::types;

    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialData);

	// Make sure the header version matches our current version.
	// For now we just throw an assertion error, but we may want to make
	// this an exception in the future.
	assert(pHeader->version == SERIAL_HEADER_CURRENT_VERSION);

    // Create an entity shell.
	std::shared_ptr<Entity> ent = std::make_shared<Entity>(0);

	// Unserialise the members.
	const char* memberData = serialData + pHeader->memberDataOffset;
	ent->_memberSerialiser.unserialiseAll(memberData);

	// Unserialise the properties.
    const PropertyHeader* pPropHeaders = reinterpret_cast<const PropertyHeader*>(serialData + sizeof(SerialHeader));
    for ( int i = 0; i < pHeader->propertyCount; i++ )
    {
		// Get the header for this property.
        const PropertyHeader* p = &(pPropHeaders[i]);

		// Calculate the pointer to the actual property data.
		const char* data = serialData + pHeader->propertyDataOffset + p->offset;
		//spdlog::get("main")->info("Property data offset: {} P offset: {}", pHeader->propertyDataOffset, p->offset);

		switch (p->subtype)
        {
		case Base::Subtype::TypeInt32:
            populate<Int>(ent, p, data);
            break;

		case Base::Subtype::TypeString:
            populate<String>(ent, p, data);
            break;

		case Base::Subtype::TypeEntityRef:
            populate<EntityRef>(ent, p, data);

        default:
            assert(false);
        }
    }

    return ent;
}
