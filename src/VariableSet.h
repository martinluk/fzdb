#ifndef FUZZY_VARIABLESET
#define FUZZY_VARIABLESET

#include <vector>
#include <string>
#include <map>
#include <typeinfo>

#include "model/types/Base.h"

class VariableSet {

public:

	VariableSet() { }
	
	void add(const std::string&& var, std::string&& value, model::types::Base::Subtype&& type) {
		if (_data.find(var) == _data.cend()) {
			_data[var] = std::pair<std::vector<std::string>, model::types::Base::Subtype>(std::vector<std::string>{ value }, type);
		}
		else {
			if (type != _data[var].second) {
				throw new std::runtime_error("Attempted to mix variable types!");
			}
			_data[var].first.push_back(value);
		}
	}

	const std::map<std::string, std::pair<std::vector<std::string>, model::types::Base::Subtype>> getData() {
		return _data;
	}

	const bool contains(std::string name) {
		return _data.find(name) != _data.cend();
	}

	const model::types::Base::Subtype typeOf(std::string name) {
		return _data[name].second;
	}

	const std::vector<std::string> getValuesFor(std::string name) {
		return _data[name].first;
	}

private:
	std::map<std::string, std::pair<std::vector<std::string>, model::types::Base::Subtype>> _data;
};
#endif // !FUZZY_VARIABLESET
