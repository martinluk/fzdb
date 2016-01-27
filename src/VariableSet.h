#ifndef FUZZY_VARIABLESET
#define FUZZY_VARIABLESET

#include <vector>
#include <set>
#include <string>
#include <map>
#include <typeinfo>
#include <stdexcept>

#include "model/types/Base.h"

using VariableType = model::types::Base::Subtype;


class VariableSet {

public:

	VariableSet(std::set<std::string> variableNames) {
		_size = variableNames.size();
		_variablesUsed = std::vector<bool>(_size);
		unsigned char count = 0;
		for (auto variableName : variableNames) {
			_variablesUsed[count] = false;
			_metaData[variableName] = std::pair<VariableType, unsigned char>(VariableType::TypeUndefined, count++);			
		}
		
	}
	
	void add(const std::string&& var, std::string&& value, VariableType&& type) {
		if (_metaData.find(var) == _metaData.cend()) {
			throw new std::runtime_error("Unexpected variable");
		}
		else {
			if (type != _metaData[var].first) {
				if (_metaData[var].first == VariableType::TypeUndefined) {
					_metaData[var].first = type;
				}
				else {
					throw new std::runtime_error("Attempted to mix variable types!");
				}				
			}
			//_data[var].first.push_back(value);
			std::vector<std::string> newRow(_size);
			newRow[_metaData[var].second] = value;
			_variablesUsed[_metaData[var].second] = true;
			_values.push_back(newRow);
		}
	}

	std::vector<std::vector<std::string>>* getData() {
		return &_values;
	}

	const bool contains(std::string name) {
		return _metaData.find(name) != _metaData.cend();
	}

	const bool used(std::string name) {
		return _variablesUsed[_metaData[name].second];
	}

	const VariableType typeOf(std::string name) {
		return _metaData[name].first;
	}

	const unsigned char indexOf(std::string name) {
		return _metaData[name].second;
	}

private:
	std::map<std::string, std::pair<VariableType, unsigned char>> _metaData;
	std::vector<std::vector<std::string>> _values;
	std::vector<bool> _variablesUsed;
	unsigned int _size;
};
#endif // !FUZZY_VARIABLESET
