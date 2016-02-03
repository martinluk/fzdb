#ifndef FUZZY_MODEL_TYPES_STRING
#define FUZZY_MODEL_TYPES_STRING

#include <string>
#include <memory>

#include "./Base.h"

namespace model {
	namespace types {
		class String : public Base {
		private:
			friend class TypeSerialiser;
			std::string _value;
			MemberSerialiser::DynamicStringMember _valueWrapper;
			MemberSerialiser _memberSerialiser;
			
			void initMemberSerialiser()
			{
				_memberSerialiser.addDynamicMember(&_valueWrapper);
			}
			
		public:
			String() :Base(100), _value(), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}
			
			String(const std::string value) : Base(100), _value(value), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}
			
			String(const std::string &value, unsigned char confidence) : Base(confidence), _value(value), _valueWrapper(_value)
			{
				initMemberSerialiser();
			}

			std::string value() { return _value; }

			virtual Subtype subtype() const
			{
				return Subtype::TypeString;
			}

			virtual std::size_t serialiseSubclass(Serialiser &serialiser) const
			{
				std::size_t stringLength = _value.size();
				std::unique_ptr<char> buffer(new char[stringLength + 1]);
				memcpy(buffer.get(), _value.c_str(), stringLength);
				buffer.get()[stringLength] = '\0';

				return Base::serialiseSubclass(serialiser)
					+ serialiser.serialise(Serialiser::SerialProperty(buffer.get(), stringLength + 1));
			}

			virtual std::string logString() const
			{
				return std::string("String(\"") + _value + std::string("\", ")
					+ std::to_string(confidence()) + std::string(")");
			}

			virtual std::string toString() override {
				return _value;
			}

			// Inherited via Base
			virtual bool Equals(const std::string val) override {
				return _value == val;
			}

		protected:
			String(const char* &serialisedData) : Base(serialisedData), _value(), _valueWrapper(_value)
			{
				//_value = std::string(serialisedData);
				//serialisedData += _value.size() + 1;
				
				initMemberSerialiser();
				serialisedData += _memberSerialiser.unserialiseDynamicMembers(serialisedData);
			}
		};
	}
}


#endif // !FUZZY_MODEL_TYPES_STRING
