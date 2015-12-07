#include "./Variant.h"
#include <cassert>
#include <algorithm>
#include <cstring>
#include "../platform.h"

Variant::Variant()
{
	setSafeDefaults();
}

Variant::Variant(const Variant &other)
{
	setSafeDefaults();

	// Easiest way is to use our assignment operator.
	*this = other;
}

Variant::~Variant()
{
	// Clean up any dynamically allocated objects.
	cleanData();
}

Variant::Variant(int value)
{
	setSafeDefaults();
	set(value);
}

Variant::Variant(const std::string &value)
{
	setSafeDefaults();
	set(value);
}

bool Variant::operator ==(const Variant &other) const
{
	if ( type_ != other.type_ ) return false;

	switch (type_)
	{
		case UNDEFINED:
			return true;	// All undefined (null) variants are the same.

		case INTEGER:
			return dataEqual<int>(other);

		case STRING:
			return dataDereferenceEqual<std::string>(other);

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			return false;
	}
}

Variant& Variant::operator =(const Variant &other)
{
	cleanData();

	switch (other.type_)
	{
		case UNDEFINED:
			type_ = UNDEFINED;
			break;

		case INTEGER:
			set(other.getInteger());
			break;

		case STRING:
			set(other.getString());
			break;

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			break;
	}

	return *this;
}

void Variant::setSafeDefaults()
{
	// NOTE: Nothing is cleaned up with this method!
	type_ = UNDEFINED;
	data_ = NULL;
}

bool Variant::isNull() const
{
	return type_ == UNDEFINED;
}

Variant::Type Variant::type() const
{
	return type_;
}

void Variant::cleanData()
{
	// If we have allocated anything in the data pointer
	// for this variant, destroy it.
	
	switch (type_)
	{
		case UNDEFINED:
		case INTEGER:
			break;

		case STRING:
			delete static_cast<std::string*>(data_);
			break;

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			break;
	}
}

void Variant::set(int value)
{
	cleanData();

	// We can store an integer within the data pointer itself,
	// as an integer should be 4 bytes.
	assert(sizeof(void*) >= sizeof(int));

	int* i = reinterpret_cast<int*>(&data_);
	*i = value;
	type_ = INTEGER;
}

void Variant::set(const std::string &value)
{
	cleanData();
	
	// Allocate a string on the heap.
	data_ = static_cast<void*>(new std::string(value));
	type_ = STRING;
}

int Variant::getInteger(bool* ok) const
{
	if ( type_ != INTEGER )
	{
		if (ok) *ok = false;
		return 0;
	}

	if (ok) *ok = true;

#if defined(ENVIRONMENT32)
	return reinterpret_cast<int>(data_);
#elif defined(ENVIRONMENT64)
	long long ret = reinterpret_cast<long long>(data_);
	return static_cast<int>(ret);
#else
	assert(false);
	return 0;
#endif
}

std::string Variant::getString(bool* ok) const
{
	if ( type_ != STRING )
	{
		if (ok) *ok = false;
		return std::string();
	}

	if (ok) *ok = true;
	return *(static_cast<std::string*>(data_));
}

template <typename T>
bool Variant::dataEqual(const Variant &other) const
{
	return *(reinterpret_cast<const T*>(&data_)) == *(reinterpret_cast<const T*>(&other.data_));
}

template <typename T>
bool Variant::dataDereferenceEqual(const Variant &other) const
{
	return *(reinterpret_cast<const T*>(data_)) == *(reinterpret_cast<const T*>(other.data_));
}

std::size_t Variant::internalDataSize() const
{
	switch (type_)
	{
		case UNDEFINED:
			return 0;

		case INTEGER:
			return sizeof(int);

		case STRING:
			return static_cast<std::string*>(data_)->size();

		default:
			// Someone's added a new type and not updated this!
			assert(false);
			return 0;
	}
}

void Variant::serialise(Serialiser &serialiser) const
{
	// Serialisation format:
	// + SerialHeader
	// - Type
	// - Data
	
	std::vector<Serialiser::SerialProperty> propList;

	// Create a header.
	SerialHeader header;
	header.dataSize = internalDataSize();

	// Add header and type informaion to list.
	propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
	propList.push_back(Serialiser::SerialProperty(&type_, sizeof(Type)));

	// Add our actual data, depending on what it is.
	switch (type_)
	{
	// Non-dereference types: pass a pointer to our pointer.
	case UNDEFINED:
	case INTEGER:
		propList.push_back(Serialiser::SerialProperty(reinterpret_cast<const void*>(&data_),
			sizeof(const void*)));
		break;

	// Dereference types: pass the pointer itself.
	case STRING:
		propList.push_back(Serialiser::SerialProperty(static_cast<std::string*>(data_)->c_str(), header.dataSize));
		break;

	default:
		// Someone's added a new type and not updated this!
		assert(false);
		break;
	}

	// Pass all the properties to the serialiser.
	serialiser.serialise(propList);
}
