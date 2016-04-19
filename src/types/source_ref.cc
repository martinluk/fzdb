#include "./source_ref.h"
#include "./author_id.h"
#include "./timestamp.h"
#include "./confidence.h"
#include "../job_queue.h"

void model::types::SourceRef::setupDefaultMetaData(const unsigned char confidence)
{
	unsigned int authorId = JobQueue::CurrentSession().lock() ? JobQueue::CurrentSession().lock()->userId() : 0;

	insertProperty(5, std::make_shared<model::types::AuthorID>(authorId), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(7, std::make_shared<model::types::TimeStamp>(), MatchState::None, EntityProperty::Type::LOCKED);
}
