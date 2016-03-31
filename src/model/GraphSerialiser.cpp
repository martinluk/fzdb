#include "GraphSerialiser.h"
#include "EntityManager.h"
#include "EntitySerialiser.h"
#include "StringMapSerialiser.h"
#include "PrimitiveMapSerialiser.h"

#include "spdlog/spdlog.h"

// Increment this when the format changes.
// TODO: Need to update this to cater for new things!
#define SERIAL_HEADER_CURRENT_VERSION 2

struct SerialHeader
{
    unsigned short version;             // Version of the serialisation structure.
    std::size_t size;                   // Total serialised size in bytes.

    std::size_t typeMapOffset;          // Offset from beginning of serialisation where the type map data resides.
    std::size_t typeMapLength;          // Length of this serialisation in bytes.

    std::size_t propertyNameMapOffset;
    std::size_t propertyNameMapLength;

    std::size_t propertyTypeMapOffset;
    std::size_t propertyTypeMapLength;

    std::size_t linkMapOffset;
    std::size_t linkMapLength;

    std::size_t entityDataOffset;       // Offset from beginning of serialisation where the entity data resides.
    std::size_t entityDataLength;       // Length of this serialisation in bytes.
};

struct EntityDataHeader
{
    std::size_t    entityCount;        // How many entity headers to expect after this header.
};

struct EntityHeader
{
    std::size_t offset; // Offset from beginning of the chunk that this entity resides at.
    std::size_t size;   // Size of the serialised entity in bytes.
};

GraphSerialiser::GraphSerialiser(EntityManager *manager) : _manager(manager)
{
    
}

std::size_t GraphSerialiser::serialise(Serialiser &serialiser) const
{
    std::size_t indexBase = serialiser.size();
    
    SerialHeader header;
    Serialiser::zeroBuffer(&header, sizeof(SerialHeader));
    
    std::vector<std::shared_ptr<Entity> > entList = _manager->entityList();

    header.version = SERIAL_HEADER_CURRENT_VERSION;
    header.size = 0;

    // Serialise this first with dummy data. It'll get overwritten with accurate data later.
    serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

    // Firstly serialise the type map.
    StringMapSerialiser typeMapSerialiser(&_manager->_entityTypeNames);
    header.typeMapOffset = serialiser.size() - indexBase;
    header.typeMapLength = typeMapSerialiser.serialise(serialiser);

    // Then the property strings.
    StringMapSerialiser propertyNameSerialiser(&_manager->_propertyNames);
    header.propertyNameMapOffset = serialiser.size() - indexBase;
    header.propertyNameMapLength = propertyNameSerialiser.serialise(serialiser);

    // Then the property types.
    PrimitiveMapSerialiser<unsigned int, model::types::SubType> propertyTypeSerialiser(_manager->_propertyTypes);
    header.propertyTypeMapOffset = serialiser.size() - indexBase;
    header.propertyTypeMapLength = propertyTypeSerialiser.serialise(serialiser);

    // Then the links.
    header.linkMapOffset = serialiser.size() - indexBase;
    header.linkMapLength = serialise(_manager->_links, serialiser);

    header.entityDataOffset = serialiser.size() - indexBase;

    // Then serialise the entities.
    EntityDataHeader edHeader;
    edHeader.entityCount = entList.size();
    serialiser.serialise(Serialiser::SerialProperty(&edHeader, sizeof(EntityDataHeader)));

    std::size_t indexEntityHeaders = serialiser.size();

    // Serialise the correct number of dummy entity headers.
    EntityHeader dummyHeader;
    Serialiser::zeroBuffer(&dummyHeader, sizeof(EntityHeader));

    for ( int i = 0; i < edHeader.entityCount; i++ )
    {
        serialiser.serialise(Serialiser::SerialProperty(&dummyHeader, sizeof(EntityHeader)));
    }

    std::size_t indexEntityRawData = serialiser.size();

    // Serialise each entity.
    std::size_t rawEntByteCount = 0;
    for ( int i = 0; i < edHeader.entityCount; i++ )
    {
        // Serialise the entity.
        EntitySerialiser eSer(entList[i]);
        std::size_t bytes = eSer.serialise(serialiser);
        
        // Update its header.
        EntityHeader* pHeader = &(serialiser.reinterpretCast<EntityHeader*>(indexEntityHeaders)[i]);
        pHeader->size = bytes;
        pHeader->offset = (indexEntityRawData - header.entityDataOffset) + rawEntByteCount;
        
        rawEntByteCount += bytes;
    }

    header.entityDataLength = serialiser.size() - header.entityDataOffset;
    std::size_t totalSerialised = serialiser.size() - indexBase;
    header.size = totalSerialised;

    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(indexBase);
    memcpy(pHeader, &header, sizeof(SerialHeader));
    return totalSerialised;
}

void GraphSerialiser::unserialise(const char *serialisedData, std::size_t length)
{
    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
    _manager->clearAll();
    
    try
    {
        StringMapSerialiser typeMapSerialiser(&_manager->_entityTypeNames);

        // Clear otherwise we get a double "source" entry.
        _manager->_entityTypeNames.clear();
        typeMapSerialiser.unserialise(serialisedData + pHeader->typeMapOffset, pHeader->typeMapLength);

        StringMapSerialiser propertyNameSerialiser(&_manager->_propertyNames);
        propertyNameSerialiser.unserialise(serialisedData + pHeader->propertyNameMapOffset, pHeader->propertyNameMapLength);

        PrimitiveMapSerialiser<unsigned int, model::types::SubType> propertyTypeSerialiser(_manager->_propertyTypes);
        propertyTypeSerialiser.unserialise(serialisedData + pHeader->propertyTypeMapOffset, pHeader->propertyTypeMapLength);

        unserialise(_manager->_links, serialisedData + pHeader->linkMapOffset, pHeader->linkMapLength);
    
        const EntityDataHeader* pEntData = reinterpret_cast<const EntityDataHeader*>(serialisedData + pHeader->entityDataOffset);
        const EntityHeader* pEntHeaders = reinterpret_cast<const EntityHeader*>(pEntData + 1);
        for ( int i = 0; i < pEntData->entityCount; i++ )
        {
            const EntityHeader* pEntHeader = &(pEntHeaders[i]);
            if ( (const char*)pEntHeader - serialisedData >= length )
            throw InvalidInputGraphException("Header start for entity " + std::to_string(i) + " exceeds length of input data.");
            
            const char* data = reinterpret_cast<const char*>(pEntData) + pEntHeader->offset;
            if ( data - serialisedData >= length )
            throw InvalidInputGraphException("Data start for entity " + std::to_string(i) + " exceeds length of input data.");
            
            if ( pEntHeader->offset + pEntHeader->size > length )
            throw InvalidInputGraphException("Length of entity " + std::to_string(i) + " exceeds length of input data.");
        
            _manager->insertEntity(EntitySerialiser::unserialise(data, pEntHeader->size));
        }
    }
    catch (const std::exception &ex)
    {
        _manager->clearAll();
        throw ex;
    }
}


std::size_t GraphSerialiser::serialise(const std::map<Entity::EHandle_t, std::set<Entity::EHandle_t> > &map, Serialiser &serialiser) const
{
    std::size_t origSize = serialiser.size();

    std::size_t count = map.size();
    serialiser.serialise(Serialiser::SerialProperty(&count, sizeof(count)));

    for ( auto it = map.begin(); it != map.end(); ++it )
    {
        Entity::EHandle_t key = it->first;
        serialiser.serialise(Serialiser::SerialProperty(&key, sizeof(key)));

        std::size_t numValues = it->second.size();
        serialiser.serialise(Serialiser::SerialProperty(&numValues, sizeof(numValues)));

        for ( auto it2 = it->second.begin(); it2 != it->second.end(); ++it2 )
        {
            Entity::EHandle_t val = *it2;
            serialiser.serialise(Serialiser::SerialProperty(&val, sizeof(val)));
        }
    }

    return serialiser.size() - origSize;
}

void GraphSerialiser::unserialise(std::map<Entity::EHandle_t, std::set<Entity::EHandle_t> > &map, const char *serialisedData, std::size_t length)
{
    const char* data = serialisedData;
    map.clear();

    const std::size_t* pCount = reinterpret_cast<const std::size_t*>(data);
    data += sizeof(std::size_t);
    if ( data - serialisedData > length )
        throw InvalidInputGraphException("Entity link table input is not long enough to contain header.");

    for ( std::size_t i = 0; i < *pCount; i++ )
    {
        const Entity::EHandle_t* pKey = reinterpret_cast<const Entity::EHandle_t*>(data);
        data += sizeof(Entity::EHandle_t);
        if ( data - serialisedData >= length )
            throw InvalidInputGraphException("Entity link table input " + std::to_string(i) + " exceeded length of input data.");

        const std::size_t* pNumValues = reinterpret_cast<const std::size_t*>(data);
        data += sizeof(std::size_t);
        if ( data - serialisedData > length )
            throw InvalidInputGraphException("Entity link table input " + std::to_string(i) + " exceeded length of input data.");

        std::set<Entity::EHandle_t> set;
        for ( std::size_t j = 0; j < *pNumValues; j++ )
        {
            const Entity::EHandle_t* val = reinterpret_cast<const Entity::EHandle_t*>(data);
            data += sizeof(Entity::EHandle_t);
            if ( data - serialisedData > length )
                throw InvalidInputGraphException("Entity link table value " + std::to_string(j) + " of input "
                                                 + std::to_string(i) + " exceeded length of input data.");

            set.insert(*val);
        }

        map.insert(std::pair<Entity::EHandle_t, std::set<Entity::EHandle_t> >(*pKey, set));
    }

    if ( data - serialisedData != length )
        throw InvalidInputGraphException("Unserialising entity link table did not exhause input data (used "
                                         + std::to_string((std::size_t)(data - serialisedData)) + " of "
                                         + std::to_string(length) + " bytes).");
}
