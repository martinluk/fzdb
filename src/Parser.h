#ifndef FUZZY_PARSER
#define FUZZY_PARSER

#include <iterator>
#include <vector>
#include <string>
#include <map>

#include "model/Triple.h"

enum class ParsedTokenType {
	VARIABLE,
	STRING,
	INT,
	NOTIMPLEMENTED,
	PROPERTY,
	ENTITYREF,
	
	//keywords
	KEYWORD_SELECT,
	KEYWORD_WHERE,
	KEYWORD_INSERT,
	KEYWORD_DELETE,

	//structural
	OPEN_CURLBRACE,
	CLOSE_CURLBRACE,
	SPLITTER1,
	SPLITTER2
};

struct TokenInfo {
public:
	ParsedTokenType type;
	unsigned int lineNumber;
	unsigned int charPosition;

	TokenInfo(ParsedTokenType ptt, unsigned int lineNo, unsigned int charPos) :
		type(ptt), lineNumber(lineNo), charPosition(charPos) { }
};

//TODO: this whole file could do with tidying up

using StringMap = std::map<std::string, std::string>;
using TokenItem = std::pair<TokenInfo, std::string>;
using TokenList = std::vector<TokenItem>;
using TokenIterator = TokenList::iterator;

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
private:
	static TokenItem identifyToken(std::string str, unsigned int line, unsigned int chr);
public:
	static TokenList Tokenize(std::string str);
	static std::vector<model::Triple> ParseTriples(TokenIterator&& iter, TokenIterator end);
	static TriplesBlock ParseInsert(TokenIterator&& iter, TokenIterator end);
	static StringMap ParseSources(TokenIterator&& iter, TokenIterator end);
	static Query ParseAll(TokenList tokens);
};

#endif