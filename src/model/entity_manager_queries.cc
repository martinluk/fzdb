#include "./entity_manager.h"
#include <cassert>
#include <set>
#include <sstream>
#include "../file_system.h"
#include "./graph_serialiser.h"
#include "../util.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include "../parser.h"
#include <spdlog/spdlog.h>
#include "../util.h"

#include "./types/base.h"
#include "./types/value_ref.h"
#include "./types/string.h"
#include "./types/entity_ref.h"
#include "./types/int.h"
#include "./types/date.h"
#include "./types/property.h"
#include "../singletons.h"

//$a <predicate> <object>
void EntityManager::Scan1(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  //get the property id

  //the variable has been used before, we only need to iterate over valid values from before
  if (variableSet.used(variableId)) {

    if ((variableSet.typeOf(variableId) & static_cast<unsigned char>(model::types::TypePosition::METASUBJECT)) == 0) {
      variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
      ScanUPR(std::move(variableSet), variableId, std::move(predicate), std::move(object), std::move(metaVar), std::move(settings));
    } else {
      ScanMPR(std::move(variableSet), variableId, std::move(predicate), std::move(object), std::move(metaVar), std::move(settings));
    }
    return;
  }

  //the variable has not been used, add all the values found!
  ScanVPR(std::move(variableSet), variableId, std::move(predicate), std::move(object), std::move(metaVar), std::move(settings));
}


//$a <predicate> $b
void EntityManager::Scan2(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  //TODO: consider the case where variableName2 is already in variableSet

  //the variable has been used before, we only need to iterate over valid values from before
  if (variableSet.used(variableId)) {

    if (variableSet.used(variableId2)) {
      // this cannot add any new data, only remove it
      if ((variableSet.typeOf(variableId) & static_cast<unsigned char>(model::types::TypePosition::METASUBJECT)) == 0) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        variableSet.enforcePosition(variableId2, model::types::TypePosition::OBJECT);
        ScanUPU(std::move(variableSet), variableId, std::move(predicate), variableId2, std::move(metaVar), std::move(settings));
      } else {
        variableSet.enforcePosition(variableId2, model::types::TypePosition::OBJECT);
        ScanMPU(std::move(variableSet), variableId, std::move(predicate), variableId2, std::move(metaVar), std::move(settings));
      }
    } else {
      if ((variableSet.typeOf(variableId) & static_cast<unsigned char>(model::types::TypePosition::METASUBJECT)) == 0) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        ScanUPV(std::move(variableSet), variableId, std::move(predicate), variableId2, std::move(metaVar), std::move(settings));
      } else {
        ScanMPV(std::move(variableSet), variableId, std::move(predicate), variableId2, std::move(metaVar), std::move(settings));
      }
    }
    return;
  } else {
    if (variableSet.used(variableId2)) {
      variableSet.enforcePosition(variableId2, model::types::TypePosition::OBJECT);
      ScanVPU(std::move(variableSet), variableId, std::move(predicate), variableId2, std::move(metaVar), std::move(settings));
    } else {
      ScanVPV(std::move(variableSet), variableId, std::move(predicate), variableId2, std::move(metaVar), std::move(settings));
    }
  }
}

//$a $b "Value"
void EntityManager::Scan3(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  if (variableSet.used(variableId)) {

    if (variableSet.used(variableId2)) {
      // this cannot add any new data, only remove it
      if ((variableSet.typeOf(variableId) & static_cast<unsigned char>(model::types::TypePosition::METASUBJECT)) == 0) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
        ScanUUR(std::move(variableSet), variableId, variableId2, std::move(object), std::move(metaVar), std::move(settings));
      } else {
        variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
        ScanMUR(std::move(variableSet), variableId, variableId2, std::move(object), std::move(metaVar), std::move(settings));
      }
    } else {
      if ((variableSet.typeOf(variableId) & static_cast<unsigned char>(model::types::TypePosition::METASUBJECT)) == 0) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        ScanUVR(std::move(variableSet), variableId, variableId2, std::move(object), std::move(metaVar), std::move(settings));
      } else {
        ScanMVR(std::move(variableSet), variableId, variableId2, std::move(object), std::move(metaVar), std::move(settings));
      }
    }
    return;
  } else {
    if (variableSet.used(variableId2)) {
      variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
      ScanVUR(std::move(variableSet), variableId, variableId2, std::move(object), std::move(metaVar), std::move(settings));
    } else {
      ScanVVR(std::move(variableSet), variableId, variableId2, std::move(object), std::move(metaVar), std::move(settings));
    }
  }
}

//$a $b $c
// TODO: where are the Ms?
void EntityManager::Scan4(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {

  if (variableSet.used(variableId)) {
    if (variableSet.used(variableId2)) {
      if (variableSet.used(variableId3)) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
        variableSet.enforcePosition(variableId3, model::types::TypePosition::OBJECT);
        ScanUUU(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      } else {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
        ScanUUV(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      }
    } else {
      if (variableSet.used(variableId3)) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        variableSet.enforcePosition(variableId3, model::types::TypePosition::OBJECT);
        ScanUVU(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      } else {
        variableSet.enforcePosition(variableId, model::types::TypePosition::SUBJECT);
        ScanUVV(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      }
    }
  } else {
    if (variableSet.used(variableId2)) {
      if (variableSet.used(variableId3)) {
        variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
        variableSet.enforcePosition(variableId3, model::types::TypePosition::OBJECT);
        ScanVUU(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      } else {
        variableSet.enforcePosition(variableId2, model::types::TypePosition::PREDICATE);
        ScanVUV(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      }
    } else {
      if (variableSet.used(variableId3)) {
        variableSet.enforcePosition(variableId3, model::types::TypePosition::OBJECT);
        ScanVVU(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      } else {
        ScanVVV(std::move(variableSet), variableId, variableId2, variableId3, std::move(metaVar), std::move(settings));
      }
    }
  }

}

//entity <predicate> $c
void EntityManager::Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const {
  if (variableSet.used(variableId)) {
    variableSet.enforcePosition(variableId, model::types::TypePosition::OBJECT);
    ScanEPU(std::move(variableSet), std::move(subject), std::move(predicate), variableId, std::move(metaVar), std::move(settings));
  } else {
    ScanEPV(std::move(variableSet), std::move(subject), std::move(predicate), variableId, std::move(metaVar), std::move(settings));
  }
}

// entity $b value
void EntityManager::Scan6(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {
  if (variableSet.used(variableId)) {
    variableSet.enforcePosition(variableId, model::types::TypePosition::PREDICATE);
    ScanEUR(std::move(variableSet), std::move(subject), variableId, std::move(object), std::move(metaVar), std::move(settings));
  } else {
    ScanEVR(std::move(variableSet), std::move(subject), variableId, std::move(object), std::move(metaVar), std::move(settings));
  }
}

// entity $b $c
void EntityManager::Scan7(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

  if (EntityExists(entityRef)) {
    if (variableSet.used(variableId)) {
      if (variableSet.used(variableId2)) {
        variableSet.enforcePosition(variableId, model::types::TypePosition::PREDICATE);
        variableSet.enforcePosition(variableId2, model::types::TypePosition::OBJECT);
        ScanEUU(std::move(variableSet), std::move(subject), variableId, variableId2, std::move(metaVar), std::move(settings));
      } else {
        variableSet.enforcePosition(variableId, model::types::TypePosition::PREDICATE);
        ScanEUV(std::move(variableSet), std::move(subject), variableId, variableId2, std::move(metaVar), std::move(settings));
      }
    } else {
      if (variableSet.used(variableId2)) {
        variableSet.enforcePosition(variableId2, model::types::TypePosition::OBJECT);
        ScanEVU(std::move(variableSet), std::move(subject), variableId, variableId2, std::move(metaVar), std::move(settings));
      } else {
        ScanEVV(std::move(variableSet), std::move(subject), variableId, variableId2, std::move(metaVar), std::move(settings));
      }
    }
  }

}

// $a $b $c
void EntityManager::ScanVVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
  for (auto entity : _entities) {
    for (auto prop : entity.second->properties()) {

      auto newRecord = std::make_shared<model::types::EntityRef>(entity.first);
      newRecord->Init(100);
      auto rowId = variableSet.add(std::move(variableId),
                                   newRecord, prop.first, entity.first,
                                   model::types::SubType::EntityRef, std::move(metaVar));

      auto newRecord2 = std::make_shared<model::types::Property>(prop.first, this, 0);
      newRecord2->Init(100);
      variableSet.add(std::move(variableId2),
                      newRecord2, prop.first, entity.first,
                      model::types::SubType::PropertyReference, std::move(metaVar), rowId);

      auto type = _propertyTypes.at(prop.first);
	  for (auto originalVal : entity.second->getProperty(prop.first)->baseValues()) {
		  std::shared_ptr<model::types::Base> val = originalVal->Clone();
		  variableSet.add(std::move(variableId3),
			  std::move(val), prop.first, entity.first,
			  std::move(type), std::move(metaVar), rowId);
		  if (settings.canon)break;
	  }     
    }
  }
}

void EntityManager::ScanVVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
	auto data = variableSet.getData(variableId3);
	variableSet.removeRowsWith(variableId3);
	for (auto dat : data) {
		if (dat.empty()) continue;
		VariableSet temp = variableSet.split();
		ScanVVR(std::move(temp), variableId, variableId2, dat.dataPointer()->toObject(), std::move(metaVar), std::move(settings));
		for (auto iter = temp.begin(); iter != temp.end(); iter++) {
			(*iter)[variableId3].reset(dat.dataPointer(), dat.entity(), dat.property());
		}
		variableSet.join(std::move(temp));
	}
}

void EntityManager::ScanVUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
	auto data = variableSet.getData(variableId2);
	variableSet.removeRowsWith(variableId2);
	for (auto dat : data) {
		if (dat.empty()) continue;
		VariableSet temp = variableSet.split();
		ScanVPV(std::move(temp), variableId, model::Predicate(model::Predicate::Type::PROPERTY, dat.dataPointer()->toString()), variableId3, std::move(metaVar), std::move(settings));
		for (auto iter = temp.begin(); iter != temp.end(); iter++) {
			(*iter)[variableId2].reset(dat.dataPointer(), dat.entity(), dat.property());
		}
		variableSet.join(std::move(temp));
	}
}

void EntityManager::ScanVUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
	auto data2 = variableSet.getData(variableId2);
	auto data3 = variableSet.getData(variableId3);
	variableSet.removeRowsWith(variableId);
	variableSet.removeRowsWith(variableId3);
	for (auto dat2 : data2) {
		if (dat2.empty()) continue;
		for (auto dat3 : data3) {
			if (dat3.empty()) continue;
			VariableSet temp = variableSet.split();

			ScanVPR(std::move(temp), std::move(variableId),
				model::Predicate(model::Predicate::Type::PROPERTY, dat2.dataPointer()->toString()),
				dat3.dataPointer()->toObject(), std::move(metaVar), std::move(settings));

			for (auto iter = temp.begin(); iter != temp.end(); iter++) {
				(*iter)[variableId2].reset(dat2.dataPointer(), dat2.entity(), dat2.property());
				(*iter)[variableId3].reset(dat3.dataPointer(), dat3.entity(), dat3.property());
			}

			variableSet.join(std::move(temp));
		}
	}
}

void EntityManager::ScanUVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
	auto data = variableSet.getData(variableId);
	variableSet.removeRowsWith(variableId);
	for (auto dat : data) {
		if (dat.empty()) continue;
		VariableSet temp = variableSet.split();
		ScanEVV(std::move(temp), model::Subject(model::Subject::Type::ENTITYREF, dat.dataPointer()->toString()), variableId2, variableId3, std::move(metaVar), std::move(settings));
		for (auto iter = temp.begin(); iter != temp.end(); iter++) {
			(*iter)[variableId].reset(dat.dataPointer(), dat.entity(), dat.property());
		}
		variableSet.join(std::move(temp));
	}
}

void EntityManager::ScanUVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
	auto data = variableSet.getData(variableId);
	auto data3 = variableSet.getData(variableId3);
	variableSet.removeRowsWith(variableId);
	variableSet.removeRowsWith(variableId3);
	for (auto dat : data) {
		if (dat.empty()) continue;
		for (auto dat3 : data3) {
			if (dat3.empty()) continue;
			VariableSet temp = variableSet.split();

			ScanEVR(std::move(temp), model::Subject(model::Subject::Type::ENTITYREF, dat.dataPointer()->toString()),
				std::move(variableId2),
				dat3.dataPointer()->toObject(), std::move(metaVar), std::move(settings));

			for (auto iter = temp.begin(); iter != temp.end(); iter++) {
				(*iter)[variableId].reset(dat.dataPointer(), dat.entity(), dat.property());
				(*iter)[variableId3].reset(dat3.dataPointer(), dat3.entity(), dat3.property());
			}

			variableSet.join(std::move(temp));
		}
	}
}

void EntityManager::ScanUUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
	auto data = variableSet.getData(variableId);
	auto data2 = variableSet.getData(variableId2);
	variableSet.removeRowsWith(variableId);
	variableSet.removeRowsWith(variableId2);
	for (auto dat : data) {
		if (dat.empty()) continue;
		for (auto dat2 : data2) {
			if (dat2.empty()) continue;
			VariableSet temp = variableSet.split();

			ScanEPV(std::move(temp), model::Subject(model::Subject::Type::ENTITYREF, dat.dataPointer()->toString()), 
				model::Predicate(model::Predicate::Type::PROPERTY, dat2.dataPointer()->toString()), 
				variableId3, std::move(metaVar), std::move(settings));

			for (auto iter = temp.begin(); iter != temp.end(); iter++) {
				(*iter)[variableId].reset(dat.dataPointer(), dat.entity(), dat.property());
				(*iter)[variableId2].reset(dat2.dataPointer(), dat2.entity(), dat2.property());
			}

			variableSet.join(std::move(temp));
		}
	}
}

void EntityManager::ScanUUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {
  // does nothing!
}



void EntityManager::ScanMVV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {

}

void EntityManager::ScanMVU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {

}

void EntityManager::ScanMUV(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {

}

void EntityManager::ScanMUU(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, unsigned int variableId3, const std::string&& metaVar, const QuerySettings&& settings) const {

}



// $a $b Record
void EntityManager::ScanVVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {
  //no variables have been used
  for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

    auto rows = ScanHelp1(std::move(variableSet),
                          std::move(iter->second), std::move(iter->second->properties()), variableId2, std::move(object), std::move(metaVar), std::move(settings));

    for (auto row : rows) {
      auto newRecord = std::make_shared<model::types::EntityRef>(iter->second->getHandle());
      newRecord->Init(100);
      variableSet.add(std::move(variableId),
                      newRecord, 0, iter->second->_handle,
                      model::types::SubType::EntityRef, std::move(metaVar), row);
    }
  }
}

void EntityManager::ScanVUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  //only properties are restricted

  auto rows = variableSet.extractRowsWith(variableId2);
  variableSet.removeRowsWith(variableId2);

  for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {
    for (auto row : rows) {

      unsigned int propId = std::dynamic_pointer_cast<model::types::Property, model::types::Base>(row[variableId2].dataPointer())->value();
      if (!iter->second->hasProperty(propId)) continue;
      auto vals = iter->second->getProperty(propId)->baseValues();
      for (auto value : vals) {
        if (value->Equals(object) > 0) {
          auto newRow = VariableSetRow(row);
          auto newRecord = std::make_shared<model::types::EntityRef>(iter->second->getHandle());
          newRow[variableId] = VariableSetValue(newRecord, 0, iter->second->_handle);
          variableSet.add(std::move(newRow));
		  if (settings.canon)break;
        }
      }
    }
  }
}

void EntityManager::ScanUVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  //for (auto iter = variableSet.cbegin(); iter != variableSet.cend(); iter++) {
  for (int i = variableSet.height() - 1; i >= 0; i--) {
    auto iter = (variableSet.begin() + i);

    if ((*iter).at(variableId).empty()) continue;
    Entity::EHandle_t entityId = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter).at(variableId).dataPointer())->value();
    auto entity = _entities.at(entityId);

    auto rows = ScanHelp1(std::move(variableSet), std::move(entity), std::move(entity->properties()), variableId2, std::move(object), std::move(metaVar), std::move(settings));

    //if rows.size() == 0 then no properties matched, remove the row?
    if (rows.size() == 0) {
      variableSet.erase(iter);
    } else {
      for (auto row : rows) {
        auto newRecord = std::make_shared<model::types::EntityRef>(entityId);
        newRecord->Init(100);
        variableSet.add(std::move(variableId),
                        newRecord, 0, entityId,
                        model::types::SubType::EntityRef, std::move(metaVar), row);
      }
    }
  }
}

void EntityManager::ScanUUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  // Only delete rows that match the pattern perfectly... does this make sense?
  for (auto iter = variableSet.begin(); iter != variableSet.end();) {

    if ((*iter)[variableId].empty() || (*iter)[variableId2].empty()) {
      iter++;
      continue;
    }

    Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[variableId].dataPointer())->value();
    auto currentEntity = _entities.at(entityHandle);

    unsigned int propertyId = std::dynamic_pointer_cast<model::types::Property, model::types::Base>((*iter)[variableId2].dataPointer())->value();

    if (currentEntity->hasProperty(propertyId)) {
      if (currentEntity->meetsCondition(propertyId, std::move(object)).size() > 0) {
        iter++;
        continue;
      }
    }

    iter = variableSet.erase(iter);
  }
}


void EntityManager::ScanMVR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {
  //TODO: does this work if two properties match?
  for (auto iter = variableSet.cbegin(); iter != variableSet.cend(); iter++) {
    if (!(*iter).at(variableId).dataPointer()) continue;
    auto vsv = (*iter).at(variableId);
    std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
    auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
    //check if val meets the property
    for(auto prop : val->properties()) {
      for (auto val : prop.second->baseValues()) {
        if (val->Equals(object) > 0) {
          auto newRecord = std::make_shared<model::types::Property>(prop.first, this, 0);
          newRecord->Init(100);
          variableSet.addToMetaRefRow(vsv.metaRef(), variableId2, newRecord, prop.first, vsv.entity());
          break;
        }
      }
    }
  }
}

void EntityManager::ScanMUR(VariableSet&& variableSet, unsigned int variableId, unsigned int variableId2, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  //TODO what if there are multiple values against the property?

  for (int i = variableSet.height() - 1; i >= 0; i--) {
    auto iter = (variableSet.begin() + i);

    if (!(*iter).at(variableId).dataPointer()) continue;
    auto vsv = (*iter).at(variableId);
    std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
    auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
    //check if val meets the property
    bool found = false;
    for (auto prop : val->properties()) {
      for (auto val : prop.second->baseValues()) {
        if (val->Equals(object) > 0) {
          found = true;
          break;
        }
        if(found)break;
      }
    }
    if (!found)variableSet.erase(iter);
  }
}


// $a Property $c
void EntityManager::ScanVPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  const unsigned int propertyId = this->getPropertyId(predicate.value);

  //the variable has not been used, add all the values found!
  for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

    auto rows = ScanHelp2(std::move(variableSet),
                          model::Subject(model::Subject::Type::ENTITYREF,
                                         std::to_string(iter->first)), std::move(predicate), variableId2, std::move(metaVar), std::move(settings));

    for (auto row : rows) {
      auto newRecord = std::make_shared<model::types::EntityRef>(iter->second->getHandle());
      newRecord->Init(100);
      variableSet.add(std::move(variableId),
                      newRecord, propertyId, iter->second->_handle,
                      model::types::SubType::EntityRef, std::move(metaVar), row);
    }

  }
}

void EntityManager::ScanVPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  const unsigned int propertyId = this->getPropertyId(predicate.value);

  for (auto iter = variableSet.begin(); iter != variableSet.end();) {
    if (iter->at(variableId2).empty()) {
      iter++;
      continue;
    }

    auto val = iter->at(variableId2).dataPointer();

    bool removeRow = true;
    for (auto entityIter = _entities.cbegin(); entityIter != _entities.cend(); entityIter++) {

      auto matchValues = entityIter->second->meetsCondition(propertyId, std::move(val));

      if (matchValues.size() > 0) {
        removeRow = false;

        auto newRecord = std::make_shared<model::types::EntityRef>(entityIter->second->getHandle());
        newRecord->Init(100);

        (*iter)[variableId] = VariableSetValue(newRecord->Clone(), propertyId, entityIter->second->getHandle());

        for (auto matchIter = matchValues.begin() + 1; matchIter < matchValues.end(); matchIter++) {

          auto newRow = variableSet.add(std::move(variableId), newRecord->Clone(), propertyId, entityIter->second->getHandle(),
                                        model::types::SubType::EntityRef, std::move(metaVar));

          variableSet.add(std::move(variableId2), iter->at(variableId2).dataPointer()->Clone(), propertyId,
                          entityIter->second->getHandle(), iter->at(variableId2).dataPointer()->subtype(), std::move(metaVar), newRow);
        }
      }
    }

    if (removeRow) {
      iter = variableSet.erase(iter);
    } else {
      iter++;
    }
  }
}

void EntityManager::ScanUPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  const unsigned int propertyId = this->getPropertyId(predicate.value);

  for (auto iter = variableSet.begin(); iter != variableSet.end(); ) {
    if ((*iter)[variableId].empty()) {
      iter = variableSet.erase(iter);
      continue;
    }
    Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[variableId].dataPointer())->value();
    auto currentEntityIter = _entities.find(entityHandle);
    std::shared_ptr<Entity> currentEntity;
    if (currentEntityIter != _entities.end()) {
      currentEntity = _entities.at(entityHandle);
    } else {
      throw std::runtime_error("Attempted to look up a non-existent entity");
    }
    if (!currentEntity->hasProperty(propertyId)) {
      iter = variableSet.erase(iter);
    } else {
      iter++;
    }
  }

  //TODO: but what about the type of the new data being added? :/
  //for (auto iter = variableSet.begin(); iter != variableSet.end(); iter++) {
  for (int i = variableSet.height() - 1; i >= 0; i--) {
    auto iter = (variableSet.begin() + i);

    Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[variableId].dataPointer())->value();
    auto currentEntityIter = _entities.find(entityHandle);
    std::shared_ptr<Entity> currentEntity;

    if (currentEntityIter != _entities.end()) {
      currentEntity = _entities.at(entityHandle);
    } else {
      throw std::runtime_error("Attempted to look up a non-existent entity");
    }

    auto values = currentEntity->getProperty(propertyId)->baseValues();

    //*(iter->begin() + variableId2) = VariableSetValue(values[0]->Clone(), 0, 0);
    variableSet.add(variableId2, values[0]->Clone(), propertyId, entityHandle, values[0]->subtype(), std::move(metaVar), i);

    for (size_t valId = 1; valId < values.size(); valId++) {

      auto iter = (variableSet.begin() + i);
      VariableSetRow newVec(*iter);
      newVec.ranking(newVec.ranking()-static_cast<int>(newVec.at(variableId2).dataPointer()->confidence()));
      unsigned int newRowId = variableSet.add(std::move(newVec));
      variableSet.add(variableId2, values[valId]->Clone(), propertyId, entityHandle, values[valId]->subtype(), std::move(metaVar), newRowId);
	  if (settings.canon)break;
    }
  }
}

void EntityManager::ScanUPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  const unsigned int propertyId = this->getPropertyId(predicate.value);

  for (auto iter = variableSet.begin(); iter != variableSet.end(); ) {

    Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[variableId].dataPointer())->value();
    auto currentEntityIter = _entities.find(entityHandle);
    std::shared_ptr<Entity> currentEntity;
    if (currentEntityIter != _entities.end()) {
      currentEntity = _entities.at(entityHandle);
    } else {
      throw std::runtime_error("Attempted to look up a non-existent entity");
    }

    if (currentEntity->meetsCondition(propertyId, model::Object(model::Object::Type::STRING, (*iter)[variableId2].dataPointer()->toString())).size() == 0) {
      iter = variableSet.erase(iter);
    } else {
      iter++;
    }
  }
}


void EntityManager::ScanMPV(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  const unsigned int propertyId = this->getPropertyId(predicate.value);

  for (auto iter = variableSet.cbegin(); iter != variableSet.cend(); iter++) {
    if (!(*iter).at(variableId).dataPointer()) continue;
    auto vsv = (*iter).at(variableId);
    std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
    auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
    //check if val meets the property
    if (val->hasProperty(propertyId)) {
      auto subprop = val->getProperty(propertyId);
      //TODO: should be all values
      auto valueToStore = subprop->baseTop()->Clone();
      variableSet.addToMetaRefRow(vsv.metaRef(), variableId2, subprop->baseTop()->Clone(), propertyId, vsv.entity());
    }
  }
}

void EntityManager::ScanMPU(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  const unsigned int propertyId = this->getPropertyId(predicate.value);

  for (int i = variableSet.height() - 1; i >= 0; i--) {
    auto iter = (variableSet.begin() + i);

    if (!(*iter).at(variableId).dataPointer()) continue;
    auto vsv = (*iter).at(variableId);
    std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
    auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
    //check if val meets the property
    if (val->hasProperty(propertyId)) {
      auto subprop = val->getProperty(propertyId);
      if (val->meetsCondition(propertyId, (*iter).at(variableId2).dataPointer()).size() > 0) {
        continue;
      }
    }
    variableSet.erase(iter);
  }
}



// $a Property Value
void EntityManager::ScanVPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::Undefined);

  for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

    auto currentEntity = iter->second;
    std::vector<std::shared_ptr<model::types::Base>> matches;

    matches = currentEntity->meetsCondition(propertyId, std::move(object));
    for (auto match : matches) {
      auto newValue = std::make_shared<model::types::EntityRef>(currentEntity->getHandle());
      newValue->Init(match->confidence());
      newValue->OrderingId(match->OrderingId());
      variableSet.add(std::move(variableId), newValue, propertyId, currentEntity->getHandle(), model::types::SubType::EntityRef, std::move(metaVar));
    }
  }
}

void EntityManager::ScanUPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::Undefined);

  for (auto iter = variableSet.begin(); iter != variableSet.end(); ) {
    Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[variableId].dataPointer())->value();
    auto currentEntity = _entities.at(entityHandle);
    auto matches = currentEntity->meetsCondition(propertyId, std::move(object));
    if (matches.size() == 0) {
      iter = variableSet.erase(iter);
    } else {
      iter++;
    }
  }
}

void EntityManager::ScanMPR(VariableSet&& variableSet, unsigned int variableId, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::Undefined);

  for (auto iter = variableSet.cbegin(); iter != variableSet.cend(); iter++) {
    if (!(*iter).at(variableId).dataPointer()) continue;
    auto vsv = (*iter).at(variableId);
    std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
    auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
    //check if val meets the property
    auto matches = val->meetsCondition(propertyId, std::move(object));
    if (matches.size() == 0) {
      //delete associated answers
      variableSet.removeMetaRefs(vsv.metaRef());
    }
  }
}

// Entity $b $c
void EntityManager::ScanEVV(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
  auto currentEntity = _entities.at(entityRef);

  for (auto propertyPair : currentEntity->properties()) {
    auto vals = propertyPair.second->baseValues();

    for (auto value : vals) {

      auto newRecord = std::make_shared<model::types::Property>(propertyPair.first, this, 0);
      newRecord->Init(100);
      auto rowId = variableSet.add(std::move(variableId),
                                   newRecord, 0, 0,
                                   model::types::SubType::PropertyReference, std::move(metaVar));

      variableSet.add(std::move(variableId2),
                      value->Clone(), 0, 0, std::move(_propertyTypes.at(propertyPair.first)), std::move(metaVar), rowId);
	  if (settings.canon)break;
    }
  }
}

void EntityManager::ScanEVU(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
  auto currentEntity = _entities.at(entityRef);

  for (int i = variableSet.height() - 1; i >= 0; i--) {
    auto iter = (variableSet.begin() + i);

    if ((*iter)[variableId2].empty()) continue;

    bool found = false;

    for (auto propertyPair : currentEntity->properties()) {
      auto met = currentEntity->meetsCondition(propertyPair.first, model::Object(model::Object::Type::STRING, (*iter)[variableId2].dataPointer()->toString()));
      if (met.size() > 0) {
        auto newRecord = std::make_shared<model::types::Property>(propertyPair.first, this, 0);
        newRecord->Init(100);
        (*iter)[variableId] = VariableSetValue(newRecord, 0, 0);
        found = true;
      }
    }

    if (!found) {
      variableSet.erase(iter);
    }
  }
}

void EntityManager::ScanEUV(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
  auto currentEntity = _entities.at(entityRef);

  std::map<unsigned int, std::shared_ptr<EntityProperty>> iterableProperties;
  auto data = variableSet.getData(variableId);
  std::transform(data.begin(), data.end(), std::inserter(iterableProperties, iterableProperties.begin()), [&](VariableSetValue b) {
    std::shared_ptr<model::types::Property> prop = std::dynamic_pointer_cast<model::types::Property, model::types::Base>(b.dataPointer());
    return std::pair<unsigned int, std::shared_ptr<EntityProperty>>(prop->value(), currentEntity->getProperty(prop->value()));
  });

  for (auto propertyPair : iterableProperties) {
    auto vals = propertyPair.second->baseValues();

    for (auto value : vals) {

      auto newRecord = std::make_shared<model::types::Property>(propertyPair.first, this, 0);
      auto rowId = variableSet.add(std::move(variableId),
                                   newRecord, 0, 0,
                                   model::types::SubType::PropertyReference, std::move(metaVar));

      variableSet.add(std::move(variableId2),
                      value->Clone(), 0, 0, std::move(_propertyTypes.at(propertyPair.first)), std::move(metaVar), rowId);
	  if (settings.canon)break;
    }
  }
}

void EntityManager::ScanEUU(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, unsigned int variableId2, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
  auto currentEntity = _entities.at(entityRef);

  std::map<unsigned int, std::shared_ptr<EntityProperty>> iterableProperties;
  auto data = variableSet.getData(variableId);
  std::transform(data.begin(), data.end(), std::inserter(iterableProperties, iterableProperties.begin()), [&](VariableSetValue b) {
    std::shared_ptr<model::types::Property> prop = std::dynamic_pointer_cast<model::types::Property, model::types::Base>(b.dataPointer());
    return std::pair<unsigned int, std::shared_ptr<EntityProperty>>(prop->value(), currentEntity->getProperty(prop->value()));
  });

  for (int i = variableSet.height() - 1; i >= 0; i--) {
    auto iter = (variableSet.begin() + i);

    if ((*iter)[variableId2].empty()) continue;

    bool found = false;

    for (auto propertyPair : iterableProperties) {
      auto met = currentEntity->meetsCondition(propertyPair.first, model::Object(model::Object::Type::STRING, (*iter)[variableId2].dataPointer()->toString()));
      if (met.size() > 0) {
        auto newRecord = std::make_shared<model::types::Property>(propertyPair.first, this, 0);
        (*iter)[variableId] = VariableSetValue(newRecord, 0, 0);
        found = true;
      }
    }

    if (!found) {
      variableSet.erase(iter);
    }
  }
}

// Entity $b Value - Scan 6
void EntityManager::ScanEVR(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

  if (EntityExists(entityRef)) {
    auto entity = _entities.at(entityRef);
    ScanHelp1(std::move(variableSet), std::move(entity), std::move(entity->properties()), variableId, std::move(object), std::move(metaVar), std::move(settings));
  }
}

void EntityManager::ScanEUR(VariableSet&& variableSet, const model::Subject&& subject, unsigned int variableId, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {
  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

  auto currentEntity = _entities.at(entityRef);

  for (auto iter = variableSet.begin(); iter != variableSet.end();) {
    if ((*iter)[variableId].empty()) {
      iter++;
      continue;
    }
    std::shared_ptr<model::types::Property> prop = std::dynamic_pointer_cast<model::types::Property, model::types::Base>((*iter)[variableId].dataPointer());
    if (currentEntity->meetsCondition(prop->value(), std::move(object)).size() == 0) {
      iter = variableSet.erase(iter);
    } else {
      iter++;
    }
  }
}

// Entity Property $c - Scan 5
void EntityManager::ScanEPV(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const {
  ScanHelp2(std::move(variableSet), std::move(subject), std::move(predicate), variableId, std::move(metaVar), std::move(settings));
}

void EntityManager::ScanEPU(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const {

  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

  //get the property id
  // Jonathat: Passing Undefined to ckip type checking, since we don't know
  // what the type is yet.
  const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::Undefined);

  if (EntityExists(entityRef)) {
    auto currentEntity = _entities.at(entityRef);

    for (auto iter = variableSet.begin(); iter != variableSet.end();) {

      if ((*iter)[variableId].empty()) {
        iter++;
        continue;
      }

      if (currentEntity->hasProperty(propertyId)) {
        if (currentEntity->meetsCondition(propertyId, std::move((*iter)[variableId].dataPointer())).size() > 0) {
          iter++;
          continue;
        }
      }

      iter = variableSet.erase(iter);
    }
  }
}

// Entity Property Value - No scan :(
void EntityManager::ScanEPR(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {

  if (metaVar == "") return;

  Entity::EHandle_t entityId = std::stoull(subject.value);
  unsigned int propertyId = std::stoul(subject.value);
  if (EntityExists(entityId)) {
    auto entity = _entities.at(entityId);
    if (entity->hasProperty(propertyId)) {

      //TODO: what if more than 1 result?

      auto results = entity->meetsCondition(propertyId, std::move(object));
      if (results.size() > 0) {
        //yay matches
        variableSet.add(variableSet.indexOf(metaVar),
                        std::make_shared<model::types::ValueRef>(entityId, propertyId, results[0]->OrderingId()),
                        propertyId, entityId, model::types::SubType::ValueReference, "");
      }
    }
  }
}

std::vector<unsigned int> EntityManager::ScanHelp1(VariableSet&& variableSet, const std::shared_ptr<Entity>&& entity,
    const std::map<unsigned int, std::shared_ptr<EntityProperty>>&& properties, unsigned int variableId,
    const model::Object&& object, const std::string&& metaVar, const QuerySettings&& settings) const {
  //get the entity handle
  //Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
  std::vector<unsigned int> rowsAdded;

  for (auto propertyPair : properties) {
    auto vals = propertyPair.second->baseValues();
    for (auto value : vals) {
      if (value->Equals(object) > 0) {
        auto newRecord = std::make_shared<model::types::Property>(propertyPair.first, this, 0);
        newRecord->Init(100);
        rowsAdded.push_back(variableSet.add(std::move(variableId),
                                            newRecord, entity->getHandle(), propertyPair.first,
                                            model::types::SubType::PropertyReference, std::move(metaVar)));
		if (settings.canon)break;
      }
    }
  }

  return rowsAdded;
}

std::vector<unsigned int> EntityManager::ScanHelp2(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, unsigned int variableId, const std::string&& metaVar, const QuerySettings&& settings) const {
  //TODO: Check variable types
  //TODO : Linkify

  //get the entity handle
  Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
  std::vector<unsigned int> rowsAdded;

  //get the property id
  // Jonathat: Passing Undefined to ckip type checking, since we don't know
  // what the type is yet.
  const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::Undefined);

  if (EntityExists(entityRef)) {
    auto currentEntity = _entities.at(entityRef);
    if (currentEntity->hasProperty(propertyId)) {
      for (auto value : currentEntity->getProperty(propertyId)->baseValues()) {
        rowsAdded.push_back(variableSet.add(std::move(variableId), value->Clone(), propertyId, entityRef, std::move(_propertyTypes.at(propertyId)), std::move(metaVar)));
		if (settings.canon)break;
      }
    }
  }

  return rowsAdded;
}
