#include "./PropertyValue.h"

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
