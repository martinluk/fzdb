#include "GraphSerialiser.h"
#include "EntityManager.h"
#include "EntitySerialiser.h"
#include "StringMapSerialiser.h"

struct SerialHeader
{
	std::size_t size;				// Total serialised size in bytes.
	std::size_t entityCount;		// How many entities were serialised.

	std::size_t typeMapOffset;		// Offset from beginning of serialisation where the type map data resides.
	std::size_t	typeMapLength;		// Length of this serialisation in bytes.
};

struct EntityHeader
{
    std::size_t offset; // Offset from beginning of serialisation that this entity resides at.
    std::size_t size;   // Size of the serialised entity in bytes.
};

GraphSerialiser::GraphSerialiser(EntityManager *manager) : _manager(manager)
{
    
}

std::size_t GraphSerialiser::serialise(Serialiser &serialiser) const
{
    std::size_t origSize = serialiser.size();
    
    SerialHeader header;
    memset(&header, 0, sizeof(SerialHeader));
    
    std::vector<Entity*> entList = _manager->entityList();
    
    header.size = 0;                        // Don't know this yet.
    header.entityCount = entList.size();

	header.typeMapOffset = 0;
	header.typeMapLength = 0;
    
	// Serialise the correct number of dummy entity headers.
    EntityHeader dummyHeader;
    memset(&dummyHeader, 0, sizeof(EntityHeader));
    
    serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
    for ( int i = 0; i < header.entityCount; i++ )
    {
        serialiser.serialise(Serialiser::SerialProperty(&dummyHeader, sizeof(EntityHeader)));
    }
    
    std::size_t entityHeaders = origSize + sizeof(SerialHeader);
    std::size_t dataBegin = serialiser.size();

	// Firstly serialise the type map.
	StringMapSerialiser typeMapSerialiser(&_manager->_entityTypeNames);
	std::size_t typeMapBytesSerialised = typeMapSerialiser.serialise(serialiser);
	std::size_t dataSerialised = typeMapBytesSerialised;

	// Then serialise each entity.
    for ( int i = 0; i < header.entityCount; i++ )
    {
        // Serialise the entity.
        EntitySerialiser eSer(entList[i]);
        std::size_t bytes = eSer.serialise(serialiser);
        
        // Update its header.
        EntityHeader* pHeader = &(serialiser.reinterpretCast<EntityHeader*>(entityHeaders)[i]);
        pHeader->size = bytes;
		pHeader->offset = dataBegin - origSize + dataSerialised;
        
        dataSerialised += bytes;
    }
    
    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(origSize);
    std::size_t totalSerialised = serialiser.size() - origSize;
    pHeader->size = totalSerialised;
	pHeader->typeMapOffset = dataBegin - origSize;
	pHeader->typeMapLength = typeMapBytesSerialised;
    
    return totalSerialised;
}

void GraphSerialiser::unserialise(const char *serialisedData)
{
    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
    const EntityHeader* otherHeaders = reinterpret_cast<const EntityHeader*>(serialisedData + sizeof(SerialHeader));

	StringMapSerialiser typeMapSerialiser(&_manager->_entityTypeNames);
	typeMapSerialiser.unserialise(serialisedData + pHeader->typeMapOffset);
    
    for ( int i = 0; i < pHeader->entityCount; i++ )
    {
        const EntityHeader* pEntHeader = &(otherHeaders[i]);
        const char* data = serialisedData + pEntHeader->offset;
        
        _manager->insertEntity(EntitySerialiser::unserialise(data));
    }
}
