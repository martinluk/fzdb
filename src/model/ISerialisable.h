#ifndef MODEL_ISERIALISABLE_H
#define MODEL_ISERIALISABLE_H

#include "./Serialiser.h"

// Implemented by classes that need to be serialised to a stream of bytes.
class ISerialisable
{
public:
	virtual ~ISerialisable() {}

	// Serialise this object through the given serialiser.
	virtual void serialise(Serialiser& serialiser) const = 0;
};

#endif	// MODEL_ISERIALISABLE_H
