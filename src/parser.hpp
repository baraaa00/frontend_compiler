#pragma once

#include "scanner.hpp"
#include "symbol_table.hpp"
#include <string>
#include <vector>

class Parser {
public:
  Parser(const std::vector<Token> &tokens);
  void parse();

private:
  const std::vector<Token> &tokens;
  int current;
  SymbolTable symbolTable;

  Token peek() const;
  Token previous() const;
  Token advance();
  bool check(TokenType type) const;
  bool match(TokenType type);
  Token consume(TokenType type, const std::string &message);
  bool isAtEnd() const;

  void program();
  void statement();
  void declaration();
  void assignment(); // Or expressionStatement
  void block();
  void ifStatement();
  void whileStatement();
  void forStatement();

  // Expression parsing
  void expression();
  void equality();
  void comparison();
  void term();
  void factor();
  void primary();

  void error(const Token &token, const std::string &message);
  void synchronize();
};
