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

using VariableType = model::types::SubType;

VariableSet::VariableSet(const std::set<std::string> &variableNames) {
    _size = variableNames.size();
    _nextMetaRef = 0;
    _variablesUsed = std::vector<bool>(_size);
    unsigned char count = 0;
    for (auto variableName : variableNames) {
        _variablesUsed[count] = false;
        _metaData[variableName] = std::pair<VariableType, unsigned char>(VariableType::TypeUndefined, count++);            
    }
    
}

void VariableSet::extend(std::string variableName) {
	
	_variablesUsed.push_back(false);
	_metaData[variableName] = std::pair<VariableType, unsigned char>(VariableType::TypeUndefined, _size);
	_size++;
}

unsigned int VariableSet::add(const std::vector<VariableSetValue>&& row) {
	if (row.size() != _size) throw std::runtime_error("Attempted to insert a row of the wrong size");
	_values.push_back(row);
	return _values.size() - 1;
}

unsigned int VariableSet::add(const std::string&& var, VariableSetValue&& value, const VariableType&& type) {
	std::vector<VariableSetValue> newRow(_size);
	_values.push_back(newRow);
	add(std::move(var), std::move(value), std::move(type), _values.size() - 1);
	return _values.size() - 1;
}

void VariableSet::add(const std::string&& var, VariableSetValue&& value, const VariableType&& type, unsigned int row) {

    if (row >= _values.size()) {
        throw std::runtime_error("Attempting to add to a non-existent row");
    }

    if (_metaData.find(var) == _metaData.cend()) {
        throw std::runtime_error("Unexpected variable");
    }
    else {

		if (type != _metaData[var].first) {
			if (_metaData[var].first == VariableType::TypeUndefined) {
				_metaData[var].first = type;
			}
			else {
				throw std::runtime_error("Attempted to mix variable types!");
			}
		}

        unsigned char offset = _metaData[var].second;
        _variablesUsed[offset] = true;
        //_values[row][_metaData[var].second] = value;
        _values[row].erase(_values[row].begin() + offset);
        _values[row].emplace(_values[row].begin() + offset, value);
    }
}

std::vector<unsigned int> VariableSet::find(const std::string varName, const std::string value) {
    auto col = _metaData[varName].second;
    std::vector<unsigned int> output;
    for (unsigned int i = 0; i < _values.size(); i++) {
        if (_values[i][col].dataPointer()->Equals(value)) output.push_back(i);
    }
    return output;
}

std::vector<std::vector<VariableSetValue>>* VariableSet::getData() {
    return &_values;
}

std::vector<VariableSetValue> VariableSet::getData(const std::string varName) {
    auto col = _metaData[varName].second;
    std::vector<VariableSetValue> output;
    std::transform(_values.begin(), _values.end(), std::inserter(output, output.begin()), [&](std::vector<VariableSetValue> row) {
        return row[col];
    });
    return output;
}

std::map<std::string, std::pair<VariableType, unsigned char>> VariableSet::getMetaData() {
    return _metaData;
}

const bool VariableSet::contains(std::string name) {
    return _metaData.find(name) != _metaData.cend();
}

const bool VariableSet::used(std::string name) {
    return _variablesUsed[_metaData[name].second];
}

const VariableType VariableSet::typeOf(std::string name) {
    return _metaData[name].first;
}

const VariableType VariableSet::typeOf(unsigned char id) {
	for (auto md : _metaData) {
		if (md.second.second == id)return md.second.first;
	}
	return VariableType::TypeUndefined;
}

const unsigned char VariableSet::indexOf(std::string name) {
    return _metaData[name].second;
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

std::vector<std::vector<VariableSetValue>> VariableSet::extractRowsWith(std::string variable, std::string value) {
	std::vector<std::vector<VariableSetValue>> results;
	for (auto row : _values) {
		if (!row[_metaData[variable].second].empty() && row[_metaData[value].second].dataPointer()->Equals(value)) {
			results.emplace_back(row);
		}
	}
	return results;
}

std::vector<std::vector<VariableSetValue>> VariableSet::extractRowsWith(std::string variable) {
	std::vector<std::vector<VariableSetValue>> results;
	for (auto row : _values) {
		if (!row[_metaData[variable].second].empty()) {
			results.emplace_back(row);
		}
	}
	return results;
}

void VariableSet::removeRowsWith(std::string variable) {
	_values.erase(std::remove_if(_values.begin(), _values.end(), [&, this, variable](std::vector<VariableSetValue> row) {
		return !row[_metaData[variable].second].empty();
	}), _values.end());
}
