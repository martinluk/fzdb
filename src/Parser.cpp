#include "Parser.h"

#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

TokenItem FSparqlParser::identifyToken(std::string str, unsigned int line, unsigned int chr) {

	boost::regex variableRegex("\\$.*");
	boost::regex stringRegex("\".*\"");
	boost::regex propertyRegex("<.*>");
	boost::regex entityRefRegex("entity:[0-9]+");
	boost::regex intRegex("[0-9]+");

	ParsedTokenType tokenType = ParsedTokenType::NOTIMPLEMENTED;

	if (boost::regex_match(str, variableRegex)) {
		tokenType = ParsedTokenType::VARIABLE;
	}

        else if (boost::regex_match(str, stringRegex)) {
		tokenType = ParsedTokenType::STRING;
		boost::algorithm::trim_if(str, boost::algorithm::is_any_of("\""));
	}

        else if (boost::regex_match(str, propertyRegex)) {
		tokenType = ParsedTokenType::PROPERTY;
		boost::algorithm::trim_if(str, boost::algorithm::is_any_of("<>"));
	}

        else if (boost::regex_match(str, entityRefRegex)) {
		tokenType = ParsedTokenType::ENTITYREF;
		str = str.substr(7, str.length() - 7);
	}
		else if (boost::regex_match(str, intRegex)) {
			tokenType = ParsedTokenType::INT;
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

	return std::pair<TokenInfo, std::string>(TokenInfo(tokenType, line, chr), str);
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

				if (buffer == "FILTER(") {
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

//parses a block of triples
std::vector<model::Triple> FSparqlParser::ParseTriples(TokenIterator&& iter, TokenIterator end) {
	std::vector<model::Triple> triples;
	std::string sub, pred;
	model::Subject::Type subType;
	model::Predicate::Type predType;
	int pos = 0;

	while (iter != end && iter->first.type != ParsedTokenType::CLOSE_CURLBRACE) {

		if (((int)iter->first.type & TOKEN_SPLITTER_MASK) == 0) {
			switch (pos) {
			case 0:
				sub = iter->second;
				switch (iter->first.type) {
				case ParsedTokenType::ENTITYREF:
					subType = model::Subject::Type::ENTITYREF;
					break;
				case ParsedTokenType::VARIABLE:
					subType = model::Subject::Type::VARIABLE;
					break;
				default:
					throw std::runtime_error("Invalid type for subject");
				}
				pos = 1;
				break;
			case 1:
				pred = iter->second;
				switch (iter->first.type) {
				case ParsedTokenType::PROPERTY:
					predType = model::Predicate::Type::PROPERTY;
					break;
				case ParsedTokenType::VARIABLE:
					predType = model::Predicate::Type::VARIABLE;
					break;
				default:
					throw std::runtime_error("Invalid type for predicate");
				}
				pos = 2;
				break;
			case 2:
				model::Object::Type objType;
				switch (iter->first.type) {
				case ParsedTokenType::STRING:
					objType = model::Object::Type::STRING;
					break;
				case ParsedTokenType::VARIABLE:
					objType = model::Object::Type::VARIABLE;
					break;
				case ParsedTokenType::INT:
					objType = model::Object::Type::INT;
					break;
				case ParsedTokenType::ENTITYREF:
					objType = model::Object::Type::ENTITYREF;
					break;
				default:
					throw std::runtime_error("Invalid type for object");
				}
				model::Triple trip(model::Subject(subType, sub), model::Predicate(predType, pred), model::Object(objType, iter->second));
				triples.push_back(trip);
				break;
			}
		}
		else {

			if (iter->first.type == ParsedTokenType::SPLITTER1) {
				pos = 0;
			}

			if (iter->first.type == ParsedTokenType::SPLITTER2) {
				pos = 1;
			}

			if (iter->first.type == ParsedTokenType::SPLITTER3) {
				pos = 2;
			}
		}

		iter++;
	}

	return triples;
}

//parses a { } block
TriplesBlock FSparqlParser::ParseInsert(TokenIterator&& iter, TokenIterator end) {

	TriplesBlock output;

	if (iter->first.type == ParsedTokenType::OPEN_CURLBRACE) {
		iter++;
		output = TriplesBlock(ParseTriples(std::move(iter), end));

		if (iter->first.type == ParsedTokenType::CLOSE_CURLBRACE) {
			iter++;
		}
	}

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

//parses a tokenised list of strings to give a query object
Query FSparqlParser::ParseAll(TokenList tokens) {

	auto iter = tokens.begin();

	QueryType type;
	StringMap sources;
	TriplesBlock conditions;
	TriplesBlock whereClause;
	std::string data0;

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

		if (iter->first.type == ParsedTokenType::KEYWORD_SELECT) {
			*iter++;
			type = QueryType::SELECT;
			whereClause = ParseInsert(std::move(iter), tokens.end());
			break;
		}

		throw ParseException("Unknown symbol: " + iter->second);
	}

	return Query(type, sources, conditions, whereClause, data0);
}
