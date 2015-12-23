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
	char* buffer = NULL;
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
	{
		// Optimisation: for the string, we copy in the null terminator
		// as well. This means it's more efficient to read back a variant
		// once it's been serialised, as we can just create a string out of
		// the char pointer pointing to the serialised data.
		header.dataSize++;	// Increment for the terminator.
		buffer = new char[header.dataSize];
		std::string* dataString = static_cast<std::string*>(data_);
		memcpy(buffer, dataString->c_str(), header.dataSize);
		buffer[header.dataSize-1] = '\0';	// Terminate!
		propList.push_back(Serialiser::SerialProperty(buffer, header.dataSize));
		break;
	}

	default:
		// Someone's added a new type and not updated this!
		assert(false);
		break;
	}

	// Pass all the properties to the serialiser.
	serialiser.serialise(propList);

	delete[] buffer;
}

Variant Variant::unserialise(const char* data)
{
	// Serialisation format:
	// + SerialHeader
	// - Type
	// - Data

	// Read back the data as a header.
	const SerialHeader* pHeader =
		reinterpret_cast<const SerialHeader*>(data);

	// Get the type.
	const Type* pType =
		reinterpret_cast<const Type*>(data
		+ sizeof(SerialHeader));

	// Switch on the type.
	switch (*pType)
	{
		case UNDEFINED:
		{
			// Return a null variant.
			return Variant();
		}

		case INTEGER:
		{
			// Interpret the rest of the data as a raw integer.
			const int* pData =
				reinterpret_cast<const int*>(data + sizeof(Variant::SerialHeader)
				+ sizeof(Variant::Type));
			return Variant(*pData);
		}

		case STRING:
		{
			// This is a pointer to the null-terminated char data.
			const char* sData =
				reinterpret_cast<const char*>(data + sizeof(Variant::SerialHeader)
				+ sizeof(Variant::Type));
			return Variant(std::string(sData));
		}	

		default:
		{
			// Someone's added a new type and not updated this!
			assert(false);
			return Variant();
		}
	}
}
