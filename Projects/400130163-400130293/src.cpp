#include <iostream>
#include <vector>
#include <cctype>
#include <string>
#include <map>
#include <regex>

using namespace std;

enum TokenType {
    KEYWORD, IDENTIFIER, INTEGER, OPERATOR, SEPARATOR, UNKNOWN
};

struct Token {
    TokenType type;
    string value;
    int line;
};

vector<string> keywords = { "Program", "Var", "Start", "End", "Print", "Read", "If", "Iteration", "Put" };
vector<char> operators = { '<', '>', '=' };
vector<char> separators = { '(', ')', '{', '}', ';' };

bool isKeyword(const string& word) {
    return find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool isIdentifier(const string& word) {
    return regex_match(word, regex("[a-zA-Z][a-zA-Z0-9]{0,4}"));
}

bool isInteger(const string& word) {
    return regex_match(word, regex("[0-9]+"));
}

vector<Token> lexicalAnalyzer(const string& code) {
    vector<Token> tokens;
    string buffer = "";
    int line = 1;

    for (size_t i = 0; i < code.length(); i++) {
        char ch = code[i];

        if (ch == '\n') {
            if (!buffer.empty()) {
                if (isKeyword(buffer))
                    tokens.push_back({ KEYWORD, buffer, line });
                else if (isInteger(buffer))
                    tokens.push_back({ INTEGER, buffer, line });
                else if (isIdentifier(buffer))
                    tokens.push_back({ IDENTIFIER, buffer, line });
                else
                    tokens.push_back({ UNKNOWN, buffer, line });
                buffer.clear();
            }
            line++;
        }
        else if (isspace(ch)) {
            if (!buffer.empty()) {
                if (isKeyword(buffer))
                    tokens.push_back({ KEYWORD, buffer, line });
                else if (isInteger(buffer))
                    tokens.push_back({ INTEGER, buffer, line });
                else if (isIdentifier(buffer))
                    tokens.push_back({ IDENTIFIER, buffer, line });
                else
                    tokens.push_back({ UNKNOWN, buffer, line });
                buffer.clear();
            }
        }
        else if (find(operators.begin(), operators.end(), ch) != operators.end()) {
            if (!buffer.empty()) {
                if (isKeyword(buffer))
                    tokens.push_back({ KEYWORD, buffer, line });
                else if (isInteger(buffer))
                    tokens.push_back({ INTEGER, buffer, line });
                else if (isIdentifier(buffer))
                    tokens.push_back({ IDENTIFIER, buffer, line });
                else
                    tokens.push_back({ UNKNOWN, buffer, line });
                buffer.clear();
            }
            tokens.push_back({ OPERATOR, string(1, ch), line });
        }
        else if (find(separators.begin(), separators.end(), ch) != separators.end()) {
            if (!buffer.empty()) {
                if (isKeyword(buffer))
                    tokens.push_back({ KEYWORD, buffer, line });
                else if (isInteger(buffer))
                    tokens.push_back({ INTEGER, buffer, line });
                else if (isIdentifier(buffer))
                    tokens.push_back({ IDENTIFIER, buffer, line });
                else
                    tokens.push_back({ UNKNOWN, buffer, line });
                buffer.clear();
            }
            tokens.push_back({ SEPARATOR, string(1, ch), line });
        }
        else {
            buffer += ch;
        }
    }

    if (!buffer.empty()) {
        if (isKeyword(buffer))
            tokens.push_back({ KEYWORD, buffer, line });
        else if (isInteger(buffer))
            tokens.push_back({ INTEGER, buffer, line });
        else if (isIdentifier(buffer))
            tokens.push_back({ IDENTIFIER, buffer, line });
        else
            tokens.push_back({ UNKNOWN, buffer, line });
    }

    return tokens;
}

void printTokens(const vector<Token>& tokens) {
    for (const auto& token : tokens) {
        cout << "Line " << token.line << ": " << token.value << " (";
        switch (token.type) {
        case KEYWORD: cout << "Keyword"; break;
        case IDENTIFIER: cout << "Identifier"; break;
        case INTEGER: cout << "Integer"; break;
        case OPERATOR: cout << "Operator"; break;
        case SEPARATOR: cout << "Separator"; break;
        default: cout << "Unknown"; break;
        }
        cout << ")\n";
    }
}

int main() {
    string code = "Program Var x;\nStart Print(x);\nEnd";
    vector<Token> tokens = lexicalAnalyzer(code);
    printTokens(tokens);
    return 0;
}
