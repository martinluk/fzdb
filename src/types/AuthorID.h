#ifndef FUZZY_MODEL_TYPES_AUTHORID
#define FUZZY_MODEL_TYPES_AUTHORID

#include <string>

#include "./UInt.h"
#include <iostream>

namespace model {
    namespace types {

        // Stores an integer value.
        class AuthorID : public UInt {      

        public:

            AuthorID() : UInt()
            {

            }
            
            AuthorID(uint32_t value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                UInt(value, confidence, author, comment)
            {
            }            
           
			void setupDefaultMetaData() override;

            virtual ~AuthorID() {}            

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<AuthorID>();
				cloned->_value = _value;
				cloned->_locked = _locked;
				cloned->_manager = _manager;
				cloned->_orderingId = _orderingId;
				return cloned;
            }    

			unsigned char confidence() const override {
				return 100;
			}

        protected:

            AuthorID(const char* &serialisedData, std::size_t length) : UInt(serialisedData, length)
            {
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_AUTHORID
