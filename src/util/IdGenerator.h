
#include <stack>

class IdGenerator {
private:
  unsigned int _count;
  std::stack<unsigned int> _pool;

public:

  IdGenerator(unsigned int startCount) : _count(startCount) {}
  IdGenerator() : _count(0) {}

  void addDeleted(unsigned int deletedId) {
    _pool.push(deletedId);
  }

  unsigned int getId() {

    if(_pool.size() > 0) {
      unsigned int id = _pool.top();
      _pool.pop();
      return id;
    }

    return _count++;
  }
};