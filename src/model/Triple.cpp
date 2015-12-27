#include "./Triple.h"

using namespace model;

Triple::Triple(std::string sub, std::string pred, std::string obj) {

	if (sub[0] == '$') {
		subject = Subject(TripleComponentType::Variable, sub);
	}
	else {
		subject = Subject(TripleComponentType::Entity, sub);
	}

	if (pred[0] == '$') {
		predicate = Predicate(TripleComponentType::Variable, pred);
	}
	else {
		predicate = Predicate(TripleComponentType::Property, pred);
	}
  
	if (obj[0] == '$') {
		object = Object(TripleComponentType::Variable, obj);
	}
	else {
		if (obj[0] == '<') {
			object = Object(TripleComponentType::Entity, obj);
		}
		else {
			object = Object(TripleComponentType::Value, obj);
		}
	}   
}

std::vector<std::string> model::Triple::variables()
{
	std::vector<std::string> output;

	if (subject.type == TripleComponentType::Variable) output.push_back(subject.value);
	if (predicate.type == TripleComponentType::Variable) output.push_back(predicate.value);
	if (object.type == TripleComponentType::Variable) output.push_back(object.value);

	return output;
}
