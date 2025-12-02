# C-like Language Scanner, Parser, and Semantic Analyzer

This project implements a Scanner (Lexical Analyzer), Parser, and Semantic Analyzer for a C-like programming language in C++.

It takes a source file as input, tokenizes it, parses the tokens into a structure (implicitly via recursive descent), and performs semantic checks such as variable declaration and type consistency.

## Features

*   **Scanner**: Tokenizes the input file into identifiers, keywords, separators, operators, literals, and comments.
*   **Parser**: A recursive descent parser that handles:
    *   Variable declarations (`int x;`, `int x = 5;`)
    *   Assignments (`x = 5;`)
    *   Control flow statements (`if`, `else`, `while`, `for`)
    *   Blocks (`{ ... }`)
    *   Expressions with operator precedence
*   **Semantic Analysis**:
    *   Symbol Table management for variable scopes.
    *   Detection of undeclared variables.
    *   Detection of redeclared variables in the same scope.

## Prerequisites

You need a C++ compiler (like `g++`) supporting C++11 or higher.

## Building

To compile the project, run the following command in the project root:

```bash
g++ src/main.cpp src/scanner.cpp src/parser.cpp -o myScanner
```

## Usage

Run the compiled executable with the path to your source file:

```bash
./myScanner <filename>
```

### Example

Create a file named `test.txt` with the following content:

```c
int a = 10;
if (a > 5) {
    a = a + 1;
}
```

Run the scanner:

```bash
./myScanner test.txt
```

**Output:**

```
Scanning test.txt...
Parsing...
Declared variable: a of type int
Assignment to a
Done.
```

## Project Structure

*   `src/scanner.hpp` / `src/scanner.cpp`: Lexical analyzer implementation.
*   `src/parser.hpp` / `src/parser.cpp`: Recursive descent parser implementation.
*   `src/symbol_table.hpp`: Symbol table for semantic analysis.
*   `src/main.cpp`: Entry point of the application.

## License

This project is licensed under the MIT License.
