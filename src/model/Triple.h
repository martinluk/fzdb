#ifndef FUZZY_MODEL_TRIPLE
#define FUZZY_MODEL_TRIPLE

#include <string>
#include <vector>

namespace model {

	enum class TripleComponentType {
		EntityRef,
		String,
		Integer,
		Property,
		Variable
	};

	struct Subject {
	public:

		enum class Type {
			ENTITYREF,
			VARIABLE
		};

		Type type;
		std::string value;

		Subject() { }

		Subject(Type t, std::string val) : value(val) {
			type = t;
			if (type != Type::ENTITYREF && type != Type::VARIABLE) {
				//is bad, but be one of those
			}
		}
	};
	
	struct Predicate {
	public:

		enum class Type {
			PROPERTY,
			VARIABLE
		};

		Type type;
		std::string value;

		Predicate() { }

		Predicate(Type t, std::string val) : value(val) {
			type = t;
			if (type != Type::PROPERTY && type != Type::VARIABLE) {
				//is bad, but be one of those
			}
		}
	};

	struct Object {
	public:

		enum class Type {
			ENTITYREF,
			STRING,
			VARIABLE,
			INT
		};

		Type type;
		std::string value;

		Object() { }

		Object(Type t, std::string val) : value(val) {
			type = t;
		}
	};



  struct Triple {
  public: 
    Subject subject;
    Predicate predicate;
    Object object;

	Triple(Subject sub, Predicate pred, Object obj) : subject(sub), predicate(pred), object(obj) {}
	std::vector<std::string> variables();
  };

}

#endif