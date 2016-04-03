#ifndef FUZZY_MODEL_TYPES_SUBTYPE
#define FUZZY_MODEL_TYPES_SUBTYPE

#define SUBTYPE_VALUE_MASK 0b100000
#define SUBTYPE_REFERENCE_MASK 0b010000

namespace model {
    namespace types {

        // Enum for the different subtypes that are defined.
        enum class SubType
        {
            TypeUndefined = 0x0,
			SUBTYPE_COUNT = 0x1,

			PropertyReference = SUBTYPE_REFERENCE_MASK | 0x0,
			ValueReference = SUBTYPE_REFERENCE_MASK | 0x1,
			
			TypeInt32 = SUBTYPE_VALUE_MASK | 0x0,
            TypeString = SUBTYPE_VALUE_MASK | 0x1,
            TypeEntityRef = SUBTYPE_VALUE_MASK | 0x2,
            TypeDate = SUBTYPE_VALUE_MASK | 0x3
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
