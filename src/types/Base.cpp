#include "./Base.h"
#include "./String.h"
#include "./EntityRef.h"

using namespace model::types;

std::shared_ptr<EntityProperty> Base::getProperty(const unsigned int &key) const {
	 switch (key) {
		 case 5: {
		 auto output = std::make_shared<EntityProperty>(5, model::types::SubType::TypeString);
		 output->append(std::make_shared<model::types::String>(std::to_string(_originalAuthorId), 0));
		 return output;
		 }
		 case 6: {
		 auto output = std::make_shared<EntityProperty>(6, model::types::SubType::TypeEntityRef);
		 output->append(std::make_shared<model::types::EntityRef>(_sourceEntityId, 0));
		 return output;
		 }
		 case 7: {
		 auto output = std::make_shared<EntityProperty>(7, model::types::SubType::TypeString);
		 output->append(std::make_shared<model::types::String>(std::to_string(_originalAuthorId), 0));
		 return output;
		 }
	 default:
		 return PropertyOwner::getProperty(key);
	 }
}