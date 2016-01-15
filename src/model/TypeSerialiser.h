#ifndef MODEL_TYPES_TYPESERIALISER_H
#define MODEL_TYPES_TYPESERIALISER_H

#include <cstring>
#include "Serialiser.h"

namespace model
{
    namespace types
    {
        class Base;

        class TypeSerialiser
        {
        public:
            TypeSerialiser(const Base* type);

            std::size_t serialise(Serialiser &serialiser) const;
            Base* unserialise(const char* serialisedData);

        private:
            const Base* baseType_;
        };
    }
}

#endif  // MODEL_TYPES_TYPESERIALISER_H
