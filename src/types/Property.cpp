#include "Property.h"
#include "../model/EntityManager.h"
#include "../singletons.h"

using namespace model::types;

std::string Property::toString() const
{
	return Singletons::database()->entityManager().getPropertyName(_value);
}
