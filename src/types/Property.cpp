#include "Property.h"
#include "../model/EntityManager.h"
#include "../singletons.h"

using namespace model::types;

std::string Property::toString() const
{
    if ( !_manager )
        throw std::runtime_error("Cannot return type string for " + std::string(getSubString(subtype())) + " property "
                                 + std::to_string(_value) + ": manager does not exist.");

    return _manager->getPropertyName(_value);
}
