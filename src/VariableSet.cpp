#include "./VariableSet.h"

#include <vector>
#include <set>
#include <string>
#include <map>
#include <typeinfo>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include "./types/Base.h"
#include "./types/ValueRef.h"

using VariableType = model::types::SubType;

VariableSet::VariableSet(const std::set<std::string> &variableNames) {
    _size = variableNames.size();
    _nextMetaRef = 0;
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

unsigned int VariableSet::add(const std::vector<VariableSetValue>&& row) {
	if (row.size() != _size) throw std::runtime_error("Attempted to insert a row of the wrong size");
	_values.push_back(row);
	return _values.size() - 1;
}

unsigned int VariableSet::add(const unsigned int var, VariableSetValue&& value, const VariableType&& type, const std::string&& metaVar) {
	std::vector<VariableSetValue> newRow(_size);
	_values.push_back(newRow);
	add(std::move(var), std::move(value), std::move(type), std::move(metaVar), _values.size() - 1);
	return _values.size() - 1;
}

void VariableSet::add(const unsigned int var, VariableSetValue&& value, const VariableType&& type, const std::string&& metaVar, unsigned int row) {

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

		if (metaVar != "") {
			unsigned int metaRef = getMetaRef();
			auto valueRef = std::make_shared<model::types::ValueRef>(value.entity(), value.property(), value.dataPointer()->OrderingId());
			auto vsv2 = VariableSetValue(valueRef, value.property(), value.entity());

			value.metaRef(metaRef);
			vsv2.metaRef(metaRef);

			add(indexOf(metaVar), std::move(vsv2), model::types::SubType::ValueReference, "");
		}

        //_values[row][_metaData[var].second] = value;
        _values[row].erase(_values[row].begin() + var);
        _values[row].emplace(_values[row].begin() + var, value);
    }
}

std::vector<unsigned int> VariableSet::find(const unsigned int varId, const std::string value) {

    std::vector<unsigned int> output;
    for (unsigned int i = 0; i < _values.size(); i++) {
        if (_values[i][varId].dataPointer()->Equals(value)) output.push_back(i);
    }
    return output;
}

std::vector<std::vector<VariableSetValue>>* VariableSet::getData() {
    return &_values;
}

std::vector<VariableSetValue> VariableSet::getData(const unsigned int varId) {

    std::vector<VariableSetValue> output;
    std::transform(_values.begin(), _values.end(), std::inserter(output, output.begin()), [&](std::vector<VariableSetValue> row) {
        return row[varId];
    });
    return output;
}
//
//std::map<std::string, std::pair<VariableType, unsigned char>> VariableSet::getMetaData() {
//    return _metaData;
//}

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
    for(int i = 0; i < _values.size(); i++) {
        for (int j = 0; j < _values[i].size(); j++) {
            if (_values[i][j].metaRef() == metaRef) {
                _values[i][j] = VariableSetValue();
            }
        }
    }
}

void VariableSet::addToMetaRefRow(unsigned int metaRef, unsigned char position, const VariableSetValue&& val) {
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
    _values.erase(std::remove_if(_values.begin(), _values.end(), [](std::vector<VariableSetValue> row) {
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

std::vector<std::vector<VariableSetValue>> VariableSet::extractRowsWith(const unsigned int variable, const std::string value) const {
	std::vector<std::vector<VariableSetValue>> results;
	for (auto row : _values) {
		if (!row[variable].empty() && row[variable].dataPointer()->Equals(value)) {
			results.emplace_back(row);
		}
	}
	return results;
}

std::vector<std::vector<VariableSetValue>> VariableSet::extractRowsWith(const unsigned int variable) const {
	std::vector<std::vector<VariableSetValue>> results;
	for (auto row : _values) {
		if (!row[variable].empty()) {
			results.emplace_back(row);
		}
	}
	return results;
}

void VariableSet::removeRowsWith(const unsigned int variable) {
	_values.erase(std::remove_if(_values.begin(), _values.end(), [&, this, variable](std::vector<VariableSetValue> row) {
		return !row[variable].empty();
	}), _values.end());
}
