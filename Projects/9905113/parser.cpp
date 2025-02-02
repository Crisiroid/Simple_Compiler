#include <iostream>
#include <vector>
#include <string>

using namespace std;

vector<string> tokens; // List of tokens
int currentTokenIndex = 0; // Current token index

// Function to get the current token
string getCurrentToken() {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    return ""; // Return an empty string if no more tokens
}

// Function to move to the next token
void advanceToken() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
}

// Function to match the expected token with the current token
bool match(string expected) {
    if (getCurrentToken() == expected) {
        advanceToken();
        return true;
    } else {
        cout << "Error: Expected '" << expected << "' but found '" << getCurrentToken() << "'" << endl;
        return false;
    }
}

// Function to parse the program
bool parseProgram() {
    cout << "Parsing Program..." << endl;

    if (!match("Keyword: Program")) { // Program should start with 'Program'
        return false;
    }
    if (!match("Keyword: Var")) { // Must have a 'Var' section
        return false;
    }
    if (!match("Identifier: x")) { // There must be an identifier, like 'x'
        return false;
    }
    if (!match("Symbol: ;")) { // Semicolon after variable declaration
        return false;
    }
    if (!match("Keyword: Start")) { // 'Start' keyword to begin the block
        return false;
    }
    if (!match("Keyword: Print")) { // Print keyword should be present
        return false;
    }
    if (!match("Symbol: (")) { // Opening parenthesis for the print statement
        return false;
    }
    if (!match("Identifier: x")) { // 'x' should be printed
        return false;
    }
    if (!match("Symbol: )")) { // Closing parenthesis for the print statement
        return false;
    }
    if (!match("Symbol: ;")) { // Semicolon after the print statement
        return false;
    }
    if (!match("Keyword: End")) { // 'End' keyword should be present to close the block
        return false;
    }
    if (!match("Keyword: end")) { // The program should end with 'end'
        return false;
    }

    return true; // Program parsed successfully
}

int main() {
    // Simulating the output from the lexer
    tokens = {
        "Keyword: Program", "Keyword: Var", "Identifier: x", "Symbol: ;",
        "Keyword: Start", "Keyword: Print", "Symbol: (", "Identifier: x", "Symbol: )",
        "Symbol: ;", "Keyword: End", "Keyword: end"
    };

    // Parsing the program
    if (parseProgram()) {
        cout << "The program is syntactically correct." << endl;
    } else {
        cout << "Syntactic error in the program!" << endl;
    }

    return 0;
}
