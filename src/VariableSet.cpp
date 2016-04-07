#include "./VariableSet.h"

#include <vector>
#include <set>
#include <string>
#include <map>
#include <typeinfo>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include "./model/Entity.h"
#include "./types/Base.h"
#include "./types/ValueRef.h"

using VariableType = model::types::SubType;

VariableSet::VariableSet(const std::set<std::string> &variableNames) {
    _size = variableNames.size();
    _nextMetaRef = 1;
    _variablesUsed = std::vector<bool>(_size);
    unsigned char count = 0;
    for (auto variableName : variableNames) {
        _variablesUsed[count] = false;
		_nameMap.insert(boost::bimap<std::string, unsigned int>::value_type(variableName, count++));
		_typeMap.push_back(VariableType::TypeUndefined);       
    }
    
}

void VariableSet::extend(std::string variableName) {
	
	_variablesUsed.push_back(false);
	_typeMap.push_back(VariableType::TypeUndefined);
	_nameMap.insert(boost::bimap<std::string, unsigned int>::value_type(variableName, _size++));
}

unsigned int VariableSet::add(const VariableSetRow&& row) {
	if (row.size() != _size) throw std::runtime_error("Attempted to insert a row of the wrong size");
	_values.push_back(row);
	return _values.size() - 1;
}

unsigned int VariableSet::add(const unsigned int var, const std::shared_ptr<model::types::Base>&& value,
	const unsigned int propertyId, const Entity::EHandle_t entityId, const VariableType&& type, const std::string&& metaVar) {

	VariableSetRow newRow(_size);
	_values.push_back(newRow);
	add(std::move(var), std::move(value), propertyId, entityId, std::move(type), std::move(metaVar), _values.size() - 1);
	return _values.size() - 1;
}

void VariableSet::add(const unsigned int var, const std::shared_ptr<model::types::Base>&& value,
	const unsigned int propertyId, const Entity::EHandle_t entityId, const VariableType&& type, const std::string&& metaVar, unsigned int row) {

    if (row >= _values.size()) {
        throw std::runtime_error("Attempting to add to a non-existent row");
    }

    if (_nameMap.right.find(var) == _nameMap.right.end()) {
        throw std::runtime_error("Unexpected variable");
    }
    else {
		
		if (type != _typeMap[var]) {
			if (_typeMap[var] == VariableType::TypeUndefined) {
				_typeMap[var] = type;
			}
			else {
				throw std::runtime_error("Attempted to mix variable types!");
			}
		}

        _variablesUsed[var] = true;

		_values[row][var].reset(value, entityId, propertyId);
		_values[row].ranking(_values[row].ranking() + value->confidence());

		if (metaVar != "") {
			unsigned int metaRef = getMetaRef();
			auto valueRef = std::make_shared<model::types::ValueRef>(entityId, propertyId, value->OrderingId());

			_values[row][var].metaRef(metaRef);

			auto metaRowId = add(indexOf(metaVar), valueRef, entityId, propertyId, model::types::SubType::ValueReference, "");
			_values[metaRowId][indexOf(metaVar)].metaRef(metaRef);
		}
    }
}

std::vector<unsigned int> VariableSet::find(const unsigned int varId, const std::string value) {

    std::vector<unsigned int> output;
    for (unsigned int i = 0; i < _values.size(); i++) {
        if (_values[i][varId].dataPointer()->Equals(value)) output.push_back(i);
    }
    return output;
}

//std::vector<VariableSetRow>* VariableSet::getData() {
//    return &_values;
//}

std::vector<VariableSetRow>::iterator VariableSet::erase(std::vector<VariableSetRow>::iterator iter) {

	if (iter != _values.end() - 1) {
		std::iter_swap(iter, _values.end() - 1);
		_values.pop_back();
		if (_values.size() == 0) return _values.end();
		return iter;
	}
	else {
		_values.pop_back();
		return _values.end();
	}
}

std::vector<VariableSetValue> VariableSet::getData(const unsigned int varId) const {

    std::vector<VariableSetValue> output;
    std::transform(_values.cbegin(), _values.cend(), std::inserter(output, output.begin()), [&](VariableSetRow row) {
        return row[varId];
    });
    return output;
}

const bool VariableSet::contains(const std::string name) const {
	return _nameMap.left.find(name) != _nameMap.left.end();
}

const bool VariableSet::contains(const unsigned int id) const {
	return _nameMap.right.find(id) != _nameMap.right.end();
}

const bool VariableSet::used(const std::string name) const {
    return _variablesUsed[_nameMap.left.at(name)];
}

const bool VariableSet::used(unsigned int id) const
{
	return _variablesUsed[id];
}

const VariableType VariableSet::typeOf(const std::string name) const {
	return _typeMap.at(_nameMap.left.at(name));
}

const VariableType VariableSet::typeOf(const unsigned char id) const {
	return _typeMap.at(id);
}

const unsigned char VariableSet::indexOf(const std::string name) const {
	return _nameMap.left.at(name);
}

const unsigned int VariableSet::getMetaRef() {
    return _nextMetaRef++;
}

void VariableSet::removeMetaRefs(unsigned int metaRef) {
	if (metaRef == 0) throw std::runtime_error("Unexpected MetaRef");
    for(int i = 0; i < _values.size(); i++) {
        for (int j = 0; j < _values[i].size(); j++) {
            if (_values[i][j].metaRef() == metaRef) {
				_values[i][j].reset();
            }
        }
    }
}

void VariableSet::addToMetaRefRow(unsigned int metaRef, unsigned char position, const VariableSetValue&& val) {
	if (metaRef == 0) throw std::runtime_error("Unexpected MetaRef");
    bool found = false;
    for (int i = 0; i < _values.size(); i++) {
        for (unsigned char j = 0; j < _values[i].size(); j++) {
            if (_values[i][j].metaRef() == metaRef && typeOf(j) != model::types::SubType::ValueReference) {
                _values[i][position] = val;
                found = true;
                break;
            }
        }
        if (found)break;
    }
}

//this doesn't seem to work
void VariableSet::trimEmptyRows() {
    _values.erase(std::remove_if(_values.begin(), _values.end(), [](VariableSetRow row) {
        bool allEmpty = true;
        for (auto val : row) {
            if (!val.empty()) {
                allEmpty = false;
                break;
            }
        }
        return allEmpty;
    }), _values.end());
}

std::vector<VariableSetRow> VariableSet::extractRowsWith(const unsigned int variable, const std::string value) const {
	std::vector<VariableSetRow> results;
	for (auto iter = _values.cbegin(); iter != _values.cend(); iter++) {
		
		if (!iter->at(variable).empty() && iter->at(variable).dataPointer()->Equals(value)) {
			results.emplace_back(*iter);
		}
	}
	return results;
}

std::vector<VariableSetRow> VariableSet::extractRowsWith(const unsigned int variable) const {
	std::vector<VariableSetRow> results;
	for (auto iter = _values.cbegin(); iter != _values.cend(); iter++) {
		if (!iter->at(variable).empty()) {
			results.emplace_back(*iter);
		}
	}
	return results;
}

void VariableSet::removeRowsWith(const unsigned int variable) {
	_values.erase(std::remove_if(_values.begin(), _values.end(), [&, this, variable](VariableSetRow row) {
		return !row[variable].empty();
	}), _values.end());
}

void VariableSet::sort()
{
	std::sort(_values.begin(), _values.end(), [](const VariableSetRow& row1, const VariableSetRow& row2)
	{
		return (row1.ranking() > row2.ranking());
	});
}
