#ifndef FUZZY_PARSER
#define FUZZY_PARSER

#include <iterator>
#include <vector>
#include <string>
#include <map>

#include "model/Triple.h"

#define TOKEN_SPLITTER_MASK 0b010000
#define TOKEN_KEYWORD_MASK 0b100000

enum class ParsedTokenType {
	VARIABLE = 0b000000,
	STRING = 0b000001,
	INT = 0b000010,
	NOTIMPLEMENTED = 0b000011,
	PROPERTY = 0b000100,
	ENTITYREF = 0b000101,

	//structural
	OPEN_CURLBRACE = 0b000110,
	CLOSE_CURLBRACE = 0b000111,
	
	//keywords
	KEYWORD_SELECT = 0b100000,
	KEYWORD_WHERE = 0b100001,
	KEYWORD_INSERT = 0b100010,
	KEYWORD_DELETE = 0b100011,
	KEYWORD_PING = 0b100100,
	KEYWORD_ECHO = 0b100101,
	KEYWORD_SOURCE = 0b100110,
	KEYWORD_DATA = 0b100111,
	
	SPLITTER1 = 0b010000,
	SPLITTER2 = 0b010001,
	SPLITTER3 = 0b010010
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