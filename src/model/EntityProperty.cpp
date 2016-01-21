#include "./EntityProperty.h"
#include <cstring>
#include <vector>
#include "TypeSerialiser.h"

template <typename T>
void EntityProperty<T>::initSubtype()
{
    using namespace model::types;
    Base* b = new T();
    _subtype = b->subtype();
    delete b;
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
	const std::vector<T*> &values) : _key(key)
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
std::vector<T*> EntityProperty<T>::values() const
{
	return _values;
}

template <typename T>
int EntityProperty<T>::count() const
{
	return _values.size();
}

template <typename T>
void EntityProperty<T>::append(T* value)
{
	_values.emplace_back(value);
}

template <typename T>
void EntityProperty<T>::append(const std::vector<T*> &list)
{
	for ( int i = 0; i < list.size(); i++ )
	{
		append(list[i]);
	}
}

template <typename T>
void EntityProperty<T>::clear()
{
    deleteAllValues();
    _values.clear();
}

template <typename T>
T* EntityProperty<T>::value(int index) const
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
    for ( int i = 0; i < _values.size(); i++ )
    {
        delete _values[i];
    }
}

template <typename T>
model::types::Base* EntityProperty<T>::baseValue(int index) const
{
    return dynamic_cast<model::types::Base*>(value(index));
}

// Template instantiations - parameters must be listed here in order to link
template class EntityProperty < model::types::String >;
template class EntityProperty < model::types::EntityRef >;
template class EntityProperty < model::types::Int >;
