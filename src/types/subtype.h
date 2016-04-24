#ifndef FUZZY_MODEL_TYPES_SUBTYPE
#define FUZZY_MODEL_TYPES_SUBTYPE

namespace model {
namespace types {

enum class TypePosition {
  SUBJECT     = 0b10000000,
  PREDICATE   = 0b01000000,
  OBJECT      = 0b00100000,
  METASUBJECT = 0b00010000,
};

// Enum for the different subtypes that are defined.
enum class SubType {
  Undefined = 0x0,
  SUBTYPE_COUNT = 0x1,

  PropertyReference = ((int)TypePosition::PREDICATE)   | 0x0,
  ValueReference    = ((int)TypePosition::METASUBJECT) | 0x1,
  EntityRef         = ((int)TypePosition::SUBJECT)     | ((int)TypePosition::OBJECT) | 0x2,
  SourceRef         = ((int)TypePosition::SUBJECT)     | ((int)TypePosition::OBJECT) | 0x3,

  Int32       = ((int)TypePosition::OBJECT) | 0x0,
  String      = ((int)TypePosition::OBJECT) | 0x1,
  Date        = ((int)TypePosition::OBJECT) | 0x2,
  UInt32      = ((int)TypePosition::OBJECT) | 0x3,
  TimeStamp   = ((int)TypePosition::OBJECT) | 0x4,
  TypeID      = ((int)TypePosition::OBJECT) | 0x5,
  AuthorID    = ((int)TypePosition::OBJECT) | 0x6,
  Confidence  = ((int)TypePosition::OBJECT) | 0x7
};

// Strings corresponding to the enum entries.
static const char* SubString[] = {
  "Undefined",
  "Int32",
  "String",
  "EntityRef",
  "Date",

  "PropertyReference",
  "ValueReference"
};

// Convenience function to get a string for a given subtype.
static const char* getSubString(SubType t) {
  return ( t < SubType::Undefined || t >= SubType::SUBTYPE_COUNT )
         ? SubString[(int)SubType::Undefined]
         : SubString[(int)t];
}

class Base;
}
}

#endif //FUZZY_MODEL_TYPES_SUBTYPE
