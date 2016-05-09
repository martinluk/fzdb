#include "./property_owner_serialiser.h"
#include "./property_owner.h"
#include <vector>
#include "./type_serialiser.h"
#include "./entity_property.h"
#include <cassert>
#include "./member_serialiser.h"
#include <spdlog/spdlog.h>

#include "./types/value_ref.h"
#include "./types/string.h"
#include "./types/entity_ref.h"
#include "./types/int.h"
#include "./types/date.h"

// This should be incremented whenever a change is made to the format!
#define SERIAL_HEADER_CURRENT_VERSION 1

struct PropertyOwnerSerialHeader {
  unsigned short  version;            // Version identifier.
  std::size_t     size;               // Total size of this serialised entity in bytes.
  std::size_t     propertyCount;      // How many property headers to expect after this header.

  std::size_t     memberDataOffset;   // Offset from beginning of data that the member data begins.
  std::size_t     memberDataLength;   // Length of member data in bytes.

  std::size_t     propertyDataOffset; // Offset from beginning of data that the property data begins.
  std::size_t     propertyDataLength; // Length of all property data in bytes.
};

struct PropertyHeader {
  std::size_t                 offset;     // Offset of property from beginning of property data chunk.
  std::size_t                 size;       // Size of serialised property data in bytes. This includes all values.
  unsigned int                key;        // Property key.
  model::types::SubType       subtype;    // Type identifier for the values this property contains.
  std::size_t                 valueCount; // How many values this property contains.
};

PropertyOwnerSerialiser::PropertyOwnerSerialiser(const std::shared_ptr<PropertyOwner> po) : _entity(po) {

}

std::size_t PropertyOwnerSerialiser::serialise(Serialiser &serialiser) const {
  typedef std::map<unsigned int, std::shared_ptr<EntityProperty>> PropertyTable;

  // Create the initial header.
  PropertyOwnerSerialHeader header;
  Serialiser::zeroBuffer(&header, sizeof(PropertyOwnerSerialHeader));
  std::size_t indexBegin = serialiser.size();

  // Initialise header values.
  header.version = SERIAL_HEADER_CURRENT_VERSION;
  header.propertyCount = _entity->_propertyTable.size();

  // We'll fill these in later.
  header.size = 0;
  header.memberDataOffset = 0;
  header.memberDataLength = 0;
  header.propertyDataOffset = 0;
  header.propertyDataLength = 0;

  // Create a default PropertyHeader that we'll serialise as many times as needed.
  // We set the values individually afterwards.
  PropertyHeader phPlaceholder;
  Serialiser::zeroBuffer(&phPlaceholder, sizeof(PropertyHeader));

  // Add all the headers to a property list.
  std::vector<Serialiser::SerialProperty> propList;
  propList.push_back(Serialiser::SerialProperty(&header, sizeof(PropertyOwnerSerialHeader)));

  for ( std::size_t i = 0; i < header.propertyCount; i++ ) {
    propList.push_back(Serialiser::SerialProperty(&phPlaceholder, sizeof(PropertyHeader)));
  }

  // Serialise all the headers and record where the actual data starts.
  serialiser.serialise(propList);
  std::size_t indexDataBegin = serialiser.size();
  std::size_t indexPropHeadersBegin = indexBegin + sizeof(PropertyOwnerSerialHeader);

  // Serialise the data members for this entity.
  std::size_t membersSerialisedLength = _entity->memberSerialiser().serialiseAll(serialiser);

  // Update the header for this serialisation.
  {
    PropertyOwnerSerialHeader tempHeader = serialiser.castAsPrimitive<PropertyOwnerSerialHeader>(indexBegin);
    tempHeader.memberDataOffset = indexDataBegin - indexBegin;
    tempHeader.memberDataLength = membersSerialisedLength;
    serialiser.overwrite(indexBegin, &tempHeader);
  }

  std::size_t propertiesOffset = serialiser.size();

  // Serialise each property.
  int propNumber = 0;
  std::size_t propDataOffset = 0;
  for ( PropertyTable::const_iterator it = _entity->_propertyTable.cbegin(); it != _entity->_propertyTable.cend(); ++it ) {
    using namespace model::types;

    // Keep track of how many bytes all of the values occupy.
    std::size_t propSerialisedSize = 0;

    std::shared_ptr<EntityProperty> prop = it->second;

    // Serialise each value.
    for (std::size_t i = 0; i < prop->count(); i++ ) {
      TypeSerialiser typeSerialiser(prop->baseValue(i));
      propSerialisedSize += typeSerialiser.serialise(serialiser);
    }

    // Record results in the header.
    {
      PropertyHeader tempHeader = serialiser.castAsPrimitive<PropertyHeader>(indexPropHeadersBegin, propNumber);
      tempHeader.offset = propDataOffset;
      tempHeader.size = propSerialisedSize;
      tempHeader.key = prop->key();
      tempHeader.subtype = prop->subtype();
      tempHeader.valueCount = prop->count();
      serialiser.overwrite<PropertyHeader>(indexPropHeadersBegin, &tempHeader, propNumber);
    }

    // Increment our counters.
    propDataOffset += propSerialisedSize;
    propNumber++;
  }

  // Record the total serialised size.
  //PropertyOwnerSerialHeader* pHeader = serialiser.reinterpretCast<PropertyOwnerSerialHeader*>(indexBegin);
  header = serialiser.castAsPrimitive<PropertyOwnerSerialHeader>(indexBegin);
  header.propertyDataOffset = propertiesOffset - indexBegin;
  header.propertyDataLength = propDataOffset;
  header.size = serialiser.size() - indexBegin;
  serialiser.overwrite<PropertyOwnerSerialHeader>(indexBegin, &header);
  
  return header.size;
}

void populate(std::shared_ptr<PropertyOwner> ent, const PropertyHeader* header, const char* data) {
  using namespace model::types;

  std::vector<BasePointer> values;
  for (std::size_t i = 0; i < header->valueCount; i++ ) {
    std::size_t advance = 0;
    BasePointer val = TypeSerialiser::unserialise(data, &advance);

    if ( !val.get() ) {
      throw PropertyOwnerSerialiser::InvalidInputEntityException("Error unserialising property " + std::to_string(header->key)
          //            + " of entity " + std::to_string(ent->getHandle())
          + " - null property returned.");
    }

    if ( val->subtype() != header->subtype && (val->subtype() == model::types::SubType::EntityRef & header->subtype != model::types::SubType::SourceRef)) {
      throw PropertyOwnerSerialiser::InvalidInputEntityException("Error unserialising property " + std::to_string(header->key)
          //           + " of entity " + std::to_string(ent->getHandle())
          + ": subtype \"" + model::types::getSubString(val->subtype())
          + "\" does not match expected subtype \""
          + model::types::getSubString(header->subtype) + "\".");
    }

    values.push_back(val);
    data += advance;
  }

  ent->insertProperty(std::shared_ptr<EntityProperty>(new EntityProperty(EntityProperty::Type::FUZZY, header->key, header->subtype, values)), MatchState::Unserialising);
}

std::shared_ptr<PropertyOwner> PropertyOwnerSerialiser::unserialise(const char* &serialData, std::size_t length) {
  using namespace model::types;

  const PropertyOwnerSerialHeader* pHeader = reinterpret_cast<const PropertyOwnerSerialHeader*>(serialData);

  // Make sure the header version matches our current version.
  if ( pHeader->version != SERIAL_HEADER_CURRENT_VERSION )
    throw InvalidInputEntityException("Entity serial version does not match expected version.");

  if ( pHeader->size != length )
    throw InvalidInputEntityException("Specified internal data size does not equal actual size of data.");

  if ( pHeader->memberDataOffset >= length )
    throw InvalidInputEntityException("Member data offset exceeds length of input data.");

  if ( pHeader->memberDataOffset + pHeader->memberDataLength > length )
    throw InvalidInputEntityException("Length of member data exceeds length of input data.");

  if ( pHeader->propertyDataOffset > length || (pHeader->propertyCount > 0 && pHeader->propertyDataOffset == length) )
    throw InvalidInputEntityException("Property data offset exceeds length of input data.");

  if ( pHeader->propertyDataOffset + pHeader->propertyDataLength > length )
    throw InvalidInputEntityException("Length of property data exceeds length of input data.");

  if ( pHeader->propertyDataOffset < pHeader->memberDataOffset + pHeader->memberDataLength )
    throw InvalidInputEntityException("Member data and property data cannot overlap.");

  // Unserialise the members.
  const char* memberData = serialData + pHeader->memberDataOffset;
  _entity->_memberSerialiser.unserialiseAll(memberData, pHeader->memberDataLength);

  // If we're locked, unlock temporarily.
  bool wasLocked = _entity->_locked;
  _entity->_locked = false;

  // Unserialise the properties.
  const PropertyHeader* pPropHeaders = reinterpret_cast<const PropertyHeader*>(serialData + sizeof(PropertyOwnerSerialHeader));
  for (std::size_t i = 0; i < pHeader->propertyCount; i++ ) {
    // Get the header for this property.
    const PropertyHeader* p = &(pPropHeaders[i]);

    if ( pHeader->propertyDataOffset + p->offset >= length )
      throw InvalidInputEntityException("Offset for entity property exceeds length of input data.");

    if ( pHeader->propertyDataOffset + p->offset + p->size > length )
      throw InvalidInputEntityException("Length of entity property exceeds length of input data.");

    // Calculate the pointer to the actual property data.
    const char* data = serialData + pHeader->propertyDataOffset + p->offset;
    //spdlog::get("main")->info("Property data offset: {} P offset: {}", pHeader->propertyDataOffset, p->offset);

    switch (p->subtype) {
    case SubType::UInt32:
    case SubType::Int32:
    case SubType::String:
    case SubType::EntityRef:
    case SubType::Date:
    case SubType::AuthorID:
    case SubType::TimeStamp:
    case SubType::TypeID:
    case SubType::SourceRef:
    case SubType::Confidence:
      populate(_entity, p, data);
      \
      break;

    default:
      throw InvalidInputEntityException("Error unserialising property " + std::to_string(p->key)
                                        //           + " of entity " + std::to_string(ent->getHandle())
                                        + ": subtype \"" + model::types::getSubString(p->subtype)
                                        + "\" is invalid.");
    }
  }

  _entity->_locked = wasLocked;
  serialData = serialData + length;
  return _entity;
}
