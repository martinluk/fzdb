#ifndef FUZZY_VARIABLESET
#define FUZZY_VARIABLESET

#include <vector>
#include <string>
#include <map>
#include <typeinfo>

#include "model/types/Base.h"

using VariableType = model::types::Base::Subtype;

class VariableSet {

public:

	VariableSet() { }
	
	void add(const std::string&& var, std::string&& value, VariableType&& type) {
		if (_data.find(var) == _data.cend()) {
			_data[var] = std::pair<std::vector<std::string>, VariableType>(std::vector<std::string>{ value }, type);
		}
		else {
			if (type != _data[var].second) {
				throw new std::runtime_error("Attempted to mix variable types!");
			}
			_data[var].first.push_back(value);
		}
	}

	const std::map<std::string, std::pair<std::vector<std::string>, VariableType>> getData() {
		return _data;
	}

	const bool contains(std::string name) {
		return _data.find(name) != _data.cend();
	}

	const VariableType typeOf(std::string name) {
		return _data[name].second;
	}

	const std::vector<std::string> getValuesFor(std::string name) {
		return _data[name].first;
	}

	void replaceValuesFor(std::string name, std::vector<std::string> data) {
		_data[name].first = data;
	}

private:
	std::map<std::string, std::pair<std::vector<std::string>, VariableType>> _data;
};
#endif // !FUZZY_VARIABLESET
