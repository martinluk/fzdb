#ifndef FUZZY_VARIABLESET
#define FUZZY_VARIABLESET

#include <vector>
#include <set>
#include <string>
#include <map>
#include <typeinfo>
#include <stdexcept>
#include <iterator>
#include <algorithm>

#include "model/types/Base.h"

using VariableType = model::types::Base::Subtype;

class VariableSetValue {
private:
	std::shared_ptr<model::types::Base> _ptr;
	unsigned int _propertyId;
	unsigned long long _entityId;

public:

	VariableSetValue(std::shared_ptr<model::types::Base> ptr, unsigned int propertyId, unsigned long long entityId) :
		_ptr(ptr), _propertyId(propertyId), _entityId(entityId) {}

	VariableSetValue() :
		_ptr(), _propertyId(0), _entityId(0) {}

	//VariableSetValue& operator=(const VariableSetValue& val) const {
	//	return VariableSetValue(val.dataPointer(), val.property(), val.entity());
	//}

	std::shared_ptr<model::types::Base> dataPointer() const { return _ptr; }
	unsigned int property() const { return _propertyId; }
	unsigned long long entity() const { return _entityId; }

};

class VariableSet {

public:

	VariableSet(const std::set<std::string> &variableNames = std::set<std::string>()) {
		_size = variableNames.size();
		_variablesUsed = std::vector<bool>(_size);
		unsigned char count = 0;
		for (auto variableName : variableNames) {
			_variablesUsed[count] = false;
			_metaData[variableName] = std::pair<VariableType, unsigned char>(VariableType::TypeUndefined, count++);			
		}
		
	}
	
	unsigned int add(const std::string&& var, VariableSetValue&& value, const VariableType&& type) {
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
			std::vector<VariableSetValue> newRow(_size);

			//replace without copying
			newRow.erase(newRow.begin() + _metaData[var].second);
			newRow.emplace(newRow.begin() + _metaData[var].second, value);

			_variablesUsed[_metaData[var].second] = true;
			_values.push_back(newRow);
			return _values.size() - 1;
		}
	}

	void add(const std::string&& var, VariableSetValue&& value, const VariableType&& type, unsigned int row) {

		if (row >= _values.size()) {
			throw new std::runtime_error("Attempting to add to a non-existent row");
		}

		if (_metaData.find(var) == _metaData.cend()) {
			throw new std::runtime_error("Unexpected variable");
		}
		else {
			unsigned char offset = _metaData[var].second;
			_variablesUsed[offset] = true;
			//_values[row][_metaData[var].second] = value;
			_values[row].erase(_values[row].begin() + offset);
			_values[row].emplace(_values[row].begin() + offset, value);
		}
	}

	std::vector<unsigned int> find(const std::string varName, const std::string value) {
		auto col = _metaData[varName].second;
		std::vector<unsigned int> output;
		for (unsigned int i = 0; i < _values.size(); i++) {
			if (_values[i][col].dataPointer()->Equals(value)) output.push_back(i);
		}
		return output;
	}

	std::vector<std::vector<VariableSetValue>>* getData() {
		return &_values;
	}

	std::vector<VariableSetValue> getData(const std::string varName) {
		auto col = _metaData[varName].second;
		std::vector<VariableSetValue> output;
		std::transform(_values.begin(), _values.end(), std::inserter(output, output.begin()), [&](std::vector<VariableSetValue> row) {
			return row[col];
		});
		return output;
	}

	std::map<std::string, std::pair<VariableType, unsigned char>> getMetaData() {
		return _metaData;
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
	std::vector<std::vector<VariableSetValue>> _values;
	std::vector<bool> _variablesUsed;
	unsigned int _size;
};
#endif // !FUZZY_VARIABLESET
