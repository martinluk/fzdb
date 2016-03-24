/**
  * Right now this is aaaaallllllll manual.
  * I'd code something to parse the files but it'll probably be quicker to do it
  * by hand as a one-off.
  */

#include <iostream>

#define STRUCT_SIZE(_struct) \
    std::cout << "Size of struct " << #_struct << ": " << sizeof(_struct) << " bytes." << std::endl

#define STRUCT_MEMBER(_struct, _member) \
{ \
    _struct st; \
    std::cout << #_struct << "." << #_member << ": Offset = " << (std::size_t)((char*)&st._member - (char*)&st) << " bytes, Length: " << sizeof(st._member) << " bytes." << std::endl; \
}

#define BEGIN_NAMESPACE(_ns) \
    std::cout << "====== " << #_ns << ".cpp ======" << std::endl

#define END_NAMESPACE(_ns) \
    std::cout << "====== End " << #_ns << ".cpp ======\n" << std::endl

namespace FileSystem
{
    struct FileHeader
    {
	char identifier[4];     // Identifier - should be "FZDB".
	unsigned short version;   // File version.
	std::size_t size;       // Size of all serialised data after the file headers.
    };
}

namespace GraphSerialiser
{
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
}

int main(int argc, char** argv)
{
    {
	using namespace FileSystem;
	BEGIN_NAMESPACE(FileSystem);
	
	STRUCT_SIZE(FileHeader);
	STRUCT_MEMBER(FileHeader, identifier);
	STRUCT_MEMBER(FileHeader, version);
	STRUCT_MEMBER(FileHeader, size);
	
	END_NAMESPACE(FileSystem);
    }
    
    {
	using namespace GraphSerialiser;
	BEGIN_NAMESPACE(GraphSerialiser);
	
	STRUCT_SIZE(SerialHeader);
	STRUCT_MEMBER(SerialHeader, version);
	STRUCT_MEMBER(SerialHeader, size);
	STRUCT_MEMBER(SerialHeader, typeMapOffset);
	STRUCT_MEMBER(SerialHeader, typeMapLength);
	STRUCT_MEMBER(SerialHeader, entityDataOffset);
	STRUCT_MEMBER(SerialHeader, entityDataLength);
	
	std::cout << "\n";
	
	STRUCT_SIZE(EntityDataHeader);
	STRUCT_MEMBER(EntityDataHeader, entityCount);
	
	std::cout << "\n";
	
	STRUCT_SIZE(EntityHeader);
	STRUCT_MEMBER(EntityHeader, offset);
	STRUCT_MEMBER(EntityHeader, size);
	
	END_NAMESPACE(GraphSerialiser);
    }
    
    return 0;
}

    
