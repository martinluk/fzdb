#ifndef MODEL_ISERIALISABLE_H
#define MODEL_ISERIALISABLE_H

#include "./Serialiser.h"

// TODO: Do we still need this interface?
namespace model
{
    // Implemented by classes that need to be serialised to a stream of bytes.
    class ISerialisable
    {
    public:
            virtual ~ISerialisable() {}

            // Serialise this object through the given serialiser.
            // Returns the number of bytes serialised.
            virtual std::size_t serialise(Serialiser& serialiser) const = 0;
    };
}

#endif	// MODEL_ISERIALISABLE_H
