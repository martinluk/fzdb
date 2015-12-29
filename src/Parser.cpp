#include "Parser.h"

//tokenises str
TokenList FSparqlParser::Tokenize(std::string str) {

	TokenList results;
	std::string buffer;
	bool speechMarks = false;
	bool squareBrackets = false;
	bool triangleBrackets = false;
	bool filter = false;
	bool typing = false;

	for (auto iter = str.begin(); iter != str.end(); iter++) {
		if ((*iter == '\n' || *iter == ' ' || *iter == '\t') && !speechMarks && !squareBrackets && !triangleBrackets && !filter) {
			if (buffer.length() > 0) {
				results.push_back(std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType::NOTIMPLEMENTED, 0, 0), buffer));
				buffer.clear();
			}
			typing = false;
		}
		else {

			if ((*iter == '{' || *iter == '}' || *iter == ';' || *iter == ',' || *iter == '.' || (*iter == ':' && typing == false)) && !speechMarks && !squareBrackets && !filter) {
				if (buffer.length() > 0) {
					results.push_back(std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType::NOTIMPLEMENTED, 0, 0), buffer));
					buffer.clear();
				}
				buffer.push_back(*iter);
				results.push_back(std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType::NOTIMPLEMENTED, 0, 0), buffer));
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
		results.push_back(std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType::NOTIMPLEMENTED, 0, 0), buffer));
		buffer.clear();
	}

	return results;
}

//parses a block of triples
std::vector<model::Triple> FSparqlParser::ParseTriples(TokenIterator&& iter, TokenIterator end) {
	std::vector<model::Triple> triples;
	std::string sub;
	std::string pred;
	int pos = 0;

	while (iter != end && iter->second != "}") {

		if (iter->second != ";" && iter->second != "," && iter->second != ".") {
			switch (pos) {
			case 0:
				sub = iter->second;
				pos = 1;
				break;
			case 1:
				pred = iter->second;
				pos = 2;
				break;
			case 2:
				model::Triple trip(sub, pred, iter->second);
				triples.push_back(trip);
				break;
			}
		}
		else {

			if (iter->second == ".") {
				pos = 0;
			}

			if (iter->second == ";") {
				pos = 1;
			}

			if (iter->second == ",") {
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

	if (iter->second == "{") {
		iter++;
		output = TriplesBlock(ParseTriples(std::move(iter), end));

		if (iter->second == "}") {
			iter++;
		}
	}

	return output;
}


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

//parses a tokenised list of strings to give a query object
Query FSparqlParser::ParseAll(TokenList tokens) {

	auto iter = tokens.begin();

	QueryType type;
	StringMap sources;
	TriplesBlock conditions;
	TriplesBlock whereClause;
	std::string data0;

	while (iter != tokens.end()) {

		if (iter->second == "SOURCE") {
			*iter++;
			sources = ParseSources(std::move(iter), tokens.end());
			continue;
		}

		if (iter->second == "INSERT") {
			*iter++;
			if (iter->second == "DATA") {
				iter++;
				type = QueryType::INSERT;
				conditions = ParseInsert(std::move(iter), tokens.end());
			}
			continue;
		}

		if (iter->second == "WHERE") {
			*iter++;
			whereClause = ParseInsert(std::move(iter), tokens.end());
			continue;
		}

		if (iter->second == "PING") {
			*iter++;
			type = QueryType::PING;

			if (iter != tokens.end()) {
				throw ParseException("PING does not take any arguments");
			}

			break;
		}

		if (iter->second == "ECHO") {
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

		throw ParseException("Unknown symbol: " + iter->second);
	}

	return Query(type, sources, conditions, whereClause, data0);
}