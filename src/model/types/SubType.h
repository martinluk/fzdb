#ifndef FUZZY_MODEL_TYPES_SUBTYPE
#define	FUZZY_MODEL_TYPES_SUBTYPE

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
			ValueReference
		};

		class Base;
	}
}

#endif //FUZZY_MODEL_TYPES_SUBTYPE