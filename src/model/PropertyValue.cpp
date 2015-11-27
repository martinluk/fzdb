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

std::pair<std::size_t,bool> PropertyValue::serialise(char* buffer, std::size_t maxSize) const
{
	// The serialised structure is as follows:
	// + SerialHeader struct
	// - Confidence
	// - Value

	// If buffer is zero, just return how large we need to be.
	std::size_t valueSize = value_.serialise(NULL, 0).first;
	std::size_t bytesRequired = sizeof(SerialHeader) + valueSize + sizeof(float);
	if ( !buffer )
	{
		return std::pair<std::size_t,bool>(bytesRequired, false);
	}

	// Work out how much we're allowed to write.
	std::size_t bytesToWrite = std::min(bytesRequired, maxSize);
	bool canWriteAll = true;
	if ( bytesToWrite < bytesRequired ) canWriteAll = false;

	// If zero, return.
	if ( bytesToWrite < 1 )
		return std::pair<std::size_t,bool>(0, canWriteAll);

	// Create a struct for serialisation.
	SerialHeader header;
	header.valueSize = valueSize;

	// Write as much of the struct as we can.
	std::size_t leftToWrite = bytesToWrite;
	std::size_t written = 0;
	std::size_t toWriteNow = std::min(sizeof(SerialHeader), leftToWrite);
	memcpy(buffer, &header, toWriteNow);
	leftToWrite -= toWriteNow;
	written += toWriteNow;
	
	// If we can't write any more, exit.
	if ( leftToWrite < 1 )
	{
		return std::pair<std::size_t,bool>(written, canWriteAll);
	}

	toWriteNow = std::min(sizeof(float), leftToWrite);
	memcpy(buffer + written, &confidence_, toWriteNow);
	leftToWrite -= toWriteNow;
	written += toWriteNow;

	// If we can't write any more, exit.
	if ( leftToWrite < 1 )
	{
		return std::pair<std::size_t,bool>(written, canWriteAll);
	}

	// Have the variant write its own data and get the result.
	std::pair<size_t,bool> result = value_.serialise(buffer + written, leftToWrite);
	
	written += result.first;
	return std::pair<std::size_t,bool>(written, canWriteAll);
}
