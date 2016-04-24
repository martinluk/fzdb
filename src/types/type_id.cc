#include "./type_id.h"
#include "../singletons.h"
#include "../model/entity_manager.h"
#include "./author_id.h"
#include "./timestamp.h"
#include "../job_queue.h"

std::string model::types::TypeID::toString() const {
  return Singletons::cDatabase()->entityManager().getTypeName(_value);
}

void model::types::TypeID::setupDefaultMetaData(const unsigned char confidence) {
  unsigned int authorId = JobQueue::CurrentSession().lock() ? JobQueue::CurrentSession().lock()->userId() : 0;
  insertProperty(5, std::make_shared<model::types::AuthorID>(authorId), MatchState::None, EntityProperty::Type::LOCKED);
  insertProperty(7, std::make_shared<model::types::TimeStamp>(), MatchState::None, EntityProperty::Type::LOCKED);
}
