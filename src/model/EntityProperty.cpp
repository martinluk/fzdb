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

using BasePointer = std::shared_ptr<model::types::Base>;

void EntityProperty::initSubtype()
{
	BasePointer b = std::make_shared<model::types::Base>();
    _subtype = b->subtype();
}

EntityProperty::EntityProperty()
{
	_count = 0;
    initSubtype();
}

EntityProperty::EntityProperty(const unsigned int& key) :
	_key(key) 
{
	_count = 0;
	initSubtype();
}

EntityProperty::EntityProperty(const unsigned int& key,
	const std::vector<BasePointer> &values) : _key(key)
{
	_count = 0;
	initSubtype();
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


std::string EntityProperty::logString() const
{
    return std::string("EntityProperty<")
            //+ std::string(model::types::SubTypeString[(int)_subtype])
		//TODO: WHAT SHOULD THIS BE??
           // + std::string(model::types::String[(int)_subtype]))
            + std::string(">(k=")
            + std::to_string(_key)
            + std::string(", [")
            + std::to_string(_count)
            + std::string("])");
}

void EntityProperty::remove(const model::types::Base &value)
{
	_valuesList.remove_if(model::types::ValuesEqualOnly(&value));
	_count = std::distance(_valuesList.cbegin(), _valuesList.cend());
}