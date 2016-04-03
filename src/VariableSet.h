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
#include <boost/bimap.hpp>

#include "./types/Base.h"

using VariableType = model::types::SubType;

// Tim? Describe what this does, it looks like magic. (Or sufficiently advanced technology)

// VariableSet is essentially a table of values
// Each column represents a variable in the query
// Each row represents a 'result'
// It is intended to be the data-interchange between the scan functions and so has been developed
// in a rather hacky fashion and really needs tidying up a bit

// VariableSetValue is a simple wrapper around the values stored in the VariableSet
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

    unsigned int metaRef() const {
        return _metaRef;
    }

    bool empty() const {
        return _ptr ==  nullptr;
    }
};

class VariableSet {

public:

    VariableSet(const std::set<std::string> &variableNames = std::set<std::string>());

	void extend(std::string variableName);

	unsigned int add(const std::vector<VariableSetValue>&& row);
    
    unsigned int add(const unsigned int var, VariableSetValue&& value, const VariableType&& type, const std::string&& metaVar);

    void add(const unsigned int var, VariableSetValue&& value, const VariableType&& type, const std::string&& metaVar, unsigned int row);

    std::vector<unsigned int> find(const unsigned int varId, const std::string value);

    std::vector<std::vector<VariableSetValue>>* getData();

    std::vector<VariableSetValue> getData(const unsigned int varId);

    //std::map<std::string, std::pair<VariableType, unsigned char>> getMetaData();

    const bool contains(const std::string name) const;
	const bool contains(const unsigned int id) const;

    const bool used(const std::string name) const;
	const bool used(unsigned int id) const;

    const VariableType typeOf(const std::string name) const;
	const VariableType typeOf(const unsigned char id) const;

    const unsigned char indexOf(const std::string name) const;

    const unsigned int getMetaRef();

    void removeMetaRefs(unsigned int metaRef);

    void addToMetaRefRow(unsigned int metaRef, unsigned char position, const VariableSetValue&& val);

    //this doesn't seem to work
    void trimEmptyRows();

	std::vector<std::vector<VariableSetValue>> extractRowsWith(const unsigned int variable, const std::string value) const;

	std::vector<std::vector<VariableSetValue>> extractRowsWith(const unsigned int variable) const;

	void removeRowsWith(const unsigned int variable);

private:
	boost::bimap<std::string, unsigned int> _nameMap;
	std::vector<model::types::SubType> _typeMap;
    std::vector<std::vector<VariableSetValue>> _values;
    std::vector<bool> _variablesUsed;
    unsigned int _size;
    unsigned int _nextMetaRef;
};
#endif // !FUZZY_VARIABLESET
