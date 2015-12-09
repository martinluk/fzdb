#ifndef MODEL_PROPERTYVALUE_H
#define MODEL_PROPERTYVALUE_H

#include "./Variant.h"

// Defines a property value. Values can have associated confidence quantifiers.
class PropertyValue
{
	public:
		// Constructs a concrete value. The confidence is 1 by default.
		PropertyValue(const Variant &value, float confidence = 1.0f);
		
		// Getters
		Variant value() const;
		float confidence() const;

	private:
		void setConfidenceClamp(float c);

		Variant	value_;
		float confidence_;
};

#endif	// MODEL_PROPERTYVALUE_H
