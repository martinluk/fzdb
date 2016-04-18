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
    static const boost::regex newEntityRegex("NEW\\( *\\$([a-zA-Z0-9]+) *, *\"([a-zA-Z0-9]+)\" *\\)");
    static const boost::regex simpleDateRegex("(0[1-9]|[1-2][0-9]|3[0-1])\\/(0[1-9]|1[0-2])\\/([0-9][0-9][0-9][0-9])");  // More validation still required.

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

    // This needs to precede the integer check.
    else if ( boost::regex_match(str, matches, simpleDateRegex) )
    {
        tokenType = ParsedTokenType::DATE;
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

   else if (boost::regex_match(str, matches, newEntityRegex)) {
	   tokenType = ParsedTokenType::NEWENTITY;	   
	   data0 = matches[2];
	   str = matches[1];
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
    else if (str == "SELECT") tokenType   = ParsedTokenType ::KEYWORD_SELECT;
    else if (str == "INSERT") tokenType   = ParsedTokenType ::KEYWORD_INSERT;
    else if (str == "DELETE") tokenType   = ParsedTokenType ::KEYWORD_DELETE;
    else if (str == "WHERE") tokenType    = ParsedTokenType ::KEYWORD_WHERE;
    else if (str == "PING") tokenType     = ParsedTokenType ::KEYWORD_PING;
    else if (str == "ECHO") tokenType     = ParsedTokenType ::KEYWORD_ECHO;
    else if (str == "META") tokenType     = ParsedTokenType ::KEYWORD_META;
    else if (str == "DATA") tokenType     = ParsedTokenType ::KEYWORD_DATA;
    else if (str == "DEBUG") tokenType    = ParsedTokenType ::KEYWORD_DEBUG;
    else if (str == "LOAD") tokenType     = ParsedTokenType ::KEYWORD_LOAD;
    else if (str == "SAVE") tokenType     = ParsedTokenType ::KEYWORD_SAVE;
    else if (str == "LINK") tokenType     = ParsedTokenType ::KEYWORD_LINK;
    else if (str == "UNLINK") tokenType   = ParsedTokenType ::KEYWORD_UNLINK;
    else if (str == "FINAL") tokenType    = ParsedTokenType ::KEYWORD_FINAL;
    else if (str == "FLUSH") tokenType    = ParsedTokenType ::KEYWORD_FLUSH;
    else if (str == "CANON") tokenType    = ParsedTokenType ::KEYWORD_CANON;
    else if (str == "USER") tokenType     = ParsedTokenType ::KEYWORD_USER;
    else if (str == "ADD") tokenType      = ParsedTokenType ::KEYWORD_ADD;
    else if (str == "PASSWORD") tokenType = ParsedTokenType ::KEYWORD_PASSWORD;
    else if (str == "PROMOTE") tokenType  = ParsedTokenType ::KEYWORD_PROMOTE;
    else if (str == "DEMOTE") tokenType   = ParsedTokenType ::KEYWORD_DEMOTE;
    else if (str == "LOGIN") tokenType    = ParsedTokenType ::KEYWORD_LOGIN;
    else if (str == "LOGOUT") tokenType   = ParsedTokenType ::KEYWORD_LOGOUT;
    else if (str == "LEVEL") tokenType    = ParsedTokenType ::KEYWORD_LEVEL;

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
        //    lineNo++;
        //    charNo = 0;
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

                if (buffer == "FILTER" || buffer == "NEW") {
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
    bool inMetaBlock = false;
    std::string currentMetaVar = "";

    while (iter != end) {

        //spdlog::get("main")->info("Token type: {} Token content: {}", (int)iter->first.type, iter->second);
        if (iter->first.type == ParsedTokenType::CLOSE_CURLBRACE) {
            
            if (pos != 3) {
                throw ParseException("Incomplete triple!");
            }

            if (inMetaBlock) {
                inMetaBlock = false;
                iter++;
                continue;
            }
            else {
                break;
            }
        }
        
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
                    pos = 3;
                    break;
				case ParsedTokenType::NEWENTITY:
					tripleBlock.Add(iter->second, iter->first.data0);
					pos = 3;
					break;
                case ParsedTokenType::KEYWORD_META:
                    if (inMetaBlock) {
                        throw ParseException("META blocks cannot be nested");
                    }
                    iter++;
                    if (iter->first.type != ParsedTokenType::VARIABLE) {
                        throw ParseException("META blocks must have a variable");
                    }
                    currentMetaVar = iter->second;
                    tripleBlock.metaVariables.insert(iter->second);
                    tripleBlock.variables.insert(iter->second);
                    iter++;
                    if (iter->first.type != ParsedTokenType::OPEN_CURLBRACE) {
                        throw ParseException("Expected { found " + iter->second);
                    }
                    inMetaBlock = true;
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
                case ParsedTokenType::DATE:
                    objType = model::Object::Type::DATE;
                    break;
                default:
                    throw ParseException("Unknown symbol: " + iter->second);
                }
                model::Object o;
                if (confidence.empty()) {
                    o = model::Object(objType, iter->second);
                }
                else {
					unsigned long conf = std::stoul(confidence);
					if (conf < 0 || conf > 100) {
						throw ParseException("Confidence values must be integers between 0 and 100");
					}
                    o = model::Object(objType, iter->second, static_cast<char>(conf));
                }
                
                if (!inMetaBlock) {
                    model::Triple trip(model::Subject(subType, sub), model::Predicate(predType, pred), o);
                    tripleBlock.Add(std::move(trip));
                }
                else {
                    model::Triple trip(model::Subject(subType, sub), model::Predicate(predType, pred), o, currentMetaVar);
                    tripleBlock.Add(std::move(trip));
                }
				pos = 3;
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
    std::string data1;
    std::vector<long long int> entities;
    QuerySettings canon;

    //while (iter != tokens.end()) {
    do {
        if (iter->first.type == ParsedTokenType::KEYWORD_INSERT) {
            *iter++;
            if (iter->first.type == ParsedTokenType::KEYWORD_DATA) {
                iter++;
                type = QueryType::INSERT;
                conditions = ParseInsert(std::move(iter), tokens.end());				
            }

			if (iter != tokens.end() && iter->first.type == ParsedTokenType::KEYWORD_WHERE) {
				*iter++;
				whereClause = ParseInsert(std::move(iter), tokens.end());
				break;
			}

            break;
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
            if (iter != tokens.end()) {
                if (iter->first.type == ParsedTokenType::KEYWORD_CANON) {
                    canon.canon = true;
                    iter++;
                }

                selectLine = ParseSelectLine(std::move(iter), tokens.end());
                if (iter != tokens.end() && iter->first.type == ParsedTokenType::KEYWORD_WHERE) {
                    iter++;
                    whereClause = ParseInsert(std::move(iter), tokens.end());
                }
                else {
                    throw ParseException("Expected 'WHERE'");
                }
            }
            else {
                throw ParseException("Incomplete SELECT statement");
            }

            break;
        }

        //linking and merging
        if (iter->first.type == ParsedTokenType::KEYWORD_LINK) {
            iter++;
            if (iter->first.type == ParsedTokenType::KEYWORD_FINAL) {
                type = QueryType::MERGE;
                iter++;
                if (iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to link");
                entities.push_back(std::stoll(iter->second));
                iter++;
                if (iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to link");
                entities.push_back(std::stoll(iter->second));
                iter++;
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
                type = QueryType::UNLINK;
                iter++;
                if (iter->first.type != ParsedTokenType::ENTITYREF) throw ParseException("Invalid arguments to unlink");
                entities.push_back(std::stoll(iter->second));
                iter++;
                if (iter != tokens.end()) {
                    throw ParseException("Unlink only takes 2 arguments");
                }
            }
            else {
                throw ParseException("Invalid arguments to unlink");
            }
            break;
        }

        if (iter->first.type == ParsedTokenType::KEYWORD_DELETE) {
            //Sample query 
            //Delete WHERE {$a 'surname' 'Fred'}
            iter++;
            type = QueryType::DELETECMD;
            if (iter != tokens.end()) {
                if (iter != tokens.end() && iter->first.type == ParsedTokenType::KEYWORD_WHERE) {
                    iter++;
                    whereClause = ParseInsert(std::move(iter), tokens.end());
                }
                else { throw ParseException("Expected 'WHERE' at delete statement."); }
            }
            else {
                throw ParseException("Incomplete DELETE statement");
            }
            break;
        }

        if(iter->first.type == ParsedTokenType::KEYWORD_USER) {
            iter++;
            //Can either be UADD, UDELETE, UPASSWORD, UPROMOTE, UDEMOTE, ULOGIN, ULOGOUT, LEVEL
            int numberOfArg=-1;
            switch(iter->first.type) {
                case ParsedTokenType::KEYWORD_ADD: 
                    type = QueryType::USER_ADD;
                    numberOfArg=2;
                    break;
                case ParsedTokenType::KEYWORD_DELETE: 
                    type=QueryType::USER_DELETE;
                    numberOfArg=1;
                    break;
                case ParsedTokenType::KEYWORD_PASSWORD: 
                    type=QueryType::USER_PASSWORD;
                    numberOfArg=2;
                    break;
                case ParsedTokenType::KEYWORD_PROMOTE: 
                    type=QueryType::USER_PROMOTE;
                    numberOfArg=1;
                    break;
                case ParsedTokenType::KEYWORD_DEMOTE: 
                    type=QueryType::USER_DEMOTE;
                    numberOfArg=1;
                    break;
                case ParsedTokenType::KEYWORD_LOGIN: 
                    type=QueryType::USER_LOGIN;
                    numberOfArg=2;
                    break;
                case ParsedTokenType::KEYWORD_LOGOUT: 
                    type=QueryType::USER_LOGOUT;
                    numberOfArg=0;
                    break;
                case ParsedTokenType::KEYWORD_LEVEL: 
                    type=QueryType::USER_LEVEL;
                    numberOfArg=0;
                    break;
                default:
                    throw ParseException("Invalid arguments following USER");
            }
            //iter now=Keyword
            switch(numberOfArg) {
                case 0:
                    //Finished
                    iter++;
                    break;
                case 1:
                    iter++;
                    if (iter!=tokens.end()) {
                        data0 = iter->second; //Retrieve data from first argument
                        iter++; 
                    } else {
                        throw ParseException("Expected 1 argument but receieved 0.");
                    }
                    break;
                case 2:
                    iter++;
                    //iter now=arg1
                    if (iter != tokens.end()) {
                        data0 = iter->second; //Retrieve data from first argument
                        iter++;
                        //iter now=arg2
                        if (iter == tokens.end()) { throw ParseException("Expected 2 argument but receieved 1."); }
                        data1 = iter->second; //Retrieve data from second argument
                        iter++;
                    } else {
                        throw ParseException("Expected 1 argument but receieved 0.");
                    }
                    break;
                default:
                    assert(numberOfArg>=0 /*Make sure numberOfArg is assigned*/);
                    assert(numberOfArg<=2 /*Query class only accept two args at most, implement otherwise if nessasary*/);
            }
            break;
        }

        throw ParseException("Unknown symbol: " + iter->second);
    } while(true);

    if (iter != tokens.end()) {
        throw ParseException("Unexpected symbol: " + iter->second);
    }

	if (conditions.HasNewEntities() || (whereClause.HasNewEntities() && type != QueryType::INSERT)) {
		throw ParseException("The NEW keyword is only allowed in WHERE sections of INSERT queries");
	}

    return Query(type, sources, conditions, whereClause, data0, data1, selectLine, entities, canon);
}
