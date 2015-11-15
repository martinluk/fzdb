#include "./EntityProperty.h"

EntityProperty::EntityProperty()
{
	// Nothing to do - key will automatically be empty.
}

bool EntityProperty::isNull() const
{
	return key_.empty();
}
