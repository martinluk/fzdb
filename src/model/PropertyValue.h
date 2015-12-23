#ifndef MODEL_PROPERTYVALUE_H
#define MODEL_PROPERTYVALUE_H

#include "./Variant.h"
#include "./ISerialisable.h"

// Defines a property value. Values can have associated confidence quantifiers.
class PropertyValue : public ISerialisable
{
	public:
		// Constructs a concrete value. The confidence is 1 by default.
		PropertyValue(const Variant &value, signed char confidence = 255);
		
		// Getters
		Variant value() const;
		float confidence() const;

		// Implementation of ISerialisable.
		virtual void serialise(Serialiser &serialiser) const override;
		static PropertyValue unserialise(const char* data);

	private:
		struct SerialHeader
		{
			std::size_t valueSize;
		};

		void setConfidenceClamp(float c);

		Variant	_value;
		signed char _confidence;
};

#endif	// MODEL_PROPERTYVALUE_H
