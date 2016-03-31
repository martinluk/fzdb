#include "Property.h"
#include "../model/EntityManager.h"
#include "../singletons.h"

using namespace model::types;

std::string Property::toString() const
{
    if ( !_manager )
        throw std::runtime_error("Cannot return entity type string: manager does not exist.");

    return _manager->getPropertyName(_value);
}
