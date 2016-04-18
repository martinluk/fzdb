#include <limits.h>
#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <iterator>
#include <vector>
#include <set>
#include <string>
#include <map>
#include <utility>

#include "parser.h"

class ParserTestIdentifyToken : public ::testing::Test {
protected:
	virtual void setUp() {

	}
	virtual void TearDown() {

	}

public:

    void assertQueryParse(std::string query, QueryType type, std::string data0, std::string data1) {
        EXPECT_NO_THROW(FSparqlParser::ParseAll(FSparqlParser::Tokenize(query)));
        Query q=FSparqlParser::ParseAll(FSparqlParser::Tokenize(query));
        ASSERT_EQ(type,q.type);
        ASSERT_EQ(data0,q.data0);
        ASSERT_EQ(data1,q.data1);
    }

    void assertQueryParse(std::string query, QueryType type, std::string data0) {
        assertQueryParse(query, type, data0, "");
    }

    void assertQueryParse(std::string query, QueryType type) {
        assertQueryParse(query, type, "", "");
    }

    bool are_equal(TokenItem token, TokenItem token1) {
        return ((token.first.type == token1.first.type) && (token.first.lineNumber == token1.first.lineNumber) && (token.first.charPosition== token1.first.charPosition) && (token.first.data0 == token1.first.data0) && (token.second == token1.second));
    }
    
    void assertQueryThrow(std::string query) {
        ASSERT_THROW(FSparqlParser::ParseAll(FSparqlParser::Tokenize(query)),ParseException);
    }

};

TEST_F(ParserTestIdentifyToken, UserLogoutQueryParseTest) {
    assertQueryParse("USER LOGOUT", QueryType::USER_LOGOUT);
    assertQueryParse("USER LOGOUT ", QueryType::USER_LOGOUT);
    assertQueryThrow("USER LOGOUT username");
    assertQueryThrow("USER LOGOUT username password");
    assertQueryThrow("USER LOGOUT username password type");
}

TEST_F(ParserTestIdentifyToken, UserPromoteQueryParseTest) {
    assertQueryParse("USER PROMOTE username", QueryType::USER_PROMOTE, "username");
    assertQueryParse("USER PROMOTE USER", QueryType::USER_PROMOTE, "USER");
    assertQueryThrow("USER PROMOTE");
    assertQueryThrow("USER PROMOTE username username2");
    assertQueryThrow("USER PROMOTE username username2 username3");
}

TEST_F(ParserTestIdentifyToken, UserLoginQueryParseTest) {
    assertQueryParse("USER LOGIN username password", QueryType::USER_LOGIN, "username", "password");
    assertQueryParse("USER LOGIN USER DELETE", QueryType::USER_LOGIN, "USER", "DELETE");
    assertQueryThrow("USER LOGIN username");
    assertQueryThrow("USER LOGIN username password type");
    //XXX Format string should also be tested.
}

TEST_F(ParserTestIdentifyToken, UserAddQueryParseTest) {
    assertQueryParse("USER ADD username password", QueryType::USER_ADD,
            "username", "password");
    assertQueryParse("USER ADD USER DELETE", QueryType::USER_ADD, "USER", "DELETE");
    assertQueryThrow("USER ADD");
    assertQueryThrow("USER ADD username");
    assertQueryThrow("USER ADD username password type");
}

TEST_F(ParserTestIdentifyToken, UserPasswordQueryParseTest) {
    assertQueryParse("USER PASSWORD opw npw", QueryType::USER_PASSWORD,
            "opw", "npw");
    assertQueryParse("USER PASSWORD USER DELETE", QueryType::USER_PASSWORD,
            "USER", "DELETE");
    assertQueryThrow("USER PASSWORD username");
    assertQueryThrow("USER PASWORD username password type");
}

TEST_F(ParserTestIdentifyToken, UserDeleteQueryParseTest) {
    assertQueryParse("USER DELETE username", QueryType::USER_DELETE, "username");
    assertQueryParse("USER DELETE USER", QueryType::USER_DELETE, "USER");
    assertQueryThrow("USER DELETE");
    assertQueryThrow("USER DELETE username username2");
    assertQueryThrow("USER DELETE username username2 username3");
    assertQueryThrow("USER DELETE username username2 username3 username4");
}
//tests for $
TEST_F(ParserTestIdentifyToken, testVariable) {
	std::string str = "$abc";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x0), 1, 1, ""), "abc");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 1, 1, ""), "abc");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x0), 2, 1, ""), "abc");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x0), 1, 2, ""), "abc");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x0), 1, 1, "a"), "abc");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x0), 1, 1, ""), "{");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for OPEN_CURLBRACE
TEST_F(ParserTestIdentifyToken, testOpenCurlbrace) {
	std::string str = "{";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType (0x6), 1, 1, ""), "{");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "{");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 2, 1, ""), "{");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 1, 2, ""), "{");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 1, 1, "a"), "{");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for CLOSED_CURLBRACE
TEST_F(ParserTestIdentifyToken, testClosedCurlbrace) {
	std::string str = "}";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "}");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 1, 1, ""), "}");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 2, 1, ""), "}");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 2, ""), "}");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, "a"), "}");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "{");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for FILTER
TEST_F(ParserTestIdentifyToken, testFILTER) {
	std::string str = "FILTER";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x9), 1, 1, ""), "FILTER"); //not implemented yet the filter function
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x6), 1, 1, ""), "FILTER");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x9), 2, 1, ""), "FILTER");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x9), 1, 2, ""), "FILTER");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x9), 1, 1, "a"), "FILTER");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x9), 1, 1, ""), "{");

//	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for Splitter1 .
TEST_F(ParserTestIdentifyToken, testSplitter1) {
	std::string str = ".";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 1, ""), ".");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), ".");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 2, 1, ""), ".");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 2, ""), ".");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 1, "a"), ".");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for Splitter2 ;
TEST_F(ParserTestIdentifyToken, testSplitter2) {
	std::string str = ";";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 1, ""), ";");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), ";");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 2, 1, ""), ";");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 2, ""), ";");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 1, "a"), ";");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for Splitter3 ,
TEST_F(ParserTestIdentifyToken, testSplitter3) {
	std::string str = ",";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 1, ""), ",");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), ",");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 2, 1, ""), ",");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 2, ""), ",");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 1, "a"), ",");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

	bool test = are_equal(token, token1);

}

//tests for SELECT_KEYWORD SELECT
TEST_F(ParserTestIdentifyToken, testSELECT) {
	std::string str = "SELECT";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x0), 1, 1, ""), "SELECT");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "SELECT");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 2, 1, ""), "SELECT");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 2, ""), "SELECT");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 1, "a"), "SELECT");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x0), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for WHERE_KEYWORD WHERE
TEST_F(ParserTestIdentifyToken, testWHERE) {
	std::string str = "WHERE";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x1), 1, 1, ""), "WHERE");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "WHERE");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 2, 1, ""), "WHERE");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 2, ""), "WHERE");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 1, "a"), "WHERE");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x1), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for INSERT_KEYWORD INSERT
TEST_F(ParserTestIdentifyToken, testINSERT) {
	std::string str = "INSERT";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x2), 1, 1, ""), "INSERT");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "INSERT");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 2, 1, ""), "INSERT");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 2, ""), "INSERT");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 1, "a"), "INSERT");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x2), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for DELETE_KEYWORD DELETE
TEST_F(ParserTestIdentifyToken, testDELETE) {
	std::string str = "DELETE";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x3), 1, 1, ""), "DELETE");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "DELETE");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x3), 2, 1, ""), "DELETE");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x3), 1, 2, ""), "DELETE");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x3), 1, 1, "a"), "DELETE");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x3), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for PING_KEYWORD PING
TEST_F(ParserTestIdentifyToken, testPING) {
	std::string str = "PING";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x4), 1, 1, ""), "PING");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "PING");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x4), 2, 1, ""), "PING");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x4), 1, 2, ""), "PING");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x4), 1, 1, "a"), "PING");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x4), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}


//tests for ECHO_KEYWORD ECHO
TEST_F(ParserTestIdentifyToken, testECHO) {
	std::string str = "ECHO";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x5), 1, 1, ""), "ECHO");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "ECHO");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x5), 2, 1, ""), "ECHO");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x5), 1, 2, ""), "ECHO");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x5), 1, 1, "a"), "ECHO");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x5), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for SOURCE_KEYWORD SOURCE
// TEST_F(ParserTestIdentifyToken, testSOURCE) {
// 	std::string str = "SOURCE";
// 	const unsigned int line = 1;
// 	const unsigned int chr = 1;
// 	const std::string data0 = "";

// 	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
// 	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x6), 1, 1, ""), "SOURCE");
// 	//wrong token type
// 	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "SOURCE");
// 	//wrong line number
// 	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x6), 2, 1, ""), "SOURCE");
// 	//wrong character position
// 	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x6), 1, 2, ""), "SOURCE");
// 	//wrong data field
// 	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x6), 1, 1, "a"), "SOURCE");
// 	//wrong string
// 	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x6), 1, 1, ""), "}");

// 	EXPECT_TRUE(are_equal(token, token1));
// 	EXPECT_FALSE(are_equal(token, token2));
// 	EXPECT_FALSE(are_equal(token, token3));
// 	EXPECT_FALSE(are_equal(token, token4));
// 	EXPECT_FALSE(are_equal(token, token5));
// 	EXPECT_FALSE(are_equal(token, token6));

// }


//tests for DATA_KEYWORD DATA
TEST_F(ParserTestIdentifyToken, testDATA) {
	std::string str = "DATA";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x7), 1, 1, ""), "DATA");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "DATA");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x7), 2, 1, ""), "DATA");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x7), 1, 2, ""), "DATA");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x7), 1, 1, "a"), "DATA");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x7), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for DEBUG_KEYWORD DEBUG
TEST_F(ParserTestIdentifyToken, testDEBUG) {
	std::string str = "DEBUG";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x8), 1, 1, ""), "DEBUG");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "DEBUG");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x8), 2, 1, ""), "DEBUG");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x8), 1, 2, ""), "DEBUG");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x8), 1, 1, "a"), "DEBUG");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x8), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for LOAD_KEYWORD LOAD
TEST_F(ParserTestIdentifyToken, testLOAD) {
	std::string str = "LOAD";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0x9), 1, 1, ""), "LOAD");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "LOAD");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x9), 2, 1, ""), "LOAD");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x9), 1, 2, ""), "LOAD");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x9), 1, 1, "a"), "LOAD");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0x9), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}

//tests for SAVE_KEYWORD SAVE
TEST_F(ParserTestIdentifyToken, testSAVE) {
	std::string str = "SAVE";
	const unsigned int line = 1;
	const unsigned int chr = 1;
	const std::string data0 = "";

	TokenItem token = FSparqlParser::identifyToken(str, line, chr);
	TokenItem token1 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_KEYWORD_MASK | 0xA), 1, 1, ""), "SAVE");
	//wrong token type
	TokenItem token2 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(0x7), 1, 1, ""), "SAVE");
	//wrong line number
	TokenItem token3 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0xA), 2, 1, ""), "SAVE");
	//wrong character position
	TokenItem token4 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0xA), 1, 2, ""), "SAVE");
	//wrong data field
	TokenItem token5 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0xA), 1, 1, "a"), "SAVE");
	//wrong string
	TokenItem token6 = std::pair<TokenInfo, std::string>(TokenInfo(ParsedTokenType(TOKEN_SPLITTER_MASK | 0xA), 1, 1, ""), "}");

	EXPECT_TRUE(are_equal(token, token1));
	EXPECT_FALSE(are_equal(token, token2));
	EXPECT_FALSE(are_equal(token, token3));
	EXPECT_FALSE(are_equal(token, token4));
	EXPECT_FALSE(are_equal(token, token5));
	EXPECT_FALSE(are_equal(token, token6));

}



    /*
    Query q = FSparqlParser::ParseAll(FSparqlParser::Tokenize("USER LOGOUT"));
    QueryType type = QueryType::USER_LOGOUT;
    std::string data0 = ""; std::string data1="";
    ASSERT_EQ(type,q.type);
    ASSERT_EQ(data0,q.data0);
    ASSERT_EQ(data1,q.data1);
    */
