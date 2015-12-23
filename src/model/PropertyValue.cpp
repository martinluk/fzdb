#include "./PropertyValue.h"
#include <algorithm>
#include <cstring>

PropertyValue::PropertyValue(const Variant &value, signed char confidence) :
	_value(value)
{
	setConfidenceClamp(confidence);
}

void PropertyValue::setConfidenceClamp(float f)
{
	// Clamp to make sure it's between 0 and 1.
	if ( f < 0 ) f = 0;
	else if ( f > 100 ) f = 100;

	_confidence = f;
}

Variant PropertyValue::value() const
{
	return _value;
}

float PropertyValue::confidence() const
{
	return _confidence;
}

void PropertyValue::serialise(Serialiser &serialiser) const
{
	// The serialised structure is as follows:
	// + SerialHeader struct
	// - Confidence
	// - Value
	
	std::vector<Serialiser::SerialProperty> propList;

	// Create a header.
	SerialHeader header;

	// We don't know the size yet, so set to zero.
	header.valueSize = 0;

	// Keep the size that the serialiser was before we added anything to it.
	std::size_t origSize = serialiser.size();

	// Prepare our properties.
	propList.push_back(Serialiser::SerialProperty(&header, sizeof(SerialHeader)));
	propList.push_back(Serialiser::SerialProperty(&_confidence, sizeof(float)));

	// Serialise these.
	serialiser.serialise(propList);

	// Record the current size.
	std::size_t prevSize = serialiser.size();

	// Serialise the variant.
	_value.serialise(serialiser);

	// Calculate how large it was.
	header.valueSize = serialiser.size() - prevSize;

	// Write this back into the serialiser.
	// We calculate where the header would have been written.
	char* headerLoc = serialiser.data() + origSize;
	SerialHeader* pHeader = reinterpret_cast<SerialHeader*>(headerLoc);
	*pHeader = header;
}

PropertyValue PropertyValue::unserialise(const char* data)
{
	// + SerialHeader struct
	// - Confidence
	// - Value

	// Firstly get the header.
	const SerialHeader* pHeader =
		reinterpret_cast<const SerialHeader*>(data);

	// Read back the confidence value.
	const float* pConfidence = reinterpret_cast<const float*>(data +
		sizeof(SerialHeader));

	// Get a pointer to the variant data.
	const char* vData = reinterpret_cast<const char*>(data +
		sizeof(SerialHeader) + sizeof(float));

	// Return everything at once!
	return PropertyValue(Variant::unserialise(vData), *pConfidence);
}
