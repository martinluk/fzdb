#ifndef MODEL_VARIANT_H
#define MODEL_VARIANT_H

#include <string>
#include "./ISerialisable.h"

// Class that can store one of multiple different value types. Used within properties.
// Arbitrary data is stored within the data_ void pointer. If the physical value fits
// within the number of bytes the pointer itself takes up (usually 4 bytes), the
// value is stored by blindly casting the pointer to that type. If this is not the
// case, an object of the correct type is allocated on the heap and the data pointer
// records its address.

// TODO: We may want this class to be implicitly shared, so that we can return
// variants without having to perform deep copies.
class Variant : public ISerialisable
{
	public:
		enum Type
		{
			UNDEFINED = 0,	// Implies null variant and that internal contents are undefined!
			INTEGER,
			STRING
		};

		// Constructs a null variant.
		Variant();

		// Destroys the variant.
		~Variant();

		// We need to implement a deep copy because
		// some things get allocated on the heap.
		Variant(const Variant &other);

		// Same for equality: the objects on the heap must be equal.
		// They do not have to be physically the same object in memory.
		bool operator ==(const Variant &other) const;

		// Also need custom assignment operator.
		Variant& operator =(const Variant &other);

		// Convenience constructors for value types.
		Variant(int value);
		Variant(const std::string &value);

		// Returns whether the variant is null.
		// Internally, null variants are represented by an undefined type.
		bool isNull() const;

		// Returns the type of the variant.
		Type type() const;

		// Setters for values.
		void set(int value);
		void set(const std::string &value);

		// Getters for values.
		// If the type does not match, a default-constructed
		// value is returned and ok (if supplied) is set to false.
		int getInteger(bool* ok = NULL) const;
		std::string getString(bool* ok = NULL) const;

		// Implementation of ISerialisable.
		virtual void serialise(Serialiser &serialiser) const override;

	private:
		struct SerialHeader
		{
			std::size_t dataSize;
		};

		void cleanData();
		void setSafeDefaults();
		std::size_t internalDataSize() const;

		// Here we assume the types between the two check objects are equal.
		
		// Check that the data pointers themselves are equal.
		template <typename T>
		bool dataEqual(const Variant &other) const;
		
		// Check that the objects the data pointers point to are equal.
		template <typename T>
		bool dataDereferenceEqual(const Variant &other) const;

		Type type_;
		void* data_;
};

#endif	// MODEL_VARIANT_H
