#include "./entity_ref.h"

void model::types::EntityRef::initMemberSerialiser() {
  _memberSerialiser.addPrimitive(&_value, sizeof(_value));

  _memberSerialiser.setInitialised();
}

model::types::EntityRef::EntityRef() : Base(), _value(0) {
}

model::types::EntityRef::EntityRef(EHandle_t value) : Base(), _value(value) {
}