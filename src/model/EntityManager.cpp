#include "./EntityManager.h"
#include <cassert>
#include <set>

EntityManager::EntityManager()
{
	lastHandle_ = Entity::INVALID_EHANDLE;
}

EntityManager::~EntityManager()
{
	for (EntityMap::iterator it = entities_.begin(); it != entities_.end(); ++it)
	{
		delete it->second;
	}
}

Entity* EntityManager::createEntity()
{
	lastHandle_++;
	assert(lastHandle_ != Entity::INVALID_EHANDLE);

	Entity* e = new Entity(0, lastHandle_);
	entities_.insert(std::pair<Entity::EHandle_t, Entity*>(lastHandle_, e));

	return e;
}

QueryResult EntityManager::BGP(std::vector<model::Triple> conditions)
{
	std::map<std::string, std::vector<std::string>> variables;

	for (int i = 0; i < conditions.size(); i++) {
		auto vars = conditions[i].variables();

		switch (vars.size()) {
		case 0: 
			//if the triple doesn't have any variables, then it doesn't affect the output
			break;
		case 1:
			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			break;
		case 2:

			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[1]);
			}

			if (variables.find(vars[1]) == variables.end()) {
				variables[vars[1]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
			}

			break;
		case 3:

			if (variables.find(vars[0]) == variables.end()) {
				variables[vars[0]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[1]);
				variables[vars[0]].push_back(vars[2]);
			}

			if (variables.find(vars[1]) == variables.end()) {
				variables[vars[1]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
				variables[vars[0]].push_back(vars[2]);
			}

			if (variables.find(vars[2]) == variables.end()) {
				variables[vars[2]] = vars;
			}
			else {
				variables[vars[0]].push_back(vars[0]);
				variables[vars[0]].push_back(vars[1]);
			}

			break;
		}
	}

	std::vector<std::set<std::string>> buckets;
	std::set<std::string> visited;

	for (auto iter = variables.begin(); iter != variables.end(); iter++) {

	}


	return QueryResult();
}

// Basic Graph Pattern
// When presented with a sanatised list of triples finds values for variables that satisfy that condition
void EntityManager::BGP2(std::vector<model::Triple> conditions)
{
	std::vector<Entity::EHandle_t> passed;
	QueryResult result;

	for (auto iter = entities_.cbegin(); iter != entities_.cend(); iter++) {

		Entity* currentEntity = iter->second;
		std::map<std::string, std::string> variableAssignments;
		bool pass = true;
		
		for (auto conditionsIter = conditions.cbegin(); conditionsIter != conditions.end(); conditionsIter++) {

			if (conditionsIter->subject.type == model::Subject::Type::ENTITYREF && conditionsIter->object.type == model::Object::Type::VARIABLE) {
				
			}

			if (conditionsIter->subject.type == model::Subject::Type::VARIABLE && ((int)conditionsIter->object.type & model::Object::VALUE_MASK)) {

				//get the property id
				unsigned int propertyId = _propertyNames[conditionsIter->predicate.value];

				//if the current entity has the property
				if (currentEntity->hasProperty(propertyId)) {
					
					bool conditionIsTrue = false;

					//match based on type in the triple
					switch (conditionsIter->object.type) {
					case model::Object::Type::STRING: {
						std::vector<model::types::String> val = currentEntity->getProperty<model::types::String>(propertyId)->values();
						conditionIsTrue = val[0].value() == conditionsIter->object.value;
						break;
					}
					case model::Object::Type::INT: {
						std::vector<model::types::Int> val = currentEntity->getProperty<model::types::Int>(propertyId)->values();
						conditionIsTrue = atoi(conditionsIter->object.value.c_str()) == val[0].value();
						break;
					}
					}					

					if (conditionIsTrue) {

						if (variableAssignments.find(conditionsIter->subject.value) == variableAssignments.end()) {
							variableAssignments[conditionsIter->subject.value] = std::to_string(currentEntity->getHandle());
						}
						else {
							if (variableAssignments[conditionsIter->subject.value] != std::to_string(currentEntity->getHandle())) {
								pass = false;
								break;
							}
						}

					}
					else {
						pass = false;
						break;
					}
				}
			}			
		}

		if (pass) {
			if (result.hasValue("$a")) {
				boost::property_tree::ptree child1;
				child1.put("", currentEntity->getHandle());
				result.getElement("$a").push_back(std::make_pair("", child1));
			}
			else {
				boost::property_tree::ptree children;
				boost::property_tree::ptree child1;
				child1.put("", currentEntity->getHandle());
				children.push_back(std::make_pair("", child1));
				result.root().add_child("$a", children);
			}
		}
	}


}
