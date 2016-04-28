#ifndef FUZZY_UTIL_NAME_MANAGER
#define FUZZY_UTIL_NAME_MANAGER

#include "./id_generator.h"
#include <string>
#include <map>
#include <vector>
#include <set>

class StringMapSerialiser;

// Class used for generating IDs.
// If an ID is released, call addDeleted() to let the generator know.
// Released IDs are kept in a stack and re-allocated.
class NameManager {
  friend class StringMapSerialiser;
 private:
  IdGenerator _idGen;

  std::map<unsigned int, std::string> _idToStringMap;
  std::map<std::string, unsigned int> _stringToIdMap;
 public:

  NameManager() {}

  NameManager(unsigned int start) : _idGen(start) {}

  unsigned int add(const std::string name) {
    unsigned int newId = _idGen.getId();
    _idToStringMap.insert(std::make_pair(newId, name));
    _stringToIdMap.insert(std::make_pair(name, newId));
    return newId;
  }

  std::string get(const unsigned int id) const {
    return _idToStringMap.at(id);
  }

  unsigned int get(const std::string name) const {
    return _stringToIdMap.at(name);
  }

  bool has(const unsigned int id) const {
    return _idToStringMap.find(id) != _idToStringMap.end();
  }

  bool has(const std::string name) const {
    return _stringToIdMap.find(name) != _stringToIdMap.end();
  }

  void remove(const unsigned int id) {
    std::string name = _idToStringMap.at(id);
    _idToStringMap.erase(id);
    _stringToIdMap.erase(name);
    _idGen.addDeleted(id);
  }

  void remove(const std::string name) {
    unsigned int id = _stringToIdMap.at(name);
    _idToStringMap.erase(id);
    _stringToIdMap.erase(name);
    _idGen.addDeleted(id);
  }

  void clear() {
    _idToStringMap.clear();
    _stringToIdMap.clear();
    _idGen.clear();
  }

  bool operator ==(const NameManager &b) const {
    return std::equal(_idToStringMap.begin(), _idToStringMap.end(), b._idToStringMap.begin()) && _idGen == b._idGen;
  }

  size_t size() const {
    return _idToStringMap.size();
  }

  std::set<std::string> names() const {
    std::set<std::string> v;
    for (auto it = _stringToIdMap.begin(); it != _stringToIdMap.end(); ++it) {
      v.insert(it->first);
    }
    return v;
  }
};

#endif // !FUZZY_UTIL_NAME_MANAGER
