#include "./Confidence.h"
#include "./AuthorID.h"
#include "./TimeStamp.h"

void model::types::Confidence::setupDefaultMetaData()
{
	insertProperty(5, std::make_shared<model::types::AuthorID>(0, 0), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(7, std::make_shared<model::types::TimeStamp>(0), MatchState::None, EntityProperty::Type::LOCKED);
}
