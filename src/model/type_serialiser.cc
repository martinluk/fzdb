#include "./type_serialiser.h"
#include "./property_owner_serialiser.h"
#include "../types/base.h"
#include "../types/int.h"
#include "../types/uint.h"
#include "../types/string.h"
#include "../types/entity_ref.h"
#include "../types/date.h"
#include "../types/author_id.h"
#include "../types/type_id.h"
#include "../types/confidence.h"
#include "../types/timestamp.h"
#include "../types/source_ref.h"
#include <spdlog/spdlog.h>
#include <cassert>
#include <stdexcept>

using namespace model::types;
struct TypeSerialHeader {
  std::size_t totalSize;
  std::size_t memberVariablesSize;
  SubType subtype;
};

TypeSerialiser::TypeSerialiser(const std::shared_ptr<Base> &type) : baseType_(type) {
}

std::size_t TypeSerialiser::serialise(Serialiser &serialiser) const {
  std::size_t initialSize = serialiser.size();
  TypeSerialHeader header;
  Serialiser::zeroBuffer(&header, sizeof(TypeSerialHeader));
  header.memberVariablesSize = 0;
  header.subtype = baseType_->subtype();
  if (header.subtype == model::types::SubType::Undefined) {
    assert(false);
  }

  if (header.subtype == model::types::SubType::SourceRef) {
    bool a = false;
  }

  std::size_t bytesSerialised = serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(TypeSerialHeader)));
  bytesSerialised += baseType_->serialiseSubclass(serialiser);
  
  header.memberVariablesSize = bytesSerialised - sizeof(TypeSerialHeader);
  
  PropertyOwnerSerialiser pos(baseType_);
  bytesSerialised += pos.serialise(serialiser);
  
  header.totalSize = bytesSerialised;

  if (header.subtype == model::types::SubType::Undefined) {
    assert(false);
  }
  
  serialiser.overwrite<TypeSerialHeader>(initialSize, &header);

  return header.totalSize;
}

std::shared_ptr<Base> TypeSerialiser::unserialise(const char* serialisedData, std::size_t* advance) {
  const char* d = serialisedData;

  const TypeSerialHeader* pHeader = reinterpret_cast<const TypeSerialHeader*>(d);
  d += sizeof(TypeSerialHeader);
  std::shared_ptr<Base> b;
  std::size_t dataSize = pHeader->totalSize - sizeof(TypeSerialHeader);

  // Since TypeSerialiser is the friend class of the subtypes,
  // we need to use the shared_ptr constructor directly instead
  // of make_shared(). The latter won't work because that would
  // require the function to be a friend of the classes, which
  // it's not.
  const char* dBefore = d;
  switch (pHeader->subtype) {
//    case SubType::Undefined:
//        //b = std::make_shared<Base>(d);
//        b = std::shared_ptr<Base>(new Base(d, dataSize));
//                break;

  case SubType::Int32:
    b = std::shared_ptr<Int>(new Int(d, pHeader->memberVariablesSize));
    break;

  case SubType::UInt32:
    b = std::shared_ptr<UInt>(new UInt(d, dataSize));
    break;

  case SubType::String:
    b = std::shared_ptr<String>(new String(d, dataSize));
    break;

  case SubType::EntityRef:
    b = std::shared_ptr<EntityRef>(new EntityRef(d, dataSize));
    break;

  case SubType::Date:
    b = std::shared_ptr<Date>(new Date(d, dataSize));
    break;

  case SubType::TimeStamp:
    b = std::shared_ptr<TimeStamp>(new TimeStamp(d, dataSize));
    break;

  case SubType::AuthorID:
    b = std::shared_ptr<AuthorID>(new AuthorID(d, dataSize));
    break;

  case SubType::Confidence:
    b = std::shared_ptr<Confidence>(new Confidence(d, dataSize));
    break;

  case SubType::SourceRef:
    b = std::shared_ptr<SourceRef>(new SourceRef(d, dataSize));
    break;

  case SubType::TypeID:
    b = std::shared_ptr<TypeID>(new TypeID(d, dataSize));
    break;

  default:
    assert(false);
    break;
  }

  PropertyOwnerSerialiser pos(b);
  pos.unserialise(d, dataSize - pHeader->memberVariablesSize);

  if ( (d - dBefore) != dataSize ) {
    throw std::runtime_error("Expected unserialisation of " + std::to_string(dataSize)
                             + " bytes but got " + std::to_string((std::size_t)(d - dBefore)) + " instead.");
  }

  if ( advance ) {
    *advance = d - serialisedData;
  }

  return b;
}
