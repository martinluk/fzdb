#include "./EntityManager.h"
#include <cassert>
#include <set>
#include <sstream>
#include "../FileSystem.h"
#include "GraphSerialiser.h"
#include "../Util.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <cassert>
#include "../Parser.h"
#include "spdlog/spdlog.h"
#include "../Util.h"

#include "./types/Base.h"
#include "./types/ValueRef.h"
#include "./types/String.h"
#include "./types/EntityRef.h"
#include "./types/Int.h"
#include "./types/Date.h"
#include "./types/Property.h"
#include "singletons.h"

//$a <predicate> <object>
void EntityManager::Scan1(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const model::Object&& object, const std::string&& metaVar) const {
    
    //get the property id
    // Jonathan: Changed this to pass an undefined type, which skips type checking.
    // We don't know what the type is in advance!
    unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::TypeUndefined);

    //the variable has been used before, we only need to iterate over valid values from before
    if (variableSet.used(variableName)) {

        unsigned char varIndex = variableSet.indexOf(variableName);

        //in this scan we can only remove variables from variableset
        switch (variableSet.typeOf(variableName)) {
            case model::types::SubType::TypeEntityRef: {                

                variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
                    [&, this, varIndex](std::vector<VariableSetValue> row) {
                    Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex].dataPointer())->value();
                    auto currentEntity = _entities.at(entityHandle);
                    auto matches = currentEntity->meetsCondition(propertyId, std::move(object));
                    return matches.size() == 0;
                }), variableSet.getData()->end());

                break;
            }
            
            case model::types::SubType::ValueReference: {

                for (auto iter = variableSet.getData()->cbegin(); iter != variableSet.getData()->cend(); iter++) {
                    if (!(*iter)[varIndex].dataPointer()) continue;
                    auto vsv = (*iter)[varIndex];
                    std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
                    auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
                    //check if val meets the property
                    auto matches = val->meetsCondition(propertyId, std::move(object));
                    if (matches.size() == 0) {
                        //delete associated answers
                        variableSet.removeMetaRefs(vsv.metaRef());
                    }
                }
                
                break;
            }

            default: {
                throw std::runtime_error("The first element in a triple MUST be an entity or a meta variable!");
            }
        }
        return;
    }

    //the variable has not been used, add all the values found!
    for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

        auto currentEntity = iter->second;
        std::vector<std::shared_ptr<model::types::Base>> matches;

        matches = currentEntity->meetsCondition(propertyId, std::move(object));
        if (matches.size() > 0) {
            VariableSetValue vsv = VariableSetValue(std::make_shared<model::types::EntityRef>(currentEntity->getHandle(), 0), 0, currentEntity->getHandle());
                
            if (metaVar != "") {

                unsigned int metaRef = variableSet.getMetaRef();
                vsv.metaRef(metaRef);

                for (auto match : matches) {
                    //TODO:
                    //is it necessery to repeat the entity and property handles?
                    auto valueRef = std::make_shared<model::types::ValueRef>(currentEntity->getHandle(), propertyId, match->OrderingId());
                    auto vsv2 = VariableSetValue(valueRef, propertyId, currentEntity->getHandle());
                    vsv2.metaRef(metaRef);
                    variableSet.add(std::move(metaVar), std::move(vsv2), model::types::SubType::ValueReference);
                }                    
            }

            variableSet.add(std::move(variableName), std::move(vsv), model::types::SubType::TypeEntityRef);
        }   
    }
}


//$a <predicate> $b
void EntityManager::Scan2(VariableSet&& variableSet, const std::string variableName, const model::Predicate&& predicate, const std::string variableName2, const std::string&& metaVar) const {
    
    //get the property id
    const unsigned int propertyId = this->getPropertyId(predicate.value);

    //TODO: consider the case where variableName2 is already in variableSet

    //the variable has been used before, we only need to iterate over valid values from before
    if (variableSet.used(variableName)) {

		unsigned char varIndex = variableSet.indexOf(variableName),
			varIndex2 = variableSet.indexOf(variableName2);

		if (variableSet.used(variableName2)) {
			// this cannot add any new data, only remove it
			if (variableSet.typeOf(variableName) != model::types::SubType::TypeEntityRef) {
				throw std::runtime_error("Not yet implemented :/");
			}

			variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
				[&, this, varIndex](std::vector<VariableSetValue> row) {
				Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex].dataPointer())->value();
				auto currentEntityIter = _entities.find(entityHandle);
				std::shared_ptr<Entity> currentEntity;
				if (currentEntityIter != _entities.end()) {
					currentEntity = _entities.at(entityHandle);
				}
				else {
					throw std::runtime_error("Attempted to look up a non-existent entity");
				}

				return currentEntity->meetsCondition(propertyId, model::Object(model::Object::Type::STRING, row[varIndex2].dataPointer()->toString())).size() == 0;
			}), variableSet.getData()->end());
		}
		else {			

			switch (variableSet.typeOf(variableName)) {

			case model::types::SubType::TypeEntityRef: {

				variableSet.getData()->erase(std::remove_if(variableSet.getData()->begin(), variableSet.getData()->end(),
					[&, this, varIndex](std::vector<VariableSetValue> row) {
					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>(row[varIndex].dataPointer())->value();
					auto currentEntityIter = _entities.find(entityHandle);
					std::shared_ptr<Entity> currentEntity;
					if (currentEntityIter != _entities.end()) {
						currentEntity = _entities.at(entityHandle);
					}
					else {
						throw std::runtime_error("Attempted to look up a non-existent entity");
					}
					return !currentEntity->hasProperty(propertyId);
				}), variableSet.getData()->end());

				//TODO: but what about the type of the new data being added? :/
				//for (auto iter = variableSet.getData()->begin(); iter != variableSet.getData()->end(); iter++) {
				for (int i = variableSet.getData()->size() - 1; i >= 0; i--) {
					auto iter = (variableSet.getData()->begin() + i);

					Entity::EHandle_t entityHandle = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[varIndex].dataPointer())->value();
					auto currentEntityIter = _entities.find(entityHandle);
					std::shared_ptr<Entity> currentEntity;

					if (currentEntityIter != _entities.end()) {
						currentEntity = _entities.at(entityHandle);
					}
					else {
						throw std::runtime_error("Attempted to look up a non-existent entity");
					}

					auto values = currentEntity->getProperty(propertyId)->baseValues();

					*(iter->begin() + varIndex2) = VariableSetValue(values[0]->Clone(), 0, 0);

					for (size_t valId = 1; valId < values.size(); valId++) {
						std::vector<VariableSetValue> newVec;
						for (size_t j = 0; j < iter->size(); j++) {
							if (j != varIndex2) {
								newVec.push_back((*iter)[j]);
							}
							else {
								newVec.push_back(VariableSetValue(values[valId]->Clone(), 0, 0));
							}
						}
						variableSet.getData()->emplace(iter + 1, newVec);
					}
				}

				break;
				} // /model::types::SubType::TypeEntityRef

			case model::types::SubType::ValueReference: {

				for (auto iter = variableSet.getData()->cbegin(); iter != variableSet.getData()->cend(); iter++) {
					if (!(*iter)[varIndex].dataPointer()) continue;
					auto vsv = (*iter)[varIndex];
					std::shared_ptr<model::types::ValueRef> valueRef = std::dynamic_pointer_cast<model::types::ValueRef, model::types::Base>(vsv.dataPointer());
					auto val = dereference(valueRef->entity(), valueRef->prop(), valueRef->value());
					//check if val meets the property
					if (val->hasProperty(propertyId)) {
						auto subprop = val->getProperty(propertyId);
						//TODO: should be all values
						auto valueToStore = subprop->baseTop()->Clone();
						variableSet.addToMetaRefRow(vsv.metaRef(), varIndex2, VariableSetValue(subprop->baseTop()->Clone(), propertyId, vsv.entity()));
					}
				}
				break;
			}

			}
		}
        return;
    }

    //the variable has not been used, add all the values found!
    for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

        auto rows = Scan5(std::move(variableSet),
            model::Subject(model::Subject::Type::ENTITYREF, 
                std::to_string(iter->first)), std::move(predicate), variableName2, std::move(metaVar));

        for (auto row : rows) {
            variableSet.add(std::move(variableName),
                VariableSetValue(std::make_shared<model::types::EntityRef>(iter->second->getHandle(), 0), propertyId, iter->second->_handle),
                model::types::SubType::TypeEntityRef, row);
        }

    }
}

//$a $b "Value"
void EntityManager::Scan3(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const model::Object&& object, const std::string&& metaVar) const {
	
	auto varId = variableSet.indexOf(variableName);
	auto varId2 = variableSet.indexOf(variableName2);

	if (variableSet.used(variableName)) {
		//entities or metavars are restricted
		

		if (variableSet.used(variableName2)) {
			//properties are restricted

			if (variableSet.typeOf(variableName) == model::types::SubType::TypeEntityRef) {

			}
		}
		else {
			//only entities are restricted
			for (auto iter = variableSet.getData()->cbegin(); iter != variableSet.getData()->cend(); iter++) {
				if ((*iter)[varId].empty()) continue;
				Entity::EHandle_t entityId = std::dynamic_pointer_cast<model::types::EntityRef, model::types::Base>((*iter)[varId].dataPointer())->value();

				auto rows = Scan6(std::move(variableSet),
					model::Subject(model::Subject::Type::ENTITYREF,	(*iter)[varId].dataPointer()->toString()), variableName2, std::move(object), std::move(metaVar));

				for (auto row : rows) {
					variableSet.add(std::move(variableName),
						VariableSetValue(std::make_shared<model::types::EntityRef>(entityId, 0), 0, entityId),
						model::types::SubType::TypeEntityRef, row);
				}
			}
		}
	}
	else {
		if (variableSet.used(variableName2)) {
			auto varId2 = variableSet.indexOf(variableName2);
			//only properties are restricted

			auto rows = variableSet.extractRowsWith(variableName2);
			variableSet.removeRowsWith(variableName2);

			for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {
				for (auto row : rows) {

					unsigned int propId = std::dynamic_pointer_cast<model::types::Property, model::types::Base>(row[varId2].dataPointer())->value();
					if (!iter->second->hasProperty(propId)) continue;
					auto vals = iter->second->getProperty(propId)->baseValues();
					for (auto value : vals) {
						if (value->Equals(object)) {
							auto newRow = std::vector<VariableSetValue>(row);
							newRow[varId] = VariableSetValue(std::make_shared<model::types::EntityRef>(iter->second->getHandle(), 0), 0, iter->second->_handle);
							variableSet.add(std::move(newRow));
						}
					}
				}
			}

		}
		else {
			//no variables have been used
			for (auto iter = _entities.cbegin(); iter != _entities.cend(); iter++) {

				auto rows = Scan6(std::move(variableSet),
					model::Subject(model::Subject::Type::ENTITYREF,
						std::to_string(iter->first)), variableName2, std::move(object), std::move(metaVar));

				for (auto row : rows) {
					variableSet.add(std::move(variableName),
						VariableSetValue(std::make_shared<model::types::EntityRef>(iter->second->getHandle(), 0), 0, iter->second->_handle),
						model::types::SubType::TypeEntityRef, row);
				}
			}
		}
	}    
}

//$a $b $c
void EntityManager::Scan4(VariableSet&& variableSet, const std::string variableName, const std::string variableName2, const std::string variableName3, const std::string&& metaVar) const {
    
    //TODO: Check variable types

    //TODO: Cover cases where the variables have been used before :/
    if (variableSet.contains(variableName) || variableSet.contains(variableName2) || variableSet.contains(variableName3)) {
        return;
    }

    //If no variables have been used so far, get EVERYTHING!

    for (auto entity : _entities) {
        for (auto prop : entity.second->properties()) {

            variableSet.add(std::move(variableName), 
                VariableSetValue(std::make_shared<model::types::EntityRef>(entity.first, 0), prop.first, entity.first),
                model::types::SubType::TypeEntityRef);

            variableSet.add(std::move(variableName2),
                VariableSetValue(std::make_shared<model::types::Property>(prop.first, 0), prop.first, entity.first),
                model::types::SubType::PropertyReference);

            auto type = _propertyTypes.at(prop.first);
            std::shared_ptr<model::types::Base> val = entity.second->getProperty(prop.first)->baseValue(0)->Clone();
            variableSet.add(std::move(variableName3),
                VariableSetValue(std::move(val), prop.first, entity.first),
                std::move(type));
        }
    }
}

//entity <predicate> $c
std::vector<unsigned int> EntityManager::Scan5(VariableSet&& variableSet, const model::Subject&& subject, const model::Predicate&& predicate, const std::string variableName, const std::string&& metaVar) const {
    
    //TODO: Check variable types
    //TODO : Linkify

    //get the entity handle
    Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
    std::vector<unsigned int> rowsAdded;

    //get the property id
    // Jonathat: Passing TypeUndefined to ckip type checking, since we don't know
    // what the type is yet.
    const unsigned int propertyId = this->getPropertyName(predicate.value, model::types::SubType::TypeUndefined);

    if (EntityExists(entityRef)) {
        auto currentEntity = _entities.at(entityRef);
        if (currentEntity->hasProperty(propertyId)) {
            for (auto value : currentEntity->getProperty(propertyId)->baseValues()) {

				auto vsv = VariableSetValue(value->Clone(), propertyId, entityRef);

				if (metaVar != "") {

					unsigned int metaRef = variableSet.getMetaRef();
					vsv.metaRef(metaRef);

						//TODO:
						//is it necessery to repeat the entity and property handles?
						auto valueRef = std::make_shared<model::types::ValueRef>(currentEntity->getHandle(), propertyId, value->OrderingId());
						auto vsv2 = VariableSetValue(valueRef, propertyId, currentEntity->getHandle());
						vsv2.metaRef(metaRef);
						variableSet.add(std::move(metaVar), std::move(vsv2), model::types::SubType::ValueReference);
				}

                rowsAdded.push_back(variableSet.add(std::move(variableName), std::move(vsv), std::move(_propertyTypes.at(propertyId))));
            }            
        }
    }

    return rowsAdded;
}

// entity $b value
std::vector<unsigned int> EntityManager::Scan6(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const model::Object&& object, const std::string&& metaVar) const {

    //TODO: Check variable types
    //TODO : Linkify

    //get the entity handle
    Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());
    std::vector<unsigned int> rowsAdded;

    if (EntityExists(entityRef)) {
        auto entity = _entities.at(entityRef);
        //iterate through properties
        for (auto propertyPair : entity->properties()) {
            auto vals = propertyPair.second->baseValues();
            for (auto value : vals) {
                if (value->Equals(object)) {
                    rowsAdded.push_back(variableSet.add(std::move(variableName),
                        VariableSetValue(std::make_shared<model::types::Property>(propertyPair.first, 0), 0, 0),
                        model::types::SubType::PropertyReference));
                }
            }
        }
    }

    return rowsAdded;
}

// entity $b $c
void EntityManager::Scan7(VariableSet&& variableSet, const model::Subject&& subject, const std::string variableName, const std::string variableName2, const std::string&& metaVar) const {
    
    Entity::EHandle_t entityRef = std::atoll(subject.value.c_str());

    if (EntityExists(entityRef)) {
        auto entity = _entities.at(entityRef);

        //if variableName is already set
        std::map<unsigned int, std::shared_ptr<EntityProperty>> iterableProperties;
        if (variableSet.used(variableName)) {
            auto data = variableSet.getData(variableName);
            std::transform(data.begin(), data.end(), std::inserter(iterableProperties, iterableProperties.begin()), [&](VariableSetValue b) {
                std::shared_ptr<model::types::Int> prop = std::dynamic_pointer_cast<model::types::Int, model::types::Base>(b.dataPointer());
                return std::pair<unsigned int, std::shared_ptr<EntityProperty>>(prop->value(), entity->getProperty(prop->value()));
            });
        }
        else {
            iterableProperties = entity->properties();
        }

		if (variableSet.used(variableName2)) {
			auto varNameId = variableSet.indexOf(variableName);
			auto varName2Id = variableSet.indexOf(variableName2);

			for (int i = variableSet.getData()->size() - 1; i >= 0; i--) {
				auto iter = (variableSet.getData()->begin() + i);

				if ((*iter)[varName2Id].empty()) continue;

				bool found = false;

				for (auto propertyPair : iterableProperties) {
					auto met = entity->meetsCondition(propertyPair.first, model::Object(model::Object::Type::STRING, (*iter)[varName2Id].dataPointer()->toString()));
					if (met.size() > 0) {
						(*iter)[varNameId] = VariableSetValue(std::make_shared<model::types::Int>(propertyPair.first, 0), 0, 0);
						found = true;
					}
				}

				if (!found) {
					variableSet.getData()->erase(iter);
				}

				/*if ((*iter)[varName2Id].dataPointer()->Equals(*vals[0].get())) {

				}

				auto rows = variableSet.find(variableName2, value->toString());
				for (auto rowId : rows) {
				variableSet.add(std::move(variableName),
				VariableSetValue(std::make_shared<model::types::Int>(propertyPair.first, 0), 0, 0),
				std::move(_propertyTypes.at(propertyPair.first)), rowId);
				}*/
			}
		}
		else {

			//iterate through properties
			for (auto propertyPair : iterableProperties) {
				auto vals = propertyPair.second->baseValues();

				for (auto value : vals) {

					auto rowId = variableSet.add(std::move(variableName),
						VariableSetValue(std::make_shared<model::types::Property>(propertyPair.first, 0), 0, 0),
						model::types::SubType::PropertyReference);

					variableSet.add(std::move(variableName2),
						VariableSetValue(value->Clone(), 0, 0),
						std::move(_propertyTypes.at(propertyPair.first)), rowId);
				}
			}

		}        
    }

}