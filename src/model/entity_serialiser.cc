#include "./entity_serialiser.h"
#include "./entity.h"
#include <vector>
#include "./type_serialiser.h"
#include "./property_owner_serialiser.h"
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

struct EntitySerialHeader {
  unsigned short  version;            // Version identifier.
  std::size_t     size;               // Total size of this serialised entity in bytes.

  std::size_t     memberDataOffset;   // Offset from beginning of data that the member data begins.
  std::size_t     memberDataLength;   // Length of member data in bytes.

  std::size_t     propertyDataOffset; // Offset from beginning of data that the property data begins.
  std::size_t     propertyDataLength; // Length of all property data in bytes.
};

EntitySerialiser::EntitySerialiser(const std::shared_ptr<Entity> ent) : _entity(ent) {

}

std::size_t EntitySerialiser::serialise(Serialiser &serialiser) const {
  std::size_t initialSize = serialiser.size();
  EntitySerialHeader header;
  Serialiser::zeroBuffer(&header, sizeof(EntitySerialHeader));
  header.version = SERIAL_HEADER_CURRENT_VERSION;

  std::size_t bytesSerialised = serialiser.serialise(Serialiser::SerialProperty(&header, sizeof(EntitySerialHeader)));
  bytesSerialised += _entity->_memberSerialiser.serialiseAll(serialiser);
  EntitySerialHeader* pHeader = serialiser.reinterpretCast<EntitySerialHeader*>(initialSize);
  pHeader->memberDataOffset = sizeof(EntitySerialHeader);
  pHeader->memberDataLength = bytesSerialised - sizeof(EntitySerialHeader);
  pHeader->propertyDataOffset = pHeader->memberDataOffset + pHeader->memberDataLength;
  PropertyOwnerSerialiser pos(_entity);
  bytesSerialised += pos.serialise(serialiser);
  pHeader = serialiser.reinterpretCast<EntitySerialHeader*>(initialSize);
  pHeader->size = bytesSerialised;
  pHeader->propertyDataLength = bytesSerialised - pHeader->propertyDataOffset;

  return pHeader->size;
}

std::shared_ptr<Entity> EntitySerialiser::unserialise(const char *serialData, std::size_t length) {
  using namespace model::types;

  const EntitySerialHeader* pHeader = reinterpret_cast<const EntitySerialHeader*>(serialData);

  // Make sure the header version matches our current version.
  if ( pHeader->version != SERIAL_HEADER_CURRENT_VERSION )
    throw InvalidInputEntityException("Entity serial version does not match expected version.");

  if ( pHeader->size != length )
    throw InvalidInputEntityException("Specified internal data size does not equal actual size of data.");

  if ( pHeader->memberDataOffset >= length )
    throw InvalidInputEntityException("Member data offset exceeds length of input data.");

  if ( pHeader->memberDataOffset + pHeader->memberDataLength > length )
    throw InvalidInputEntityException("Length of member data exceeds length of input data.");

  //if ( pHeader->propertyDataOffset > length || (pHeader->propertyCount > 0 && pHeader->propertyDataOffset == length) )
  //    throw InvalidInputEntityException("Property data offset exceeds length of input data.");

  if ( pHeader->propertyDataOffset + pHeader->propertyDataLength > length )
    throw InvalidInputEntityException("Length of property data exceeds length of input data.");

  if ( pHeader->propertyDataOffset < pHeader->memberDataOffset + pHeader->memberDataLength )
    throw InvalidInputEntityException("Member data and property data cannot overlap.");

  // Create an entity shell.
  std::shared_ptr<Entity> ent = std::make_shared<Entity>(0);

  // Unserialise the members.
  const char* memberData = serialData + pHeader->memberDataOffset;
  ent->_memberSerialiser.unserialiseAll(memberData, pHeader->memberDataLength);
  PropertyOwnerSerialiser pos(ent);
  const char* propertyData = serialData + pHeader->propertyDataOffset;
  pos.unserialise(propertyData, pHeader->propertyDataLength);

  return ent;
}
