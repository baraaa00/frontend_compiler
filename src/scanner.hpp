#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using std::cout;
using std::string;
using std::vector;

enum class TokenType {
  KEYWORD,
  STATEMENT,
  OPERATOR,
  SEPARATOR,
  LITERAL,
  IDENTIFIER,
  COMMENT,
  UNKNOWN
};

struct Token {
  TokenType type;
  string value;
  int line; // Optional: for error reporting
};

bool isID(const std::string &str);
bool isComment(const std::string &str);
bool isDigit(const std::string &str);
bool isString(const std::string &str);
bool isBool(const std::string &str);
bool isLiteral(const std::string &str);
bool isKeyword(const std::string &str);
bool isStatement(const std::string &str);
bool isOperator(const std::string &str);
bool isSeparator(const std::string &str);
bool isNoSpaces(const std::string &str);

// Helper to convert string to TokenType (for internal use if needed, or just
// use the bool functions)
TokenType getTokenType(const std::string &str);

// Modified to return tokens
vector<Token> lexicalAnalyze(const std::string &nameOfFile);
