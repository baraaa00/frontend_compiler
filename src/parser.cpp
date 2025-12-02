#include "parser.hpp"
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens), current(0) {}

void Parser::parse() {
  try {
    program();
  } catch (const std::runtime_error &error) {
    // Error already reported
  }
}

Token Parser::peek() const { return tokens[current]; }

Token Parser::previous() const { return tokens[current - 1]; }

Token Parser::advance() {
  if (!isAtEnd())
    current++;
  return previous();
}

bool Parser::check(TokenType type) const {
  if (isAtEnd())
    return false;
  return peek().type == type;
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

Token Parser::consume(TokenType type, const std::string &message) {
  if (check(type))
    return advance();
  error(peek(), message);
  throw std::runtime_error(message);
}

bool Parser::isAtEnd() const { return current >= tokens.size(); }

void Parser::error(const Token &token, const std::string &message) {
  cerr << "[Line " << token.line << "] Error at '" << token.value
       << "': " << message << endl;
}

void Parser::synchronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().type == TokenType::SEPARATOR && previous().value == ";")
      return;
    if (peek().type == TokenType::KEYWORD ||
        peek().type == TokenType::STATEMENT)
      return;
    advance();
  }
}

// Helper to check specific value
bool matchValue(const Token &token, const std::string &val) {
  return token.value == val;
}

void Parser::program() {
  while (!isAtEnd()) {
    statement();
  }
}

void Parser::statement() {
  Token t = peek();
  if (t.type == TokenType::KEYWORD) {
    // Could be declaration (int x;) or return
    if (t.value == "return") {
      advance();
      if (!check(TokenType::SEPARATOR) || peek().value != ";") {
        expression();
      }
      consume(TokenType::SEPARATOR, "Expect ';' after return value.");
      if (previous().value != ";")
        error(previous(), "Expect ';' after return value.");
    } else {
      declaration();
    }
  } else if (t.type == TokenType::STATEMENT) {
    if (t.value == "for")
      forStatement();
    else if (t.value == "while")
      whileStatement();
    else if (t.value == "if")
      ifStatement();
    else {
      error(t, "Unknown statement.");
      advance();
    }
  } else if (t.type == TokenType::SEPARATOR && t.value == "{") {
    block();
  } else if (t.type == TokenType::IDENTIFIER) {
    // Assignment or function call (assuming assignment for now as per sample)
    // Or it could be 'x int ;' error case.
    // If we see ID, we expect assignment or expression.
    // Let's try to parse as expression statement.
    expression();
    if (check(TokenType::SEPARATOR) && peek().value == ";") {
      advance();
    } else {
      error(peek(), "Expect ';' after expression.");
    }
  } else if (t.type == TokenType::SEPARATOR && t.value == ";") {
    advance(); // Empty statement
  } else {
    error(t, "Unexpected token.");
    advance();
  }
}

void Parser::declaration() {
  // Type ID ; or Type ID = Expr ;
  Token typeToken = advance(); // We know it's a keyword

  if (!check(TokenType::IDENTIFIER)) {
    error(peek(), "Expect variable name.");
    return;
  }

  Token nameToken = advance();

  if (check(TokenType::OPERATOR) && peek().value == "=") {
    advance();
    expression(); // Initializer
  }

  if (check(TokenType::SEPARATOR) && peek().value == ";") {
    advance();
  } else {
    error(peek(), "Expect ';' after variable declaration.");
  }

  // Semantic Check: Declare variable
  if (!symbolTable.insert(nameToken.value, typeToken.value)) {
    error(nameToken,
          "Variable '" + nameToken.value + "' already declared in this scope.");
  } else {
    cout << "Declared variable: " << nameToken.value << " of type "
         << typeToken.value << endl;
  }
}

void Parser::block() {
  consume(TokenType::SEPARATOR,
          "Expect '{' to begin block."); // Check value too?
  if (previous().value != "{")
    error(previous(), "Expect '{'.");

  symbolTable.enterScope();
  while (!isAtEnd() && !(check(TokenType::SEPARATOR) && peek().value == "}")) {
    statement();
  }
  consume(TokenType::SEPARATOR, "Expect '}' after block.");
  if (previous().value != "}")
    error(previous(), "Expect '}'.");
  symbolTable.exitScope();
}

void Parser::forStatement() {
  advance(); // 'for'
  consume(TokenType::SEPARATOR, "Expect '(' after 'for'.");
  if (previous().value != "(")
    error(previous(), "Expect '('.");

  // Initializer
  if (check(TokenType::KEYWORD)) {
    declaration();
  } else if (check(TokenType::IDENTIFIER)) {
    expression();
    consume(TokenType::SEPARATOR, "Expect ';' after loop initializer.");
    if (previous().value != ";")
      error(previous(), "Expect ';'.");
  } else if (check(TokenType::SEPARATOR) && peek().value == ";") {
    advance();
  } else {
    error(peek(), "Expect expression or declaration in for initializer.");
  }

  // Condition
  if (!check(TokenType::SEPARATOR) || peek().value != ";") {
    expression();
  }
  consume(TokenType::SEPARATOR, "Expect ';' after loop condition.");
  if (previous().value != ";")
    error(previous(), "Expect ';'.");

  // Increment
  if (!check(TokenType::SEPARATOR) || peek().value != ")") {
    expression();
  }
  consume(TokenType::SEPARATOR, "Expect ')' after for clauses.");
  if (previous().value != ")")
    error(previous(), "Expect ')'.");

  statement(); // Body
}

void Parser::ifStatement() {
  advance(); // 'if'
  consume(TokenType::SEPARATOR, "Expect '(' after 'if'.");
  if (previous().value != "(")
    error(previous(), "Expect '('.");
  expression();
  consume(TokenType::SEPARATOR, "Expect ')' after condition.");
  if (previous().value != ")")
    error(previous(), "Expect ')'.");
  statement();

  if (check(TokenType::KEYWORD) && peek().value == "else") {
    advance();
    statement();
  }
}

void Parser::whileStatement() {
  advance(); // 'while'
  consume(TokenType::SEPARATOR, "Expect '(' after 'while'.");
  if (previous().value != "(")
    error(previous(), "Expect '('.");
  expression();
  consume(TokenType::SEPARATOR, "Expect ')' after condition.");
  if (previous().value != ")")
    error(previous(), "Expect ')'.");
  statement();
}

void Parser::expression() {
  // Simple assignment support: ID = Expr
  // Or just equality/comparison
  // We need to handle assignment here because it's right associative and low
  // precedence

  // Lookahead to see if it's assignment
  if (check(TokenType::IDENTIFIER)) {
    // We need to see if next is '=' or '+=' etc.
    // But recursive descent usually handles this by parsing lower precedence
    // first. Assignment is lowest.

    // However, standard C expression grammar:
    // expression -> assignment_expression
    // assignment_expression -> conditional_expression | unary_expression
    // assignment_operator assignment_expression

    // Let's simplify:
    // expression -> term { (+|-) term } ...
    // But we want to support x = 5

    // Let's try parsing an equality. If we hit '=', it was an l-value.
    // This requires more complex parsing or backtracking.
    // Simplified approach: check if current is ID and next is '='

    if (current + 1 < tokens.size() &&
        tokens[current + 1].type == TokenType::OPERATOR &&
        (tokens[current + 1].value == "=" ||
         tokens[current + 1].value == "+=" ||
         tokens[current + 1].value == "-=")) {

      Token nameToken = advance();
      string op = advance().value;
      expression(); // Recursive for right side

      // Semantic Check: Variable must exist
      string type = symbolTable.lookup(nameToken.value);
      if (type == "") {
        error(nameToken, "Undefined variable '" + nameToken.value + "'.");
      } else {
        // Type checking could go here (e.g. assigning string to int)
        cout << "Assignment to " << nameToken.value << endl;
      }
      return;
    }
  }

  equality();
}

void Parser::equality() {
  comparison();
  while (check(TokenType::OPERATOR) &&
         (peek().value == "==" || peek().value == "!=")) {
    advance();
    comparison();
  }
}

void Parser::comparison() {
  term();
  while (check(TokenType::OPERATOR) &&
         (peek().value == "<" || peek().value == ">" || peek().value == "<=" ||
          peek().value == ">=")) {
    advance();
    term();
  }
}

void Parser::term() {
  factor();
  while (check(TokenType::OPERATOR) &&
         (peek().value == "+" || peek().value == "-")) {
    advance();
    factor();
  }
}

void Parser::factor() {
  // * /
  primary();
  while (check(TokenType::OPERATOR) &&
         (peek().value == "*" || peek().value == "/")) {
    advance();
    primary();
  }
}

void Parser::primary() {
  if (check(TokenType::LITERAL)) {
    advance();
    return;
  }
  if (check(TokenType::IDENTIFIER)) {
    Token t = advance();
    // Semantic Check: Usage
    if (symbolTable.lookup(t.value) == "") {
      error(t, "Undefined variable '" + t.value + "'.");
    }

    // Check for post-increment/decrement
    if (check(TokenType::OPERATOR) &&
        (peek().value == "++" || peek().value == "--")) {
      advance();
    }
    return;
  }
  if (check(TokenType::SEPARATOR) && peek().value == "(") {
    advance();
    expression();
    consume(TokenType::SEPARATOR, "Expect ')' after expression.");
    if (previous().value != ")")
      error(previous(), "Expect ')'.");
    return;
  }

  error(peek(), "Expect expression.");
  advance();
}
