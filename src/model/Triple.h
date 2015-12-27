#ifndef FUZZY_MODEL_TRIPLE
#define FUZZY_MODEL_TRIPLE

#include <string>
#include <vector>

namespace model {

	enum class TripleComponentType {
		Entity,
		Variable,
		Property,
		Value
	};

	struct Subject {
	public:
		TripleComponentType type;
		std::string value;

		Subject() { }

		Subject(TripleComponentType t, std::string val) : value(val) {
			type = t;
			if (type != TripleComponentType::Entity && type != TripleComponentType::Variable) {
				//is bad, but be one of those
			}
		}
	};
	
	struct Predicate {
	public:
		TripleComponentType type;
		std::string value;

		Predicate() { }

		Predicate(TripleComponentType t, std::string val) : value(val) {
			type = t;
			if (type != TripleComponentType::Property && type != TripleComponentType::Variable) {
				//is bad, but be one of those
			}
		}
	};

	struct Object {
	public:
		TripleComponentType type;
		std::string value;

		Object() { }

		Object(TripleComponentType t, std::string val) : value(val) {
			type = t;
			if (type != TripleComponentType::Value && type != TripleComponentType::Variable && type != TripleComponentType::Entity) {
				//is bad, but be one of those
			}
		}
	};



  struct Triple {
  public: 
    Subject subject;
    Predicate predicate;
    Object object;

    Triple(std::string sub, std::string pred, std::string obj);

	std::vector<std::string> variables();
  };

}

#endif