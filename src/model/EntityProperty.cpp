#include "./EntityProperty.h"

EntityProperty::EntityProperty()
{
	// Nothing to do - key will automatically be empty.
}

bool EntityProperty::isNull() const
{
	return key_.empty();
}

bool EntityProperty::isConcrete() const
{
	if ( values_.size() != 1 ) return false;

	// The confidence must be 1.
	return values_[0].confidence() == 1.0f;
}

bool EntityProperty::isEmpty() const
{
	return values_.size() == 0;
}

std::string EntityProperty::key() const
{
	return key_;
}

std::vector<PropertyValue> EntityProperty::values() const
{
	return values_;
}

int EntityProperty::count() const
{
	return values_.size();
}

Variant EntityProperty::concreteValue() const
{
	return isConcrete() ? values_[0].value() : Variant();
}

void EntityProperty::append(const PropertyValue &value)
{
	values_.push_back(value);
}

void EntityProperty::append(const std::vector<PropertyValue> &list)
{
	for ( int i = 0; i < list.size(); i++ )
	{
		append(list[i]);
	}
}

void EntityProperty::setConcrete(const Variant &value)
{
	clear();
	append(PropertyValue(value, 1.0f));
}

void EntityProperty::clear()
{
	values_.clear();
}

PropertyValue EntityProperty::value(int index) const
{
	return values_[index];
}

const std::string& EntityProperty::keyRef() const
{
	return key_;
}
