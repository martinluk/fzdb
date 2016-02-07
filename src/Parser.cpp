#include "Parser.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

#include <iostream>

#include "./filters/RegexFilter.h"
#include "./filters/OrderingFilters.h"
#include "spdlog/spdlog.h"

TokenItem FSparqlParser::identifyToken(std::string str, unsigned int line, unsigned int chr) {

	static const boost::regex variableRegex("\\$(.+)");
	static const boost::regex stringRegex("\"(.*)\"");
	static const boost::regex propertyRegex("<(.*)>");
	static const boost::regex entityRefRegex("entity:([0-9]+)");
	static const boost::regex intRegex("[0-9]+");
	static const boost::regex simpleConfidenceRatingRegex("\\[([0-9]+)\\]");
	static const boost::regex filterRegex("FILTER *([a-zA-Z]*)\\( *(.+) *\\)");

   boost::smatch matches;
   std::string data0 = "";

	ParsedTokenType tokenType = ParsedTokenType::NOTIMPLEMENTED;

	if (boost::regex_match(str, matches, variableRegex)) {
		tokenType = ParsedTokenType::VARIABLE;
		str = matches[1];
	}

	else if (boost::regex_match(str, matches, stringRegex)) {
		tokenType = ParsedTokenType::STRING;
      str = matches[1];
	}

	else if (boost::regex_match(str, matches, propertyRegex)) {
		tokenType = ParsedTokenType::PROPERTY;
      str = matches[1];
	}

	else if (boost::regex_match(str, matches, entityRefRegex)) {
		tokenType = ParsedTokenType::ENTITYREF;
      str = matches[1];
	}

	else if (boost::regex_match(str, intRegex)) {
		tokenType = ParsedTokenType::INT;
	}

   else if (boost::regex_match(str, matches, filterRegex)) {
		tokenType = ParsedTokenType::FILTER;
		data0 = matches[1];
      str = matches[2];
	}

	else if (boost::regex_match(str, matches, simpleConfidenceRatingRegex)) {
		tokenType = ParsedTokenType::CONFIDENCE_RATING;
      str = matches[1];
	}

	else if (str == "{") tokenType = ParsedTokenType::OPEN_CURLBRACE;

	else if (str == "}") tokenType = ParsedTokenType::CLOSE_CURLBRACE;

	else if (str == ";") tokenType = ParsedTokenType::SPLITTER2;

	else if (str == ",") tokenType = ParsedTokenType::SPLITTER3;

	else if (str == ".") tokenType = ParsedTokenType::SPLITTER1;

	//keywords
	else if (str == "SELECT") tokenType = ParsedTokenType::KEYWORD_SELECT;
	else if (str == "INSERT") tokenType = ParsedTokenType::KEYWORD_INSERT;
	else if (str == "DELETE") tokenType = ParsedTokenType::KEYWORD_DELETE;
	else if (str == "WHERE") tokenType = ParsedTokenType::KEYWORD_WHERE;
	else if (str == "PING") tokenType = ParsedTokenType::KEYWORD_PING;
	else if (str == "ECHO") tokenType = ParsedTokenType::KEYWORD_ECHO;
	else if (str == "SOURCE") tokenType = ParsedTokenType::KEYWORD_SOURCE;
	else if (str == "DATA") tokenType = ParsedTokenType::KEYWORD_DATA;
	else if (str == "DEBUG") tokenType = ParsedTokenType::KEYWORD_DEBUG;
	else if (str == "LOAD") tokenType = ParsedTokenType::KEYWORD_LOAD;
	else if (str == "SAVE") tokenType = ParsedTokenType::KEYWORD_SAVE;
	else if (str == "LINK") tokenType = ParsedTokenType::KEYWORD_LINK;
	else if (str == "UNLINK") tokenType = ParsedTokenType::KEYWORD_UNLINK;
	else if (str == "FINAL") tokenType = ParsedTokenType::KEYWORD_FINAL;
	else if (str == "FLUSH") tokenType = ParsedTokenType::KEYWORD_FLUSH;

	return std::pair<TokenInfo, std::string>(TokenInfo(tokenType, line, chr, data0), str);
}

//tokenises str
TokenList FSparqlParser::Tokenize(std::string str) {

	TokenList results;
	std::string buffer;
	bool speechMarks = false;
	bool squareBrackets = false;
	bool triangleBrackets = false;
	bool filter = false;
	bool typing = false;

	unsigned int lineNo = 0;
	unsigned int charNo = 0;

	for (auto iter = str.begin(); iter != str.end(); iter++, charNo++) {

		//if (*iter == '\n') {
		//	lineNo++;
		//	charNo = 0;
		//}

		//whitespace characters delimit tokens
		if ((*iter == '\n' || *iter == ' ' || *iter == '\t') && !speechMarks && !squareBrackets && !triangleBrackets && !filter) {
			if (buffer.length() > 0) {

				results.push_back(identifyToken(buffer, lineNo, charNo - buffer.length()));

				buffer.clear();
			}
			typing = false;
		}
		else {

			if ((*iter == '{' || *iter == '}' || *iter == ';' || *iter == ',' || *iter == '.' /*|| (*iter == ':' && typing == false)*/) && !speechMarks && !squareBrackets && !filter) {
				if (buffer.length() > 0) {
					results.push_back(identifyToken(buffer, lineNo, charNo - buffer.length()));
					buffer.clear();
				}
				buffer.push_back(*iter);
				results.push_back(identifyToken(buffer, lineNo, charNo - buffer.length()));
				buffer.clear();
			}
			else {

				buffer += *iter;

				if (*iter == '^') {
					typing = true;
				}

				if (buffer == "FILTER") {
					filter = true;
				}

				if (filter && *iter == ')') {
					filter = false;
				}

				if (*iter == '"') {
					speechMarks = !speechMarks;
				}

				if (*iter == '[' && !squareBrackets) {
					squareBrackets = true;
				}

				if (*iter == ']' && squareBrackets) {
					squareBrackets = false;
				}

				if (*iter == '<' && !triangleBrackets) {
					triangleBrackets = true;
				}

				if (*iter == '>' && triangleBrackets) {
					triangleBrackets = false;
				}

			}

		}
	}

	if (buffer.length() > 0) {
		results.push_back(identifyToken(buffer, lineNo, charNo - buffer.length()));
		buffer.clear();
	}

	return results;
}

std::string FSparqlParser::parseConfidenceRating(TokenIterator&& iter, TokenIterator end) {
	if (iter->first.type == ParsedTokenType::CONFIDENCE_RATING) {
		std::string val = iter->second;
		iter++;
		return val;
	}
	return "";
}

//parses a block of triples
TriplesBlock FSparqlParser::ParseTriples(TokenIterator&& iter, TokenIterator end) {
   TriplesBlock tripleBlock;
	std::string sub, pred;
	model::Subject::Type subType;
	model::Predicate::Type predType;
	int pos = 0;

	while (iter != end && iter->first.type != ParsedTokenType::CLOSE_CURLBRACE) {

		//spdlog::get("main")->info("Token type: {} Token content: {}", (int)iter->first.type, iter->second);
		
		if (((int)iter->first.type & TOKEN_SPLITTER_MASK) == 0) {
			switch (pos) {
			case 0:
				sub = iter->second;
				switch (iter->first.type) {
				case ParsedTokenType::ENTITYREF:
					subType = model::Subject::Type::ENTITYREF;
               pos = 1;
					break;
				case ParsedTokenType::VARIABLE:
					subType = model::Subject::Type::VARIABLE;
					tripleBlock.variables.insert(iter->second);
               pos = 1;
					break;
            case ParsedTokenType::FILTER:
               tripleBlock.Add(parseFilter(std::move(iter->first), std::move(iter->second)));
			   pos = 0;
               break;
               
				default:
					throw ParseException("Unknown symbol: " + iter->second);
				}				
				break;
			case 1:
				pred = iter->second;
				switch (iter->first.type) {
				case ParsedTokenType::PROPERTY:
					predType = model::Predicate::Type::PROPERTY;
					break;
				case ParsedTokenType::VARIABLE:
					predType = model::Predicate::Type::VARIABLE;
					tripleBlock.variables.insert(iter->second);
					break;
				default:
					throw ParseException("Unknown symbol: " + iter->second);
				}
				pos = 2;
				break;
			case 2:
				model::Object::Type objType;
				std::string confidence = parseConfidenceRating(std::move(iter), end);
				switch (iter->first.type) {
				case ParsedTokenType::STRING:
					objType = model::Object::Type::STRING;
					break;
				case ParsedTokenType::VARIABLE:
					objType = model::Object::Type::VARIABLE;
					tripleBlock.variables.insert(iter->second);
					break;
				case ParsedTokenType::INT:
					objType = model::Object::Type::INT;
					break;
				case ParsedTokenType::ENTITYREF:
					objType = model::Object::Type::ENTITYREF;
					break;
				default:
					throw ParseException("Unknown symbol: " + iter->second);
				}
				model::Object o;
				if (confidence.empty()) {
					o = model::Object(objType, iter->second);
				}
				else {
					o = model::Object(objType, iter->second, std::stoul(confidence));
				}
				model::Triple trip(model::Subject(subType, sub), model::Predicate(predType, pred), o);
            tripleBlock.Add(std::move(trip));
				break;
			}
		}
		else {

			switch (iter->first.type) {
			case ParsedTokenType::SPLITTER1: 
				pos = 0;
				break;
			case ParsedTokenType::SPLITTER2:
				pos = 1;
				break;
			case ParsedTokenType::SPLITTER3:
				pos = 2;
				break;
			default:
				throw ParseException("Unknown symbol: " + iter->second);
			}			
		}

		iter++;
	}

	return tripleBlock;
}

//parses a { } block
TriplesBlock FSparqlParser::ParseInsert(TokenIterator&& iter, TokenIterator end) {

	TriplesBlock output;

	if (iter == end)
		throw ParseException("Unexpected EOF");

	if(iter->first.type != ParsedTokenType::OPEN_CURLBRACE)
		throw ParseException("Expected { found " + iter->second);

	iter++;
	output = ParseTriples(std::move(iter), end);

	if (iter == end)
		throw ParseException("Unexpected EOF");
	
	if (iter->first.type != ParsedTokenType::CLOSE_CURLBRACE)
		throw ParseException("Expected } found " + iter->second);

	iter++;

	return output;
}

/* TODO: uncomment and fix
//parses prolouge
StringMap FSparqlParser::ParseSources(TokenIterator&& iter, TokenIterator end) {

	StringMap sources;

	while (iter != end && iter->second != "INSERT" &&  iter->second != "SELECT" && iter->second != "DELETE") {

		std::string name;

		name = iter->second;
		iter++;

		if (iter != end) {
			if (iter->second == ":") {
				iter++;
				if (iter != end) {
					sources[name] = iter->second;
					iter++;
				}
			}
		}
	}

	return sources;
}
*/

IFilter* FSparqlParser::parseFilter(const TokenInfo&& filterInfo, const std::string&& filterDescription) {
   IFilter* output = nullptr;
   if (filterInfo.data0.length() > 0) {
	   if (filterInfo.data0 == "regex" && RegexFilter::TestAndCreate(&output, filterDescription)) return output;
   }
   else {
	   if (GreaterThanFilter::TestAndCreate(&output, filterDescription)) return output;
   }   
   throw ParseException("Invalid filter description");
}

std::vector<std::string> FSparqlParser::ParseSelectLine(TokenIterator&& iter, TokenIterator end) {
	std::vector<std::string> output;
	while (iter != end && iter->first.type == ParsedTokenType::VARIABLE) {
		output.push_back(iter->second);
		iter++;
	}
	return output;
}

//parses a tokenised list of strings to give a query object
Query FSparqlParser::ParseAll(TokenList tokens) {

	auto iter = tokens.begin();

	QueryType type;
	StringMap sources;
	TriplesBlock conditions;
	TriplesBlock whereClause;
	std::vector<std::string> selectLine;
	std::string data0;
	std::vector<long long int> entities;

	while (iter != tokens.end()) {

		/*if (iter->first.type == ParsedTokenType::KEYWORD_SOURCE) {
			*iter++;
			sources = ParseSources(std::move(iter), tokens.end());
			continue;
		}*/

		if (iter->first.type == ParsedTokenType::KEYWORD_INSERT) {
			*iter++;
			if (iter->first.type == ParsedTokenType::KEYWORD_DATA) {
				iter++;
				type = QueryType::INSERT;
				conditions = ParseInsert(std::move(iter), tokens.end());
			}
			continue;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_WHERE) {
			*iter++;
			whereClause = ParseInsert(std::move(iter), tokens.end());
			continue;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_PING) {
			*iter++;
			type = QueryType::PING;

			if (iter != tokens.end()) {
				throw ParseException("PING does not take any arguments");
			}

			break;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_FLUSH) {
			*iter++;
			type = QueryType::FLUSH;

			if (iter != tokens.end()) {
				throw ParseException("Flush does not take any arguments");
			}

			break;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_ECHO) {
			*iter++;
			type = QueryType::DEBUGECHO;

			if (iter != tokens.end()) {
				data0 = iter->second;
				*iter++;
				if (iter != tokens.end()) {
					throw ParseException("ECHO only takes one argument");
				}
			}

			break;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_DEBUG)
		{
			*iter++;
			type = QueryType::DEBUGOTHER;

			if (iter != tokens.end())
			{
				data0 = iter->second;
				*iter++;
				if (iter != tokens.end())
				{
					throw ParseException("DEBUG commands take a maximum of one argument");
				}
			}
			break;
		}
		
		if (iter->first.type == ParsedTokenType::KEYWORD_LOAD)
		{
			*iter++;
			type = QueryType::LOAD;

			if (iter != tokens.end())
			{
				data0 = iter->second;
				*iter++;
				if (iter != tokens.end())
				{
					throw ParseException("LOAD commands take a maximum of one argument");
				}
			}
			break;
		}
		
		if (iter->first.type == ParsedTokenType::KEYWORD_SAVE)
		{
			*iter++;
			type = QueryType::SAVE;

			if (iter != tokens.end())
			{
				data0 = iter->second;
				*iter++;
				if (iter != tokens.end())
				{
					throw ParseException("SAVE commands take a maximum of one argument");
				}
			}
			break;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_SELECT) {
			iter++;
			type = QueryType::SELECT;
			selectLine = ParseSelectLine(std::move(iter), tokens.end());
			if (iter->first.type == ParsedTokenType::KEYWORD_WHERE) {
				iter++;
				whereClause = ParseInsert(std::move(iter), tokens.end());
			}
			else {
				throw ParseException("Expected 'WHERE'");
			}
			break;
		}

		//linking and merging
		if (iter->first.type == ParsedTokenType::KEYWORD_LINK) {
			iter++;
			if (iter->first.type == ParsedTokenType::KEYWORD_FINAL) {
				type = QueryType::MERGE;
				iter++;
				if(iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to link");
				entities.push_back(std::stoll(iter->second));
				iter++;
				if (iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to link");
				entities.push_back(std::stoll(iter->second));
				if (iter != tokens.end()) {
					throw ParseException("Link only takes 2 arguments");
				}
			}
			else {
				if (iter->first.type == ParsedTokenType::ENTITYREF) {
					entities.push_back(std::stoll(iter->second));
					type = QueryType::LINK;
					iter++;
					if (iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to link");
					entities.push_back(std::stoll(iter->second));
					iter++;
					if (iter != tokens.end()) {
						throw ParseException("Link only takes 2 arguments");
					}
				}
				else {
					throw ParseException("Invalid arguments to link");
				}
			}
			break;
		}

		if (iter->first.type == ParsedTokenType::KEYWORD_UNLINK) {
			iter++;			
			if (iter->first.type == ParsedTokenType::ENTITYREF) {
				entities.push_back(std::stoll(iter->second));
				type = QueryType::LINK;
				iter++;
				if (iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to link");
				entities.push_back(std::stoll(iter->second));
				if (iter != tokens.end()) {
					throw ParseException("Link only takes 2 arguments");
				}
			}
			else {
				throw ParseException("Invalid arguments to link");
			}
			break;
		}

		throw ParseException("Unknown symbol: " + iter->second);
	}

	return Query(type, sources, conditions, whereClause, data0, selectLine, entities);
}
