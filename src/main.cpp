#include "parser.hpp"
#include "scanner.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    cout << "Usage: ./myScanner <filename>" << endl;
    return 1;
  }

  string filename = argv[1];
  cout << "Scanning " << filename << "..." << endl;

  try {
    vector<Token> tokens = lexicalAnalyze(filename);

    cout << "Parsing..." << endl;
    Parser parser(tokens);
    parser.parse();

    cout << "Done." << endl;
  } catch (const std::exception &e) {
    cerr << "Error: " << e.what() << endl;
    return 1;
  }

  return 0;
}
