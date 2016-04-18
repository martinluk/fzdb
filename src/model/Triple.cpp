#include "./triple.h"

using namespace model;

std::vector<std::string> model::Triple::variables()
{
    std::vector<std::string> output;

    if (subject.type == Subject::Type::VARIABLE) output.push_back(subject.value);
    if (predicate.type == Predicate::Type::VARIABLE) output.push_back(predicate.value);
    if (object.type == Object::Type::VARIABLE) output.push_back(object.value);

    return output;
}
