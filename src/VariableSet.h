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

#include "./types/Base.h"

using VariableType = model::types::SubType;

// Tim? Describe what this does, it looks like magic. (Or sufficiently advanced technology)
class VariableSetValue {
private:

    //these should really be const, but setting them to const causes all kinds of strange issues
    std::shared_ptr<model::types::Base> _ptr;
    unsigned int _propertyId;
    unsigned long long _entityId;

    unsigned int _metaRef;

public:

    VariableSetValue(std::shared_ptr<model::types::Base> ptr, unsigned int propertyId, unsigned long long entityId) :
        _ptr(ptr), _propertyId(propertyId), _entityId(entityId) {}

    VariableSetValue() :
        _ptr(), _propertyId(0), _entityId(0) {}

    std::shared_ptr<model::types::Base> dataPointer() const { return _ptr; }
    unsigned int property() const { return _propertyId; }
    unsigned long long entity() const { return _entityId; }

    void metaRef(unsigned int metaRef) {
        _metaRef = metaRef;
    }

    unsigned int metaRef() {
        return _metaRef;
    }

    bool empty() {
        return _ptr ==  nullptr;
    }
};

class VariableSet {

public:

    VariableSet(const std::set<std::string> &variableNames = std::set<std::string>()) {
        _size = variableNames.size();
        _nextMetaRef = 0;
        _variablesUsed = std::vector<bool>(_size);
        unsigned char count = 0;
        for (auto variableName : variableNames) {
            _variablesUsed[count] = false;
            _metaData[variableName] = std::pair<VariableType, unsigned char>(VariableType::TypeUndefined, count++);            
        }
        
    }

	void extend(std::string variableName) {
		
		_variablesUsed.push_back(false);
		_metaData[variableName] = std::pair<VariableType, unsigned char>(VariableType::TypeUndefined, _size);
		_size++;
	}
    
    unsigned int add(const std::string&& var, VariableSetValue&& value, const VariableType&& type) {
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

	const VariableType typeOf(unsigned char id) {
		for (auto md : _metaData) {
			if (md.second.second == id)return md.second.first;
		}
		return VariableType::TypeUndefined;
	}

    const unsigned char indexOf(std::string name) {
        return _metaData[name].second;
    }

    const unsigned int getMetaRef() {
        return _nextMetaRef++;
    }

    void removeMetaRefs(unsigned int metaRef) {
        for(int i = 0; i < _values.size(); i++) {
            for (int j = 0; j < _values[i].size(); j++) {
                if (_values[i][j].metaRef() == metaRef) {
                    _values[i][j] = VariableSetValue();
                }
            }
        }
    }

    void addToMetaRefRow(unsigned int metaRef, unsigned char position, const VariableSetValue&& val) {
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
    void trimEmptyRows() {
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

private:
    std::map<std::string, std::pair<VariableType, unsigned char>> _metaData;
    std::vector<std::vector<VariableSetValue>> _values;
    std::vector<bool> _variablesUsed;
    unsigned int _size;
    unsigned int _nextMetaRef;
};
#endif // !FUZZY_VARIABLESET
