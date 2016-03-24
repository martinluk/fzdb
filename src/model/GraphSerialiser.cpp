#include "GraphSerialiser.h"
#include "EntityManager.h"
#include "EntitySerialiser.h"
#include "StringMapSerialiser.h"

#include "spdlog/spdlog.h"

// Increment this when the format changes.
#define SERIAL_HEADER_CURRENT_VERSION 1

struct SerialHeader
{
	unsigned short version;			// Version of the serialisation structure.
	std::size_t size;				// Total serialised size in bytes.

	std::size_t typeMapOffset;		// Offset from beginning of serialisation where the type map data resides.
	std::size_t	typeMapLength;		// Length of this serialisation in bytes.

	std::size_t entityDataOffset;	// Offset from beginning of serialisation where the entity data resides.
	std::size_t	entityDataLength;	// Length of this serialisation in bytes.
};

struct EntityDataHeader
{
	std::size_t	entityCount;		// How many entity headers to expect after this header.
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
    
	// Zero the fields we don't know yet.
	header.size = 0;
	header.typeMapOffset = 0;
	header.typeMapLength = 0;
	header.entityDataOffset = 0;
	header.entityDataLength = 0;

	serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
	std::size_t indexDataBegin = serialiser.size();

	// Firstly serialise the type map.
	StringMapSerialiser typeMapSerialiser(&_manager->_entityTypeNames);
	header.typeMapOffset = indexDataBegin - indexBase;
	header.typeMapLength = typeMapSerialiser.serialise(serialiser);

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
    
	SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(indexBase);
	std::size_t totalSerialised = serialiser.size() - indexBase;
    pHeader->size = totalSerialised;
	pHeader->typeMapOffset = header.typeMapOffset;
	pHeader->typeMapLength = header.typeMapLength;
	pHeader->entityDataOffset = header.entityDataOffset;
	pHeader->entityDataLength = header.entityDataLength;
    
    return totalSerialised;
}

void GraphSerialiser::unserialise(const char *serialisedData, std::size_t length)
{
	const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
	_manager->clearAll();
	
	try
	{
	    StringMapSerialiser typeMapSerialiser(&_manager->_entityTypeNames);
	    typeMapSerialiser.unserialise(serialisedData + pHeader->typeMapOffset, pHeader->typeMapLength);
	
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
