#include "EntityRef.h"

void model::types::EntityRef::initMemberSerialiser()
{
	_memberSerialiser.addPrimitive(&_value, sizeof(_value));

	_memberSerialiser.setInitialised();
}

model::types::EntityRef::EntityRef() : Base(), _value(0)
{
}

model::types::EntityRef::EntityRef(EHandle_t value, unsigned int author, unsigned char confidence, const std::string & comment) : Base(), _value(value)
{
}