#include "EntitySerialiser.h"
#include "Entity.h"
#include <vector>
#include "TypeSerialiser.h"
#include "EntityProperty.h"
#include <cassert>

struct SerialHeader
{
    std::size_t     size;           // Total size of this serialised entity in bytes.
    EHandle_t       handle;         // Entity handle.
    unsigned int    type;           // Entity type.
    std::size_t     propertyCount;  // How many property headers to expect after this header.
};

struct PropertyHeader
{
    std::size_t                 offset;     // Offset of property data from beginning of the entire serialisation.
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
    memset(&header, 0, sizeof(SerialHeader));
    std::size_t origSize = serialiser.size();

    // Initialise header values.
    header.size = 0;    // We don't know this yet.
    header.handle = _entity->handle_;
    header.type = _entity->_type;
    header.propertyCount = _entity->_propertyTable.size();

    // Create a default PropertyHeader that we'll serialise as many times as needed.
    PropertyHeader phPlaceholder;
    memset(&phPlaceholder, 0, sizeof(PropertyHeader));

    // Add all the headers to a property list.
    std::vector<Serialiser::SerialProperty> propList;
    propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

    for ( int i = 0; i < header.propertyCount; i++ )
    {
        propList.push_back(Serialiser::SerialProperty(&phPlaceholder, sizeof(PropertyHeader)));
    }

    // Serialise all the headers and record where the actual data starts.
    serialiser.serialise(propList);
    std::size_t propDataBegin = serialiser.size();
    std::size_t propHeaderBegin = origSize + sizeof(SerialHeader);

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
        PropertyHeader* pPropHeader = serialiser.reinterpretCast<PropertyHeader*>(propHeaderBegin + (propNumber * sizeof(PropertyHeader)));
        pPropHeader->size = propSerialisedSize;
        pPropHeader->key = prop->key();
        pPropHeader->valueCount = prop->count();
        pPropHeader->offset = (propDataBegin - origSize) + propDataOffset;
        pPropHeader->subtype = prop->subtype();

        // Increment our counters
        propDataOffset += propSerialisedSize;
        propNumber++;
    }

    // Record the total serialised size.
    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(origSize);
    pHeader->size = serialiser.size() - origSize;
	return pHeader->size;
}

template <typename T>
void populate(std::shared_ptr<Entity> ent, const PropertyHeader* header, const char* data)
{
    using namespace model::types;

    std::vector<T*> values;
    for ( int i = 0; i < header->valueCount; i++ )
    {
        // Data is automatically incremented.
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

//    for ( int i = 0; i < valueCount; i++ )
//    {
//        // serialisedData will be incremented.
//        T* val = dynamic_cast<T*>(TypeSerialiser::unserialise(serialisedData));
//        assert(val);
//        append(val);
//    }

    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialData);

    // Create an entity shell.
	std::shared_ptr<Entity> ent = std::make_shared<Entity>(pHeader->type, pHeader->handle);

    const PropertyHeader* pPropHeaders = reinterpret_cast<const PropertyHeader*>(serialData + sizeof(SerialHeader));
    for ( int i = 0; i < pHeader->propertyCount; i++ )
    {
        const PropertyHeader* p = &(pPropHeaders[i]);
        const char* data = serialData + p->offset;

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
