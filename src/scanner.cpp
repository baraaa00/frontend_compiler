#include "scanner.hpp"
#include <iostream>
#include <string>

using std::cout;
using std::vector;

bool isID(const std::string &str) {
  if (str == "main")
    return false;
  if (std::isdigit(str[0]))
    return false;
  int counter = 0;
  if (str[0] == '_')
    counter++;

  for (; counter < str.size(); counter++)
    if (!(isalnum(str[counter])))
      return false;

  return true;
}
bool isComment(const std::string &str) { return str == "/*" || str == "//"; }

bool isDigit(const std::string &str) {
  return std::all_of(str.begin(), str.end(), ::isdigit);
}

bool isString(const std::string &str) {
  return str.size() >= 2 && str[0] == '"' && str[str.size() - 1] == '"';
}

bool isChar(const std::string &str) {
  return str.size() == 3 && str[0] == '\'' && str[2] == '\'';
}
bool isBool(const std::string &str) { return str == "true" || str == "false"; }

bool isLiteral(const std::string &str) {
  return isDigit(str) || isString(str) || isChar(str) || isBool(str);
}

bool isKeyword(const std::string &str) {
  const vector<std::string> keywords{"int",  "float",  "char",
                                     "auto", "double", "do",
                                     "main", "switch", "return"};
  for (const auto &keyword : keywords)
    if (keyword == str)
      return true;

  return false;
}

bool isStatement(const std::string &str) {
  const vector<std::string> statements{"for", "while", "if", "else"};
  for (const auto &statement : statements)
    if (statement == str)
      return true;

  return false;
}

bool isOperator(const std::string &str) {
  const vector<std::string> operators{
      "<", ">",  "<=", ">=", "*",  "+",  "-",  "/",
      "=", "-=", "*=", "+=", "/=", "++", "--", "=="};
  for (const auto &op : operators)
    if (op == str)
      return true;

  return false;
}

bool isSeparator(const std::string &str) {
  const vector<std::string> Separators{"{", "}", ",", "(", ")", ";"};
  for (const auto &separate : Separators)
    if (separate == str)
      return true;

  return false;
}

bool isNoSpaces(const std::string &str) { return str == " " || str == "\n"; }

TokenType getTokenType(const std::string &token) {
  if (isOperator(token))
    return TokenType::OPERATOR;
  if (isSeparator(token))
    return TokenType::SEPARATOR;
  if (isKeyword(token))
    return TokenType::KEYWORD;
  if (isStatement(token))
    return TokenType::STATEMENT;
  if (isLiteral(token))
    return TokenType::LITERAL;
  if (isID(token))
    return TokenType::IDENTIFIER;
  if (isComment(token))
    return TokenType::COMMENT;
  return TokenType::UNKNOWN;
}

vector<Token> lexicalAnalyze(const std::string &nameOfFile) {
  vector<Token> tokens;
  char ch;
  std::string buffer;
  std::fstream file(nameOfFile, std::fstream::in);

  if (!file.is_open()) {
    cout << "error while opening the file\n";
    exit(0);
  }

  bool miltiCm = false, singleCm = false;
  int lineNumber = 1; // Basic line counting

  while (file >> std::noskipws >> ch) {
    if (ch == '\n')
      lineNumber++;

    if (singleCm || miltiCm) {
      if (singleCm && ch == '\n')
        singleCm = false;

      if (miltiCm && ch == '*') {
        if (!(file >> ch)) {
          std::cerr << "Error: multi-line comment not closed before EOF!\n";
          break;
        }
        if (ch == '/')
          miltiCm = false;
      }
      continue;
    }

    if (ch == '/') {
      char nextCh;
      if (file >> nextCh) {
        if (nextCh == '*') {
          miltiCm = true;
          continue;
        } else if (nextCh == '/') {
          singleCm = true;
          continue;
        } else {
          // Not a comment, push back the character or handle it
          // This part is tricky with stream. Let's just treat '/' as operator
          // if not comment But we already read nextCh. If it was not a comment
          // start, we have '/' and nextCh. We should process '/' and then
          // process nextCh. However, the original logic was a bit different.
          // Let's stick to the original logic structure but adapt for token
          // collection.

          // Actually, let's revert to a simpler lookahead or just use the
          // buffer logic more carefully. The original code had:
          /*
            if (ch == '/') {
              std::string comm(1, ch);
              file >> ch;
              ...
          */
          // I will try to preserve the original flow but fix the logic to not
          // consume if not comment. But `file >> ch` consumes. Let's use `peek`
          // or putback.
          file.putback(nextCh);
        }
      }
    }

    if (isNoSpaces(std::string(1, ch))) {
      if (!buffer.empty()) {
        TokenType type = getTokenType(buffer);
        if (type !=
            TokenType::COMMENT) { // We might skip comments in output tokens
          tokens.push_back({type, buffer, lineNumber});
        }
        buffer = "";
      }
      continue;
    }

    if (isOperator(std::string(1, ch)) && !isOperator(buffer)) {
      if (!buffer.empty()) {
        TokenType type = getTokenType(buffer);
        if (type != TokenType::COMMENT)
          tokens.push_back({type, buffer, lineNumber});
        buffer = "";
      }
    }

    if (!isOperator(std::string(1, ch)) && isOperator(buffer)) {
      TokenType type = getTokenType(buffer);
      if (type != TokenType::COMMENT)
        tokens.push_back({type, buffer, lineNumber});
      buffer = "";
    }

    if (isSeparator(std::string(1, ch))) {
      if (!buffer.empty()) {
        TokenType type = getTokenType(buffer);
        if (type != TokenType::COMMENT)
          tokens.push_back({type, buffer, lineNumber});
        buffer = "";
      }
      // Separator itself
      TokenType type = getTokenType(std::string(1, ch));
      if (type != TokenType::COMMENT)
        tokens.push_back({type, std::string(1, ch), lineNumber});
      continue;
    }
    buffer += ch;
  }

  if (!buffer.empty()) {
    TokenType type = getTokenType(buffer);
    if (type != TokenType::COMMENT)
      tokens.push_back({type, buffer, lineNumber});
  }

  file.close();
  return tokens;
}
