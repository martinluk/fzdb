#include "./Triple.h"

using namespace model;

Triple::Triple(std::string sub, std::string pred, std::string obj) {
  subject = sub;
  predicate = pred;
  object = obj;
}