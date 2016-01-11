#include "./EntityProperty.h"
#include <cstring>
#include <vector>

EntityProperty::EntityProperty()
{
	// Nothing to do - key will automatically be empty.
}

EntityProperty::EntityProperty(const std::string &key) :
	key_(key)
{

}

EntityProperty::EntityProperty(const std::string &key,
	const std::vector<PropertyValue> &values) : key_(key)
{
	append(values);
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

void EntityProperty::serialise(Serialiser &serialiser) const
{
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
	header.keySize = key_.size()+1;
	header.valueCount = values_.size();
	header.totalSize = 0;	// We don't yet know this.

	// Add this to the list.
	propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));

	// Make a note of how large the serialiser was - we'll use this to find
	// the header location later.
	std::size_t origSize = serialiser.size();

	// We need to add a load of null ValueHeaderItems to the list too.
	// All these will get updated as we go along.
	std::size_t numValues = values_.size();
	ValueHeaderItem vHeader;
	vHeader.size = 0;
	for (std::size_t i = 0; i < numValues; i++)
	{
		propList.push_back(Serialiser::SerialProperty(&vHeader, sizeof(ValueHeaderItem)));
	}

	// Also add our key string.
	std::vector<char> strBuffer(header.keySize, NULL);
	memcpy(strBuffer.data(), key_.c_str(), header.keySize-1);
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
		values_[i].serialise(serialiser);

		// See how large the serialiser now is.
		std::size_t sizeAfter = serialiser.size();

		// Calculate how large the value was.
		std::size_t valueSize = sizeAfter - sizeBefore;

		// Record this.
		pVHeader[i].size = valueSize;
		pHeader->totalSize += valueSize;
	}
}
EntityProperty EntityProperty::unserialise(const char* data)
{
	// Serialisation format:
	// + SerialHeader
	// + ValueHeaderItems
	// + ...
	// - Key
	// - PropertyValues
	// - ...

	// Firstly get the header.
	const SerialHeader* pHeader =
		reinterpret_cast<const SerialHeader*>(data);
	
	// Get a pointer to the first ValueHeaderItem.
	const ValueHeaderItem* pVHI =
		reinterpret_cast<const ValueHeaderItem*>(data + sizeof(SerialHeader));
	
	// Get a pointer to the key.
	const char* pKey =
		reinterpret_cast<const char*>(data + sizeof(SerialHeader) +
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
	return EntityProperty(std::string(pKey), pvList);
}
