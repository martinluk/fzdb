#include "./Base.h"
#include "./String.h"
#include "./Int.h"
#include "./EntityRef.h"

using namespace model::types;

bool model::types::Base::hasProperty(const unsigned int & key, MatchState state) const
{
	if (key == 5) return true;
	if (key == 6) return true;
	if (key == 7) return true;
	if (key == 8) return true;
	return PropertyOwner::hasProperty(key);
}

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
		 output->append(std::make_shared<model::types::String>(boost::posix_time::to_simple_string(_timeCreated), 0));
		 return output;
		 }
		 case 8: {
			 auto output = std::make_shared<EntityProperty>(8, model::types::SubType::TypeInt32);
			 output->append(std::make_shared<model::types::Int>(_confidence, 0));
			 return output;
		 }
	 default:
		 return PropertyOwner::getProperty(key);
	 }
}