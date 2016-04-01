#ifndef FUZZY_MODEL_TYPES_SUBTYPE
#define    FUZZY_MODEL_TYPES_SUBTYPE

namespace model {
    namespace types {

        // Enum for the different subtypes that are defined.
        enum class SubType
        {
            TypeUndefined = 0,
            TypeInt32,
            TypeString,
            TypeEntityRef,
            TypeDate,

            //Used in query processing but cannot be stored
            PropertyReference,
            ValueReference,

            SUBTYPE_COUNT
        };

        // Strings corresponding to the enum entries.
        static const char* SubTypeString[] =
        {
            "TypeUndefined",
            "TypeInt32",
            "TypeString",
            "TypeEntityRef",
            "TypeDate",

            "PropertyReference",
            "ValueReference"
        };

        // Convenience function to get a string for a given subtype.
        static const char* getSubTypeString(SubType t)
        {
            return ( t < SubType::TypeUndefined || t >= SubType::SUBTYPE_COUNT )
                ? SubTypeString[(int)SubType::TypeUndefined]
                : SubTypeString[(int)t];
        }

        class Base;
    }
}

#endif //FUZZY_MODEL_TYPES_SUBTYPE
