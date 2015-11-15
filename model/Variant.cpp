#include "./Variant.h"
#include <cassert>

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
			_type = UNDEFINED;
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
			delete data_;
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
	assert(sizeof(void) >= sizeof(int));

	int* i = static_cast<int*>(&data_);
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

int getInteger(bool* ok)
{
	if ( type_ != INTEGER )
	{
		if (ok) *ok = false;
		return 0;
	}

	if (ok) *ok = true;
	return static_cast<int>(data_);
}

std::string getString(bool* ok)
{
	if ( type_ != STRING )
	{
		if (ok) *ok = false;
		return std::string();
	}

	if (ok) *ok = true;
	return *(static_cast<std::string*>(data_));
}

temaplate <typename T>
bool Variant::dataEqual(const Variant &other) const
{
	return *(static_cast<T*>(&data_)) == *(static_cast<T*>(&other.data_));
}

template <typename T>
bool Variant::dataDereferenceEqual(const Variant &other) const
{
	return *(static_cast<T*>(data_)) == *(static_cast<T*>(other.data_));
}
