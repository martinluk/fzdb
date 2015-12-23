#ifndef MODEL_SERIALISER_H
#define MODEL_SERIALISER_H

#include <utility>
#include <vector>

class Serialiser
{
public:
	// Specifies a property to serialise.
	// Pointer points to the data, size specifies how many bytes.
	typedef std::pair<const void*,std::size_t> SerialProperty;

	Serialiser();

	// Serialise the given list of properties.
	void serialise(const std::vector<SerialProperty> &properties);

	// Clear all data.
	void clear();

	// Returns a pointer to the beginning of the data.
	char* data();
	const char* cdata() const;

	// Returns the size of the data.
	std::size_t size() const;

private:
	// Vector that holds the actual serialised data.
	std::vector<char> serialData_;
};

#endif	// MODEL_SERIALISER_H
