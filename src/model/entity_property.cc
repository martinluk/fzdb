#include "./entity_property.h"
#include <cstring>
#include <vector>
#include <iterator>
#include "./type_serialiser.h"

#include "./types/base.h"
#include "./types/string.h"
#include "./types/entity_ref.h"
#include "./types/int.h"
#include "./types/date.h"
#include <stdexcept>
#include "./database.h"

using BasePointer = std::shared_ptr<model::types::Base>;

EntityProperty::EntityProperty(Type type) : _type(type), _subtype(model::types::SubType::Undefined) {

}

EntityProperty::EntityProperty(Type type, const unsigned int& key, model::types::SubType subtype) :
  _type(type), _key(key), _subtype(subtype) {

}

EntityProperty::EntityProperty(Type type, const unsigned int& key, model::types::SubType subtype,
                               const std::vector<BasePointer> &values) : _type(type), _key(key), _subtype(subtype) {
  append(values);
}

EntityProperty::~EntityProperty() {
  clear();
}

bool EntityProperty::isNull() const {
  return _key == 0;
}

bool EntityProperty::isConcrete() const {
  if (_valuesList.size() != 1 ) return false;

  // The confidence must be 1.
  return _valuesList.front()->confidence() == 100;
}

bool EntityProperty::isEmpty() const {
  return _valuesList.begin() == _valuesList.end();
}

unsigned int EntityProperty::key() const {
  return _key;
}


std::size_t EntityProperty::count() const {
  return _valuesList.size();
}

void EntityProperty::append(BasePointer value) {
  if (value->subtype() != _subtype) {
    throw std::invalid_argument(std::string("Type ") + model::types::getSubString(value->subtype())
                                + std::string(" does not match property type ") + model::types::getSubString(_subtype));
  }

  switch (_type) {
  case Type::CONCRETEMULTI: {

    if (value->confidence() != 100) {
      throw std::runtime_error("The property requires a concrete value");
    }

    _valuesList.emplace_back(value);
    std::stable_sort(_valuesList.begin(), _valuesList.end(), model::types::ConfidenceCompare<model::types::Base>());
    unsigned int count = 0;
    for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
      (*iter)->OrderingId(count++);
    }
    break;
  }
  case Type::CONCRETESINGLE: {
    if (value->confidence() != 100) {
      throw std::runtime_error("The property requires a concrete value");
    }

    _valuesList.clear();
    value->OrderingId(0);
    _valuesList.emplace_back(value);
    break;
  }
  case Type::FUZZY: {

    _valuesList.emplace_back(value);
    std::stable_sort(_valuesList.begin(), _valuesList.end(), model::types::ConfidenceCompare<model::types::Base>());
    unsigned int count = 0;
    for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
      (*iter)->OrderingId(count++);
    }
    break;
  }

  case Type::LOCKED:
    throw std::runtime_error("Attempt to add to locked property");
  default:
    break;
  }

}

void EntityProperty::append(const std::vector<BasePointer> &list) {
  switch (_type) {
  case Type::FUZZY: {
    for (auto it = list.cbegin(); it != list.cend(); ++it) {
      model::types::SubType st = (*it)->subtype();
      if (st != _subtype) {
        throw std::invalid_argument(std::string("Type ") + model::types::getSubString(st)
                                    + std::string(" does not match property type ") + model::types::getSubString(_subtype));
      }
    }

    _valuesList.insert(_valuesList.end(), list.begin(), list.end());
    std::stable_sort(_valuesList.begin(), _valuesList.end(), model::types::ConfidenceCompare<model::types::Base>());
    unsigned int count = 0;
    for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
      (*iter)->OrderingId(count++);
    }
    break;
  }
  case Type::LOCKED: {
    throw std::runtime_error("Attempt to add to locked property");
  }
  default:
    break;
  }
}

void EntityProperty::clear() {
  _valuesList.clear();
}

const unsigned int& EntityProperty::keyRef() const {
  return _key;
}

BasePointer EntityProperty::baseValue(int index) const {
  auto iter = _valuesList.begin();
  for (int i = 0; i < index; i++) iter++;
  return *iter;
}

BasePointer EntityProperty::baseTop() const {
  return _valuesList.front();
}

std::vector<BasePointer> EntityProperty::baseValues() const {
  return _valuesList;
}


std::string EntityProperty::logString(const Database* db) const {
  using namespace model::types;

  std::string keyStr = std::to_string(_key);

  if (db) {
    auto strIt = db->entityManager().propertyNameMap().get(_key);
    keyStr = "\"" + strIt + "\"";
  }

  return std::string("EntityProperty<")
         + std::string(getSubString(_subtype))
         + std::string(">(k=")
         + keyStr
         + std::string(", [")
         + std::to_string(_valuesList.size())
         + std::string("])");
}

void EntityProperty::remove(const model::types::Base &value) {
  if (_type == Type::LOCKED) throw std::runtime_error("Attempt to remove locked property");
  _valuesList.erase(std::remove_if(_valuesList.begin(), _valuesList.end(), model::types::ValuesEqualOnly(&value)), _valuesList.end());
}

void EntityProperty::remove(const int orderingId) {
  //Removes entries whose orderingId matches the one in parameter.
  //Iterating over the _valuesList and check
  //TODO Potential optimisation: use the remove_if above, it looks so cool.
  std::vector<BasePointer>::iterator iter;
  for(iter=_valuesList.begin() ; iter!=_valuesList.end(); ++iter) {
    BasePointer e = *iter;
    if(e->OrderingId() == orderingId) {
      _valuesList.erase(iter);
      return;
    }
  }
}

bool EntityProperty::memberwiseEqual(const EntityProperty *other) const {
  if ( _key != other->_key ||
       _valuesList.size() != other->_valuesList.size() ||
       _subtype != other->_subtype )
    return false;

  auto thisIt = _valuesList.begin();
  auto otherIt = other->_valuesList.begin();

  while ( thisIt != _valuesList.end() ) {
    if ( otherIt == other->_valuesList.end() )
      return false;

    if ( !(*thisIt)->memberwiseEqual((*otherIt).get()) )
      return false;

    ++thisIt;
    ++otherIt;
  }

  return otherIt == other->_valuesList.end();
}

void EntityProperty::lock() {
  _type = Type::LOCKED;
}

std::shared_ptr<EntityProperty> EntityProperty::Clone() const {
  auto cloned = std::make_shared<EntityProperty>(_type);
  cloned->_key = _key;
  cloned->_subtype = _subtype;
  for (auto val : _valuesList) {
    cloned->_valuesList.emplace_back(val->Clone());
  }
  return cloned;
}

EntityProperty::Type EntityProperty::type() const {
  return _type;
}
