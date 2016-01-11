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
	const std::vector<T> &values) : _key(key)
{
	append(values);
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
	return _values[0].confidence() == 100;
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
std::vector<T> EntityProperty<T>::values() const
{
	return _values;
}

template <typename T>
int EntityProperty<T>::count() const
{
	return _values.size();
}

//template <typename T>
//Variant EntityProperty<T>::concreteValue() const
//{
//	return isConcrete() ? _values[0].value() : Variant();
//}

template <typename T>
void EntityProperty<T>::append(const T &value)
{
	_values.push_back(value);
}

template <typename T>
void EntityProperty<T>::append(const std::vector<T> &list)
{
	for ( int i = 0; i < list.size(); i++ )
	{
		append(list[i]);
	}
}

//template <typename T>
//void EntityProperty<T>::setConcrete(const Variant &value)
//{
//	clear();
//	append(PropertyValue(value, 255));
//}

template <typename T>
void EntityProperty<T>::clear()
{
	_values.clear();
}

template <typename T>
T EntityProperty<T>::value(int index) const
{
	return _values[index];
}

//template <typename T>
//bool EntityProperty<T>::containsValue(T value)
//{
//	for (auto iter = _values.cbegin(); iter != _values.cend(); iter++) {
//		if (iter->value().getString() == value) {
//			return true;
//		}
//	}
//	return false;
//}

template <typename T>
const unsigned int& EntityProperty<T>::keyRef() const
{
	return _key;
}

template <typename T>
void EntityProperty<T>::serialise(Serialiser &serialiser) const
{
	/*
	// Serialisation format:
	// + SerialHeader
	// + ValueHeaderItems
	// + ...
	// - Key
	// - PropertyValues
	// - ...
	
	std::vector<Serialiser::SerialProperty> propList;
	
	// Construct a header.
	SerialHeader header;
	header.keySize = sizeof(unsigned int);
	header.valueCount = _values.size();
	header.totalSize = 0;	// We don't yet know this.

	// Add this to the list.
	propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

	// Make a note of how large the serialiser was - we'll use this to find
	// the header location later.
	std::size_t origSize = serialiser.size();

	// We need to add a load of null ValueHeaderItems to the list too.
	// All these will get updated as we go along.
	std::size_t numValues = _values.size();
	ValueHeaderItem vHeader;
	vHeader.size = 0;
	for (std::size_t i = 0; i < numValues; i++)
	{
		propList.push_back(Serialiser::SerialProperty(&vHeader, sizeof(ValueHeaderItem)));
	}

	// Also add our key string.
	std::vector<char> strBuffer(header.keySize, NULL);
	memcpy(strBuffer.data(), (void*)_key, header.keySize);
	propList.push_back(Serialiser::SerialProperty(strBuffer.data(),
		header.keySize));

	// Serialise all the things we have so far.
	serialiser.serialise(propList);

	// Now get a pointer to our original header.
	char* serStart = serialiser.data() + origSize;
	SerialHeader* pHeader = reinterpret_cast<SerialHeader*>(serStart);
	ValueHeaderItem* pVHeader = reinterpret_cast<ValueHeaderItem*>(serStart + sizeof(SerialHeader));
	
	// Initialise the total size to the size of the data so far.
	pHeader->totalSize = sizeof(SerialHeader) + (numValues * sizeof(ValueHeaderItem))
		+ pHeader->keySize;

	// For each of our property values:
	for (int i = 0; i < numValues; i++)
	{
		// Record how large the serialiser is at first.
		std::size_t sizeBefore = serialiser.size();

		// Serialise the PropertyValue.
		_values[i].serialise(serialiser);

		// See how large the serialiser now is.
		std::size_t sizeAfter = serialiser.size();

		// Calculate how large the value was.
		std::size_t valueSize = sizeAfter - sizeBefore;

		// Record this.
		pVHeader[i].size = valueSize;
		pHeader->totalSize += valueSize;
	}*/
}

template <typename T>
EntityProperty<T> EntityProperty<T>::unserialise(const char* data)
{
	// Serialisation format:
	// + SerialHeader
	// + ValueHeaderItems
	// + ...
	// - Key
	// - PropertyValues
	// - ...
	/*
	// Firstly get the header.
	const SerialHeader* pHeader =
		reinterpret_cast<const SerialHeader*>(data);
	
	// Get a pointer to the first ValueHeaderItem.
	const ValueHeaderItem* pVHI =
		reinterpret_cast<const ValueHeaderItem*>(data + sizeof(SerialHeader));
	
	// Get a pointer to the key.
	const unsigned int pKey =
		reinterpret_cast<unsigned int>(data + sizeof(SerialHeader) +
		(pHeader->valueCount * sizeof(ValueHeaderItem)));
	
	// Get a pointer to the actual data.
	const char* pData =
		reinterpret_cast<const char*>(pKey + pHeader->keySize);

	// Create a vector to fill with values.
	std::vector<PropertyValue> pvList;

	// Get all the values!
	for (int i = 0; i < pHeader->valueCount; i++)
	{
		// Unserialise with the data pointed to.
		pvList.push_back(PropertyValue::unserialise(pData));

		// Advance the pointer by the correct number of bytes.
		pData += pVHI[i].size;
	}

	// Return the property.
	return EntityProperty((unsigned int) pKey, pvList);*/
	return EntityProperty<T>();
}

//template instantiations - parameters must be listed here in order to link
template class EntityProperty < model::types::String >;
template class EntityProperty < model::types::EntityRef >;
template class EntityProperty < model::types::Int >;