#include "./SourceRef.h"
#include "./AuthorID.h"
#include "./TimeStamp.h"
#include "./Confidence.h"

void model::types::SourceRef::setupDefaultMetaData()
{
	insertProperty(5, std::make_shared<model::types::AuthorID>(0, 0), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(7, std::make_shared<model::types::TimeStamp>(0), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(8, std::make_shared<model::types::Confidence>(0, 0), MatchState::None, EntityProperty::Type::CONCRETESINGLE);
}
