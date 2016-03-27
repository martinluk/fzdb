#ifndef FUZZY_MODEL_TYPES_SUBTYPE
#define    FUZZY_MODEL_TYPES_SUBTYPE

namespace model {
    namespace types {

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
