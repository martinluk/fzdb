#include "./EntityProperty.h"
#include <cstring>
#include <vector>
#include <iterator>
#include "TypeSerialiser.h"

#include "./types/Base.h"
#include "./types/String.h"
#include "./types/EntityRef.h"
#include "./types/Int.h"
#include "./types/Date.h"
#include <stdexcept>
#include "Database.h"

using BasePointer = std::shared_ptr<model::types::Base>;

EntityProperty::EntityProperty() : _subtype(model::types::SubType::TypeUndefined)
{
	_count = 0;
}

EntityProperty::EntityProperty(const unsigned int& key, model::types::SubType subtype) :
	_key(key), _subtype(subtype)
{
	_count = 0;
}

EntityProperty::EntityProperty(const unsigned int& key, model::types::SubType subtype,
	const std::vector<BasePointer> &values) : _key(key), _subtype(subtype)
{
	_count = 0;
    append(values);
}

EntityProperty::~EntityProperty()
{
	clear();
}

bool EntityProperty::isNull() const
{
	return _key == 0;
}

bool EntityProperty::isConcrete() const 
{
	if ( _count != 1 ) return false;

	// The confidence must be 1.
	return _valuesList.front()->confidence() == 100;
}

bool EntityProperty::isEmpty() const
{
	return _valuesList.begin() == _valuesList.end();
}

unsigned int EntityProperty::key() const
{
	return _key;
}


unsigned int EntityProperty::count() const
{
	return _count;
}

void EntityProperty::append(BasePointer value)
{
	if (value->subtype() != _subtype)
	{
		throw std::invalid_argument(std::string("Type ") + model::types::getSubTypeString(value->subtype())
			+ std::string(" does not match property type ") + model::types::getSubTypeString(_subtype));
	}

	_count += 1;
	_valuesList.emplace_front(value);
	_valuesList.sort(model::types::ConfidenceCompare<model::types::Base>());
	unsigned int count = 0;
	for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
		(*iter)->OrderingId(count++);
	}
}

void EntityProperty::append(const std::vector<BasePointer> &list)
{
	for (auto it = list.cbegin(); it != list.cend(); ++it)
	{
		model::types::SubType st = (*it)->subtype();
		if (st != _subtype)
		{
			throw std::invalid_argument(std::string("Type ") + model::types::getSubTypeString(st)
				+ std::string(" does not match property type ") + model::types::getSubTypeString(_subtype));
		}
	}

	_count += list.size();
	_valuesList.insert_after(_valuesList.cbefore_begin(), list.begin(), list.end());
	_valuesList.sort(model::types::ConfidenceCompare<model::types::Base>());
	unsigned int count = 0;
	for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
		(*iter)->OrderingId(count++);
	}
}

void EntityProperty::clear()
{
	_count = 0;
	_valuesList.clear();
}

const unsigned int& EntityProperty::keyRef() const
{
	return _key;
}

BasePointer EntityProperty::baseValue(int index) const
{
	auto iter = _valuesList.begin();
	for (int i = 0; i < index; i++) iter++;
	return *iter;
}

BasePointer EntityProperty::baseTop() const
{
	return _valuesList.front();
}

std::vector<BasePointer> EntityProperty::baseValues() const
{
	std::vector<BasePointer> output;
	std::transform(_valuesList.begin(), _valuesList.end(), std::inserter(output, output.begin()), [&](BasePointer input) {
		return input;
	});
	return output;
}


std::string EntityProperty::logString(const Database* db) const
{
	using namespace model::types;

	std::string keyStr = std::to_string(_key);

	if (db)
	{
		auto strIt = db->entityManager().propertyNameMap().right.find(_key);
		if (strIt != db->entityManager().propertyNameMap().right.end())
			keyStr = "\"" + strIt->second + "\"";
	}

    return std::string("EntityProperty<")
            + std::string(getSubTypeString(_subtype))
            + std::string(">(k=")
            + keyStr
            + std::string(", [")
            + std::to_string(_count)
            + std::string("])");
}

void EntityProperty::remove(const model::types::Base &value)
{
	_valuesList.remove_if(model::types::ValuesEqualOnly(&value));
	_count = std::distance(_valuesList.cbegin(), _valuesList.cend());
}