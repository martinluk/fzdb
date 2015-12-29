#ifndef FUZZY_PARSER
#define FUZZY_PARSER

#include <iterator>
#include <vector>
#include <string>
#include <map>

#include "model/Triple.h"

enum class Tokens {
	SELECT
};

//TODO: this whole file could do with tidying up

using StringIterator = std::vector<std::string>::iterator;
using StringMap = std::map<std::string, std::string>;

//General exception class for parsing
//TODO: extend to show line and column
class ParseException : public std::exception {

private:

	std::string _message;

public:
	
	ParseException(std::string msg) {
		_message = msg;
	}

	virtual const char* what() const throw()
	{
		return _message.c_str();
	}
};

//Represents a block of triples
struct TriplesBlock {
public:
	std::vector<model::Triple> triples;
	std::string name;
	std::vector<std::string> filters;

	TriplesBlock(std::vector<model::Triple> trip, std::string n) {
		triples = trip;
		name = n;
	}

	TriplesBlock(std::vector<model::Triple> trip) {
		triples = trip;
	}

	TriplesBlock() {}
};

//Types of query
//ECHO is called DEBUGECHO to avoid a namespace collision on linux
enum class QueryType {
	SELECT,
	INSERT,
	DEL,
	PING,
	DEBUGECHO,
	USER
};

//Aggregate query object - this should contain all info required to do a query
struct Query {
public:

	QueryType type;
	StringMap sources;
	TriplesBlock conditions;
	TriplesBlock whereClause;
	std::string data0;

	Query(QueryType t, StringMap s, TriplesBlock cond, TriplesBlock wh, std::string dat0) {
		type = t;
		sources = s;
		conditions = cond;
		whereClause = wh;
		data0 = dat0;
	}
};

class FSparqlParser {
public:
	static std::vector<std::string> Tokenize(std::string str);
	static std::vector<model::Triple> ParseTriples(StringIterator&& iter, StringIterator end);
	static TriplesBlock ParseInsert(StringIterator&& iter, StringIterator end);
	static StringMap ParseSources(StringIterator&& iter, StringIterator end);
	static Query ParseAll(std::vector<std::string> tokens);
};

#endif