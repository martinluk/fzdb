#ifndef MODEL_PROPERTYVALUE_H
#define MODEL_PROPERTYVALUE_H

#include "./Variant.h"
#include "./ISerialisable.h"

// Defines a property value. Values can have associated confidence quantifiers.
class PropertyValue : public ISerialisable
{
	public:
		// Constructs a concrete value. The confidence is 1 by default.
		PropertyValue(const Variant &value, float confidence = 1.0f);
		
		// Getters
		Variant value() const;
		float confidence() const;

		// Implementation of ISerialisable.
		std::pair<std::size_t,bool> serialise(char* buffer, std::size_t maxSize) const override;

	private:
		struct SerialHeader
		{
			std::size_t valueSize;
		};

		void setConfidenceClamp(float c);

		Variant	value_;
		float confidence_;
};

#endif	// MODEL_PROPERTYVALUE_H
