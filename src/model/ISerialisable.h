#ifndef MODEL_ISERIALISABLE_H
#define MODEL_ISERIALISABLE_H

#include <cstddef>
#include <utility>

// Implemented by classes that need to be serialised to a stream of bytes.
class ISerialisable
{
public:
	virtual ~ISerialisable() {}

	// If buffer is not NULL, serialises this object to the memory pointed to
	// by buffer. No more than maxSize bytes will be written. Returns the number
	// of bytes written and a boolean specifying whether the entire object was
	// able to be written.
	// If buffer is NULL, no writing occurs and the return value is the minimum
	// size in bytes of a buffer that this serialised object would fit into.
	virtual std::pair<std::size_t,bool> serialise(char* buffer, std::size_t maxSize) const = 0;
};

#endif	// MODEL_ISERIALISABLE_H
