#ifndef FUZZY_MODEL_TRIPLE
#define FUZZY_MODEL_TRIPLE

#include <string>

namespace model {

  struct Triple {
  public: 
    std::string subject;
    std::string predicate;
    std::string object;

    Triple(std::string sub, std::string pred, std::string obj);
  };

}

#endif