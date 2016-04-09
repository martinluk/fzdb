#ifndef FUZZY_MODEL_TYPES_TYPEID
#define FUZZY_MODEL_TYPES_TYPEID

#include <string>

#include "./UInt.h"
#include <iostream>

namespace model {
    namespace types {

        // Stores an integer value.
        class TypeID : public UInt {      

        public:

            TypeID() : UInt()
            {

            }
            
            TypeID(uint32_t value) :
                UInt(value)
            {
            }            
           

            virtual ~TypeID() {}            

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<TypeID>();
				cloned->_value = _value;
				copyValues(cloned);
				return cloned;
            }    

			std::string toString() const override;

			void setupDefaultMetaData(const unsigned char confidence) override;

			unsigned char confidence() const override {
				return 100;
			}

        protected:

            TypeID(const char* &serialisedData, std::size_t length) : UInt(serialisedData, length)
            {
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_TYPEID
