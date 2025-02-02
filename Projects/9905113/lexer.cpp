#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <unordered_set>

using namespace std;

// List of keywords
unordered_set<string> keywords = {"Program", "Var", "Start", "End", "If", "Iteration", "Print", "Read", "Put"};

class Lexer {
private:
    string input;
    size_t pos = 0;
    
    // Function to get the next character from the input string
    char getNextChar() {
        return pos < input.size() ? input[pos++] : '\0';
    }
    
    // Function to unread the last character
    void unreadChar() {
        if (pos > 0) pos--;
    }
    
public:
    Lexer(const string& input) : input(input) {}

    // Tokenization function to process the input string
    vector<string> tokenize() {
        vector<string> tokens;
        char c;
        
        while ((c = getNextChar()) != '\0') {
            // Skip whitespaces
            if (isspace(c)) continue;

            // Identify keywords and identifiers
            if (isalpha(c)) {
                string token;
                token += c;
                while (isalnum(c = getNextChar())) {
                    token += c;
                }
                unreadChar(); // Unread the last character

                // Check if the token is a keyword or an identifier
                if (keywords.count(token)) {
                    tokens.push_back("Keyword: " + token);
                } else {
                    tokens.push_back("Identifier: " + token);
                }
                continue;
            }

            // Identify integers
            if (isdigit(c)) {
                string number;
                number += c;
                while (isdigit(c = getNextChar())) {
                    number += c;
                }
                unreadChar();
                tokens.push_back("Integer: " + number);
                continue;
            }

            // Identify operators and symbols
            switch (c) {
                case '+': case '-': case '=': case '<': case '>': case ';': case '(': case ')': case '{': case '}':
                    tokens.push_back("Symbol: " + string(1, c));
                    break;
                default:
                    cout << "Unknown error: " << c << endl;
            }
        }
        return tokens;
    }
};

int main() {
    ifstream file("input.txt"); // Input file containing the code
    if (!file) {
        cout << "Error opening file!" << endl;
        return 1;
    }

    string code((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    Lexer lexer(code);
    
    vector<string> tokens = lexer.tokenize();
    
    for (const auto& token : tokens) {
        cout << token << endl;
    }

    return 0;
}
