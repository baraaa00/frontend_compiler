#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using std::string;
using std::unordered_map;
using std::vector;

struct SymbolInfo {
  string name;
  string type;
  // Add more info if needed (e.g., line number, constness)
};

class SymbolTable {
private:
  // Stack of scopes, each scope is a map of name -> SymbolInfo
  vector<unordered_map<string, SymbolInfo>> scopes;

public:
  SymbolTable() {
    // Start with a global scope
    enterScope();
  }

  void enterScope() { scopes.push_back({}); }

  void exitScope() {
    if (!scopes.empty()) {
      scopes.pop_back();
    }
  }

  bool insert(const string &name, const string &type) {
    if (scopes.empty())
      return false;

    // Check if already exists in current scope
    auto &currentScope = scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
      return false; // Already declared in this scope
    }

    currentScope[name] = {name, type};
    return true;
  }

  string lookup(const string &name) {
    // Search from inner-most scope to outer-most
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
      auto found = it->find(name);
      if (found != it->end()) {
        return found->second.type;
      }
    }
    return ""; // Not found
  }
};
