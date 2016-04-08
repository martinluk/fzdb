#ifndef FUZZY_MODEL_TYPES_SOURCE
#define FUZZY_MODEL_TYPES_SOURCE

#include <string>

#include "./EntityRef.h"

typedef unsigned long long EHandle_t;

namespace model {
    namespace types {

        // Class representing a reference to another entity.
        // This reference is stored as a handle to the entity concerned.
        // There is no guarantee that this handle will still be valid
        // when it is used - the caller needs to check!
        class SourceRef : public EntityRef {
        public:
            SourceRef() : EntityRef()
            {        
            }

            SourceRef(EHandle_t value, unsigned int author, unsigned char confidence = 100, const std::string &comment = std::string()) :
                EntityRef(value, confidence, author, comment)
            {
            }           

			void setupDefaultMetaData() override;
            
            virtual ~SourceRef() {}

            virtual std::shared_ptr<Base> Clone() override {
                auto cloned = std::make_shared<SourceRef>(_value, _originalAuthorId, _confidence);
				cloned->_orderingId = _orderingId;
				return cloned;
            }
          

        protected:
           
            SourceRef(const char* &serialisedData, std::size_t length) : EntityRef(serialisedData, length)
            {
            }
        };
    }
}


#endif // !FUZZY_MODEL_TYPES_ENTITY
