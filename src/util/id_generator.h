#ifndef FUZZY_UTIL_ID_GENERATOR
#define FUZZY_UTIL_ID_GENERATOR

#include <deque>

class StringMapSerialiser;

// Class used for generating IDs.
// If an ID is released, call addDeleted() to let the generator know.
// Released IDs are kept in a stack and re-allocated.
class IdGenerator {
  friend class StringMapSerialiser;
 private:
  unsigned int _count;
  std::deque<unsigned int> _pool;

 public:

  IdGenerator(unsigned int startCount) : _count(startCount) {}
  IdGenerator() : _count(1) {}

  void addDeleted(unsigned int deletedId) {
    _pool.push_front(deletedId);
  }

  unsigned int getId() {

    if(_pool.size() > 0) {
      unsigned int id = _pool.at(0);
      _pool.pop_front();
      return id;
    }

    return _count++;
  }

  void clear() {
    _count = 1;
    _pool.clear();
  }

  bool operator ==(const IdGenerator &b) const {
    if (_count != b._count) return false;
    if (_pool.size() != b._pool.size()) return false;
    //TODO: item by item check
    return true;
  }
};

#endif