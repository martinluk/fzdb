#ifndef FUZZY_MODEL_TYPES_SUBTYPE
#define FUZZY_MODEL_TYPES_SUBTYPE

#define SUBTYPE_VALUE_MASK 0b100000
#define SUBTYPE_REFERENCE_MASK 0b010000

namespace model {
    namespace types {

        // Enum for the different subtypes that are defined.
        enum class SubType
        {
            Undefined = 0x0,
			SUBTYPE_COUNT = 0x1,

			PropertyReference = SUBTYPE_REFERENCE_MASK | 0x0,
			ValueReference = SUBTYPE_REFERENCE_MASK | 0x1,
			
			Int32 = SUBTYPE_VALUE_MASK | 0x0,
            String = SUBTYPE_VALUE_MASK | 0x1,
            EntityRef = SUBTYPE_VALUE_MASK | 0x2,
            Date = SUBTYPE_VALUE_MASK | 0x3,
			UInt32 = SUBTYPE_VALUE_MASK | 0x4,
			TimeStamp = SUBTYPE_VALUE_MASK | 0x5,
			TypeID = SUBTYPE_VALUE_MASK | 0x6,
			AuthorID = SUBTYPE_VALUE_MASK | 0x7,
			SourceRef = SUBTYPE_VALUE_MASK | 0x8,
			Confidence = SUBTYPE_VALUE_MASK | 0x9
        };

        // Strings corresponding to the enum entries.
        static const char* SubString[] =
        {
            "Undefined",
            "Int32",
            "String",
            "EntityRef",
            "Date",

            "PropertyReference",
            "ValueReference"
        };

        // Convenience function to get a string for a given subtype.
        static const char* getSubString(SubType t)
        {
            return ( t < SubType::Undefined || t >= SubType::SUBTYPE_COUNT )
                ? SubString[(int)SubType::Undefined]
                : SubString[(int)t];
        }

        class Base;
    }
}

#endif //FUZZY_MODEL_TYPES_SUBTYPE
