#include "GraphSerialiser.h"
#include "EntityManager.h"
#include "EntitySerialiser.h"

struct SerialHeader
{
    std::size_t size;           // Total serialised size in bytes.
    std::size_t entityCount;    // How many entities were serialised.
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
    
    EntityHeader dummyHeader;
    memset(&dummyHeader, 0, sizeof(EntityHeader));
    
    serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
    for ( int i = 0; i < header.entityCount; i++ )
    {
        serialiser.serialise(Serialiser::SerialProperty(&dummyHeader, sizeof(EntityHeader)));
    }
    
    std::size_t entityHeaders = origSize + sizeof(SerialHeader);
    std::size_t dataBegin = serialiser.size();
    
    std::size_t dataSerialised = 0;
    for ( int i = 0; i < header.entityCount; i++ )
    {
        // Serialise the entity.
        EntitySerialiser eSer(entList[i]);
        std::size_t bytes = eSer.serialise(serialiser);
        
        // Update its header.
        EntityHeader* pHeader = &(serialiser.reinterpretCast<EntityHeader*>(entityHeaders)[i]);
        pHeader->size = bytes;
        pHeader->offset = dataBegin + dataSerialised;
        
        dataSerialised += bytes;
    }
    
    SerialHeader* pHeader = serialiser.reinterpretCast<SerialHeader*>(origSize);
    std::size_t totalSerialised = serialiser.size() - origSize;
    pHeader->size = totalSerialised;
    
    return totalSerialised;
}

void GraphSerialiser::unserialise(const char *serialisedData)
{
    const SerialHeader* pHeader = reinterpret_cast<const SerialHeader*>(serialisedData);
    const EntityHeader* otherHeaders = reinterpret_cast<const EntityHeader*>(serialisedData + sizeof(SerialHeader));
    
    for ( int i = 0; i < pHeader->entityCount; i++ )
    {
        const EntityHeader* pEntHeader = &(otherHeaders[i]);
        const char* data = serialisedData + pEntHeader->offset;
        
        _manager->insertEntity(EntitySerialiser::unserialise(data));
    }
}
