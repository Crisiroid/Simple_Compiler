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

struct SymbolTable {
    struct VariableInfo {
        int value;
        bool assigned;
    };

    map<string, VariableInfo> variables;

    void declareVariable(const string& name) {
        if (variables.find(name) != variables.end()) {
            cout << "Error: Variable '" << name << "' is already declared!\n";
        }
        else {
            variables[name] = { 0, false };
        }
    }

    void assignVariable(const string& name, int value) {
        if (variables.find(name) == variables.end()) {
            cout << "Error: Variable '" << name << "' is not declared!\n";
        }
        else {
            variables[name] = { value, true };
        }
    }

    int getValue(const string& name) {
        if (variables.find(name) == variables.end()) {
            cout << "Error: Variable '" << name << "' is not declared!\n";
            return 0;
        }
        if (!variables[name].assigned) {
            cout << "Error: Variable '" << name << "' is used before assignment!\n";
            return 0;
        }
        return variables[name].value;
    }
};

void parseTokens(const vector<Token>& tokens, SymbolTable& symTable) {
    for (size_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == KEYWORD) {
            if (tokens[i].value == "Var") {
                if (i + 1 < tokens.size() && tokens[i + 1].type == IDENTIFIER) {
                    symTable.declareVariable(tokens[i + 1].value);
                    i++;
                }
                else {
                    cout << "Syntax Error: Expected identifier after 'Var' at line " << tokens[i].line << "\n";
                }
            }
            else if (tokens[i].value == "Print") {
                if (i + 2 < tokens.size() && tokens[i + 1].type == SEPARATOR && tokens[i + 1].value == "(" &&
                    tokens[i + 2].type == IDENTIFIER) {
                    cout << tokens[i + 2].value << " = " << symTable.getValue(tokens[i + 2].value) << "\n";
                    i += 3;
                }
                else {
                    cout << "Syntax Error: Incorrect Print statement at line " << tokens[i].line << "\n";
                }
            }
        }
    }
}

int main() {
    string code = "Program Var x;\nStart Print 23;\nEnd";

    vector<Token> tokens = lexicalAnalyzer(code);

    printTokens(tokens);

    SymbolTable symTable;
    parseTokens(tokens, symTable);

    return 0;
}
