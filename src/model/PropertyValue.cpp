#include "./PropertyValue.h"
#include <algorithm>
#include <cstring>

PropertyValue::PropertyValue(const Variant &value, float confidence) :
	value_(value)
{
	setConfidenceClamp(confidence);
}

void PropertyValue::setConfidenceClamp(float f)
{
	// Clamp to make sure it's between 0 and 1.
	if ( f < 0.0f ) f = 0.0f;
	else if ( f > 1.0f ) f = 1.0f;

	confidence_ = f;
}

Variant PropertyValue::value() const
{
	return value_;
}

float PropertyValue::confidence() const
{
	return confidence_;
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
	propList.push_back(Serialiser::SerialProperty(&confidence_, sizeof(float)));

	// Serialise these.
	serialiser.serialise(propList);

	// Record the current size.
	std::size_t prevSize = serialiser.size();

	// Serialise the variant.
	value_.serialise(serialiser);

	// Calculate how large it was.
	header.valueSize = serialiser.size() - prevSize;

	// Write this back into the serialiser.
	// We calculate where the header would have been written.
	char* headerLoc = serialiser.data() + origSize;
	SerialHeader* pHeader = reinterpret_cast<SerialHeader*>(headerLoc);
	*pHeader = header;
}
