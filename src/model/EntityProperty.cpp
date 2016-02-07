#include "./EntityProperty.h"
#include <cstring>
#include <vector>
#include <iterator>
#include "TypeSerialiser.h"

using BasePointer = std::shared_ptr<model::types::Base>;

template <typename T>
void EntityProperty<T>::initSubtype()
{
	BasePointer b = std::make_shared<T>();
    _subtype = b->subtype();
}

template <typename T>
EntityProperty<T>::EntityProperty()
{
    initSubtype();
}

template <typename T>
EntityProperty<T>::EntityProperty(const unsigned int& key) :
	_key(key)
{
    initSubtype();
}

template <typename T>
EntityProperty<T>::EntityProperty(const unsigned int& key,
	const std::vector<std::shared_ptr<T>> &values) : _key(key)
{
    initSubtype();
    append(values);
}

template <typename T>
EntityProperty<T>::~EntityProperty()
{
    deleteAllValues();
}

template <typename T>
bool EntityProperty<T>::isNull() const
{
	return _key == 0;
}

template <typename T>
bool EntityProperty<T>::isConcrete() const 
{
	if ( _values.size() != 1 ) return false;

	// The confidence must be 1.
	return _values[0]->confidence() == 100;
}

template <typename T>
bool EntityProperty<T>::isEmpty() const
{
	return _values.size() == 0;
}

template <typename T>
unsigned int EntityProperty<T>::key() const
{
	return _key;
}

template <typename T>
std::vector<std::shared_ptr<T>> EntityProperty<T>::values() const
{
	return _values;
}

template <typename T>
int EntityProperty<T>::count() const
{
	return _values.size();
}

template <typename T>
void EntityProperty<T>::append(std::shared_ptr<T> value)
{
	_values.emplace_back(value);
}

template <typename T>
void EntityProperty<T>::append(const std::vector<std::shared_ptr<T>> &list)
{
	_values.insert(_values.begin(), list.cbegin(), list.cend());
}

template <typename T>
void EntityProperty<T>::clear()
{
    deleteAllValues();
    _values.clear();
}

template <typename T>
std::shared_ptr<T> const& EntityProperty<T>::value(int index) const
{
	return _values[index];
}

template <typename T>
const unsigned int& EntityProperty<T>::keyRef() const
{
	return _key;
}

template <typename T>
void EntityProperty<T>::deleteAllValues()
{
	_values.clear();
}

template <typename T>
BasePointer EntityProperty<T>::baseValue(int index) const
{
	return std::dynamic_pointer_cast<model::types::Base, T>(_values[index]);
}

template <typename T>
std::vector<BasePointer> EntityProperty<T>::baseValues() const
{
	std::vector<BasePointer> output;
	std::transform(_values.begin(), _values.end(), std::inserter(output, output.begin()), [&](std::shared_ptr<T> input) {
		return std::static_pointer_cast<model::types::Base, T>(input);
	});
	return output;
}


template<typename T>
std::string EntityProperty<T>::logString() const
{
    return std::string("EntityProperty<")
            //+ std::string(model::types::Base::SubtypeString[(int)_subtype])
            + std::string(model::types::Base::SubtypeString(_subtype))
            + std::string(">(k=")
            + std::to_string(_key)
            + std::string(", [")
            + std::to_string(_values.size())
            + std::string("])");
}

// Template instantiations - parameters must be listed here in order to link
template class EntityProperty < model::types::String >;
template class EntityProperty < model::types::EntityRef >;
template class EntityProperty < model::types::Int >;
