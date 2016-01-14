#ifndef FUZZY_PARSER
#define FUZZY_PARSER

#include <iterator>
#include <vector>
#include <string>
#include <map>

#include "model/Triple.h"

#define TOKEN_SPLITTER_MASK 0b010000
#define TOKEN_KEYWORD_MASK 0b100000

// Jonathan: Formatted these a bit more nicely as they're numbers, not flags,
// and I'm adding to them :P
enum class ParsedTokenType {
        VARIABLE       = 0x0,
        STRING         = 0x1,
        INT            = 0x2,
        NOTIMPLEMENTED = 0x3,
        PROPERTY       = 0x4,
        ENTITYREF      = 0x5,

	//structural
        OPEN_CURLBRACE  = 0x6,
        CLOSE_CURLBRACE = 0x7,
	
	//keywords
        KEYWORD_SELECT = TOKEN_KEYWORD_MASK | 0x0,
        KEYWORD_WHERE  = TOKEN_KEYWORD_MASK | 0x1,
        KEYWORD_INSERT = TOKEN_KEYWORD_MASK | 0x2,
        KEYWORD_DELETE = TOKEN_KEYWORD_MASK | 0x3,
        KEYWORD_PING   = TOKEN_KEYWORD_MASK | 0x4,
        KEYWORD_ECHO   = TOKEN_KEYWORD_MASK | 0x5,
        KEYWORD_SOURCE = TOKEN_KEYWORD_MASK | 0x6,
        KEYWORD_DATA   = TOKEN_KEYWORD_MASK | 0x7,
        KEYWORD_DEBUG  = TOKEN_KEYWORD_MASK | 0x8,
	
        SPLITTER1 = TOKEN_SPLITTER_MASK | 0x0,
        SPLITTER2 = TOKEN_SPLITTER_MASK | 0x1,
        SPLITTER3 = TOKEN_SPLITTER_MASK | 0x2,
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
        DEBUGOTHER,
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