#include "./EntityProperty.h"
#include <cstring>
#include <vector>
#include <iterator>
#include "TypeSerialiser.h"

using BasePointer = std::shared_ptr<model::types::Base>;

//template <typename T>
//EntityProperty<T>::cmp = [](int left, int right) { return left < right;  };

template <typename T>
void EntityProperty<T>::initSubtype()
{
	BasePointer b = std::make_shared<T>();
    _subtype = b->subtype();
}

template <typename T>
EntityProperty<T>::EntityProperty()
{
	_count = 0;
    initSubtype();
}

template <typename T>
EntityProperty<T>::EntityProperty(const unsigned int& key) :
	_key(key) 
{
	_count = 0;
	initSubtype();
}

template <typename T>
EntityProperty<T>::EntityProperty(const unsigned int& key,
	const std::vector<std::shared_ptr<T>> &values) : _key(key)
{
	_count = 0;
	initSubtype();
    append(values);
}

template <typename T>
EntityProperty<T>::~EntityProperty()
{
	clear();
}

template <typename T>
bool EntityProperty<T>::isNull() const
{
	return _key == 0;
}

template <typename T>
bool EntityProperty<T>::isConcrete() const 
{
	if ( _count != 1 ) return false;

	// The confidence must be 1.
	return _valuesList.front()->confidence() == 100;
}

template <typename T>
bool EntityProperty<T>::isEmpty() const
{
	return _valuesList.begin() == _valuesList.end();
}

template <typename T>
unsigned int EntityProperty<T>::key() const
{
	return _key;
}

template <typename T>
std::vector<std::shared_ptr<T>> EntityProperty<T>::values() const
{
	std::vector<std::shared_ptr<T>> outVector;
	outVector.insert(outVector.begin(), _valuesList.begin(), _valuesList.end());
	return outVector;
}

template <typename T>
std::shared_ptr<T> EntityProperty<T>::top() const
{
	return _valuesList.front();
}

template <typename T>
unsigned int EntityProperty<T>::count() const
{
	return _count;
}

template <typename T>
void EntityProperty<T>::append(std::shared_ptr<T> value)
{
	_count += 1;
	_valuesList.emplace_front(value);
	_valuesList.sort(model::types::ConfidenceCompare<T>());
	unsigned int count = 0;
	for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
		(*iter)->OrderingId(count++);
	}
}

template <typename T>
void EntityProperty<T>::append(const std::vector<std::shared_ptr<T>> &list)
{
	_count += list.size();
	_valuesList.insert_after(_valuesList.cbefore_begin(), list.begin(), list.end());
	_valuesList.sort(model::types::ConfidenceCompare<T>());
	unsigned int count = 0;
	for (auto iter = _valuesList.begin(); iter != _valuesList.end(); ++iter) {
		(*iter)->OrderingId(count++);
	}
}

template <typename T>
void EntityProperty<T>::clear()
{
	_count = 0;
	_valuesList.clear();
}

template <typename T>
std::shared_ptr<T> const& EntityProperty<T>::value(int index) const
{
	auto iter = _valuesList.begin();
	for (int i = 0; i < index; i++) iter++;
	return *iter;
}

template <typename T>
const unsigned int& EntityProperty<T>::keyRef() const
{
	return _key;
}

template <typename T>
BasePointer EntityProperty<T>::baseValue(int index) const
{
	auto iter = _valuesList.begin();
	for (int i = 0; i < index; i++) iter++;
	return std::dynamic_pointer_cast<model::types::Base, T>(*iter);
}

template <typename T>
BasePointer EntityProperty<T>::baseTop() const
{
	return std::dynamic_pointer_cast<model::types::Base, T>(_valuesList.front());
}

template <typename T>
std::vector<BasePointer> EntityProperty<T>::baseValues() const
{
	std::vector<BasePointer> output;
	std::transform(_valuesList.begin(), _valuesList.end(), std::inserter(output, output.begin()), [&](std::shared_ptr<T> input) {
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
            + std::to_string(_count)
            + std::string("])");
}

// Template instantiations - parameters must be listed here in order to link
template class EntityProperty < model::types::String >;
template class EntityProperty < model::types::EntityRef >;
template class EntityProperty < model::types::Int >;
template class EntityProperty < model::types::Date >;
