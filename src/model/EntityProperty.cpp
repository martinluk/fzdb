#include "./EntityProperty.h"
#include <cstring>
#include <vector>

template <typename T>
EntityProperty<T>::EntityProperty()
{
	// Nothing to do - key will automatically be empty.
}

template <typename T>
EntityProperty<T>::EntityProperty(const unsigned int& key) :
	_key(key)
{

}

template <typename T>
EntityProperty<T>::EntityProperty(const unsigned int& key,
        const std::vector<T*> &values) : _key(key)
{
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
	_values.push_back(value);
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

// Template instantiations - parameters must be listed here in order to link
template class EntityProperty < model::types::String >;
template class EntityProperty < model::types::EntityRef >;
template class EntityProperty < model::types::Int >;
