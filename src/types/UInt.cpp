#include "./UInt.h"
#include "./TimeStamp.h"
#include "./EntityRef.h"
#include "./SourceRef.h"
#include "./Confidence.h"
#include "./AuthorID.h"

void model::types::UInt::setupDefaultMetaData()
{
	insertProperty(5, std::make_shared<model::types::AuthorID>(0, 0), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(6, std::make_shared<model::types::SourceRef>(0, 0), MatchState::None, EntityProperty::Type::CONCRETESINGLE);
	insertProperty(7, std::make_shared<model::types::TimeStamp>(0), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(8, std::make_shared<model::types::Confidence>(0, 0), MatchState::None, EntityProperty::Type::CONCRETESINGLE);
}
