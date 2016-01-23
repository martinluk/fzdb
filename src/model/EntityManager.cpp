#include "./EntityManager.h"
#include <cassert>
#include <set>
#include <sstream>

EntityManager::EntityManager()
{
	_lastHandle = Entity::INVALID_EHANDLE;
	_lastProperty = 0;
}

EntityManager::~EntityManager()
{
	for (EntityMap::iterator it = _entities.begin(); it != _entities.end(); ++it)
	{
		delete it->second;
	}
}

Entity* EntityManager::createEntity()
{
	_lastHandle++;
	assert(_lastHandle != Entity::INVALID_EHANDLE);

	Entity* e = new Entity(0, _lastHandle);
	_entities.insert(std::pair<Entity::EHandle_t, Entity*>(_lastHandle, e));

	return e;
}

QueryResult EntityManager::SeparateTriples(std::vector<model::Triple> conditions)
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
VariableSet EntityManager::BGP(std::vector<model::Triple> conditions)
{
	std::vector<Entity::EHandle_t> passed;
	VariableSet result;

	//sort by entropy
	std::sort(conditions.begin(), conditions.end(), [](model::Triple t1, model::Triple t2) { return t1.Entropy() < t2.Entropy(); });

	//iterate over conditions
	for (auto conditionsIter = conditions.cbegin(); conditionsIter != conditions.end(); conditionsIter++) {

		//There are 7 valid combinations

		if (conditionsIter->subject.type == model::Subject::Type::VARIABLE) {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 1 - $a <prop> value
					this->Scan1(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), std::move(conditionsIter->object));
				}
				else {
					//option 2 - $a <prop> $b
					this->Scan2(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), conditionsIter->object.value);
				}
			}
			else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option3 - $a $b value
					//this->Scan1(std::move(result), conditionsIter->subject.value, std::move(conditionsIter->predicate), std::move(conditionsIter->object));
					throw NotImplementedException("Queries of the form $a $b value are not yet implemented");
				}
				else {
					//option 4 - $a $b $c
					this->Scan4(std::move(result), conditionsIter->subject.value, conditionsIter->predicate.value, conditionsIter->object.value);
				}
			}
		}
		else {
			if (conditionsIter->predicate.type == model::Predicate::Type::PROPERTY) {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//doesn't contain any variables.. is meaningless
				}
				else {
					//option 5 - entity <prop> $c
					this->Scan5(std::move(result), std::move(conditionsIter->subject), std::move(conditionsIter->predicate), conditionsIter->object.value);
				}
			}
	 		else {
				if (model::Object::IsValue(conditionsIter->object.type)) {
					//option 7 - entity $b value
					throw NotImplementedException("Queries of the form entity $b value are not yet implemented");
				}
				else {
					//option 8 - entity $b $c
					throw NotImplementedException("Queries of the form entity $b $c are not yet implemented");
				}
			}
		}
	}

	return result;
}

//Inserts new data into the data store
void EntityManager::Insert(std::vector<model::Triple> triples) {
	auto iter = triples.cbegin();
	auto end = triples.cend();
	for (; iter != end; iter++) {
		auto triple = *iter;

		auto entity_id = std::stoll(triple.subject.value);

		//create the entity if it doesn't exist
		if (_entities.find(entity_id) == _entities.end()) {
			_entities[entity_id] = new Entity(1, ++_lastHandle);
		}

		Entity* currentEntity = _entities[entity_id];		
		unsigned int propertyId;

		switch (triple.object.type) {
		case model::Object::Type::STRING:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeString, true);
			this->addToEntity<model::types::String>(currentEntity, propertyId, std::move(triple.object));
			break;
		case model::Object::Type::ENTITYREF:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeEntityRef, true);
			this->addToEntity<model::types::EntityRef>(currentEntity, propertyId, std::move(triple.object));
			break;
		case model::Object::Type::INT:
			propertyId = this->getPropertyName(triple.predicate.value, model::types::Base::Subtype::TypeInt32, true);
			this->addToEntity<model::types::Int>(currentEntity, propertyId, std::move(triple.object));
			break;
		}
	}
}

std::vector<Entity*> EntityManager::entityList() const
{
    std::vector<Entity*> list;
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        list.push_back(it->second);
    }
    
    return list;
}

// TODO: Do we need some sort of handle renumber function too?
void EntityManager::insertEntity(Entity *ent)
{
    assert(ent->getHandle() != Entity::INVALID_EHANDLE);
    assert(_entities.find(ent->getHandle()) == _entities.end());
    
    if ( _lastHandle < ent->getHandle() )
        _lastHandle = ent->getHandle();
    
    _entities.insert(std::pair<Entity::EHandle_t, Entity*>(ent->getHandle(), ent));
}

void EntityManager::clearAll()
{
    _entities.clear();
    _lastHandle = Entity::INVALID_EHANDLE;
    _lastProperty = 0;
    _entityTypeNames.clear();
    _propertyNames.clear();
    _propertyTypes.clear();
}

std::size_t EntityManager::entityCount() const
{
    return _entities.size();
}

std::string EntityManager::dumpContents() const
{
    std::stringstream str;
    str << "Number of entities: " << _entities.size() << "\n";
    
    for ( auto it = _entities.cbegin(); it != _entities.cend(); ++it )
    {
        const Entity* ent = it->second;
        str << ent->logString() << "\n";
        if ( ent->propertyCount() > 0 )
        {
            str << "{\n";
            
            const std::map<unsigned int, IEntityProperty*> &propMap = ent->properties();
            for ( auto it2 = propMap.cbegin(); it2 != propMap.cend(); ++it2 )
            {
                IEntityProperty* prop = it2->second;
                str << "  " << prop->logString() << "\n";
                
                if ( prop->count() > 0 )
                {
                    str << "  {\n";
                    
                    for ( int i = 0; i < prop->count(); i++ )
                    {
                        model::types::Base* val = prop->baseValue(i);
                        str << "    " << val->logString() << "\n";
                    }
                    
                    str << "  }\n";
                }
            }
            
            str << "}\n";
        }
    }
    
    return str.str();
}
