#include "./Base.h"
#include "./String.h"
#include "./Int.h"
#include "./UInt.h"
#include "./TimeStamp.h"
#include "./EntityRef.h"
#include "./SourceRef.h"
#include "./Confidence.h"
#include "./AuthorID.h"
#include "../JobQueue.h"

using namespace model::types;

void model::types::Base::setupDefaultMetaData(const unsigned char confidence)
{
	unsigned int authorId = JobQueue::CurrentSession().lock() ? JobQueue::CurrentSession().lock()->userId() : 0;
	insertProperty(5, std::make_shared<model::types::AuthorID>(authorId), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(6, std::make_shared<model::types::SourceRef>(0), MatchState::None, EntityProperty::Type::CONCRETESINGLE);
	insertProperty(7, std::make_shared<model::types::TimeStamp>(), MatchState::None, EntityProperty::Type::LOCKED);
	insertProperty(8, std::make_shared<model::types::Confidence>(confidence, authorId), MatchState::None, EntityProperty::Type::CONCRETESINGLE);
}


void Base::initMemberSerialiser()
{
	_memberSerialiser.addPrimitive(&_orderingId, sizeof(_orderingId));
	_memberSerialiser.setInitialised();
}

void model::types::Base::copyValues(const std::shared_ptr<model::types::Base> cloned)
{
	cloned->_initialised = true;
	cloned->_locked = _locked;
	cloned->_manager = _manager;
	cloned->_orderingId = _orderingId;

	for (auto prop : _propertyTable) {
		cloned->insertProperty(prop.second->Clone());
	}
}

Base::Base() {
	_initialised = false;
	_orderingId = 0;
}

void Base::Init(unsigned char confidence) {
	if (!_initialised) {
		setupDefaultMetaData(confidence);
		_initialised = true;
	}
}

Base::~Base() {}

std::shared_ptr<Base> Base::Clone() {
	auto cloned = std::make_shared<Base>();
	copyValues(cloned);
	return cloned;
}

unsigned char Base::Equals(const std::string &val) const {
	return 0;
}

// This specifically should NOT compare the confidence, ordering, source, author, time of creation or comment.
bool Base::valuesEqualOnly(const Base *other) const
{
	return subtype() == other->subtype();
}

// Returns whether this value is equal to the given object.
unsigned char Base::Equals(const model::Object &object) {
	if (object.type == model::Object::Type::VARIABLE) return 0;
	if (object.type == model::Object::Type::INT && subtype() != SubType::Int32) return 0;
	if (object.type == model::Object::Type::STRING && subtype() != SubType::String) return 0;
	if (object.type == model::Object::Type::ENTITYREF && subtype() != SubType::EntityRef) return 0;
	return Equals(object.value);
}

// What's the string representation of this value?
std::string Base::toString() const
{
	return "";
}

unsigned char Base::confidence() const
{
	return std::dynamic_pointer_cast<model::types::Confidence>(getProperty(8)->baseTop())->value();
}


// Subclasses reimplement this.
// As a base class, our type is undefined.
SubType Base::subtype() const
{
	return SubType::Undefined;
}

std::string Base::logString(const Database* db) const
{
	return std::string("Base(") + std::to_string(confidence()) + std::string(")");
}

void Base::OrderingId(unsigned int id) {
	_orderingId = id;
}

unsigned int Base::OrderingId() {
	return _orderingId;
}

// For debugging - make sure we are -exactly- the same as the other type.
bool Base::memberwiseEqual(const Base* other) const
{
	return subtype() == other->subtype() &&
		_orderingId == other->_orderingId &&
		confidence() == other->confidence();
}

// Called when serialising.
std::size_t Base::serialiseSubclass(Serialiser &serialiser)
{
	if (!_memberSerialiser.initialised()) Base::initMemberSerialiser();
	return _memberSerialiser.serialiseAll(serialiser);
}

// Called to construct from serialised data.
Base::Base(const char* &serialisedData, std::size_t length)
{
	initMemberSerialiser();
//	initConvenienceMembers();
	serialisedData += _memberSerialiser.unserialiseAll(serialisedData, length);
	_initialised = true;
}