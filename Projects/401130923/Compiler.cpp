#include <iostream>
#include <fstream>
#include <vector>
#include <cctype>
#include <string>
#include <set>

using namespace std;

// Token types
enum TokenType {
    KEYWORD, IDENTIFIER, INTEGER, OPERATOR, SYMBOL, ERROR
};

// Token structure
struct Token {
    TokenType type;
    string value;
    int line;
};

// Keywords set
set<string> keywords;

// Functions to initialize keywords
void initializeKeywords() {
    keywords.insert("Program");
    keywords.insert("Var");
    keywords.insert("Start");
    keywords.insert("End");
    keywords.insert("Print");
    keywords.insert("Read");
    keywords.insert("If");
    keywords.insert("Iteration");
    keywords.insert("Put");
}

// Function to tokenize the input
vector<Token> tokenize(const string& filename) {
    ifstream file(filename.c_str()); // Fix: Use c_str() to convert string to const char*
    vector<Token> tokens;
    string word;
    char ch;
    int line = 1;

    if (!file.is_open()) {
        cout << "Error opening file!" << endl;
        return tokens;
    }

    while (file.get(ch)) {
        if (ch == '\n') {
            line++;
        } else if (isspace(ch)) {
            continue;
        } else if (isalpha(ch)) {
            word = ch;
            while (file.get(ch) && (isalnum(ch))) {
                word += ch;
            }
            file.unget();
            if (keywords.find(word) != keywords.end()) {
                Token t; t.type = KEYWORD; t.value = word; t.line = line; tokens.push_back(t);
            } else {
                Token t; t.type = IDENTIFIER; t.value = word; t.line = line; tokens.push_back(t);
            }
        } else if (isdigit(ch)) {
            word = ch;
            while (file.get(ch) && isdigit(ch)) {
                word += ch;
            }
            file.unget();
            Token t; t.type = INTEGER; t.value = word; t.line = line; tokens.push_back(t);
        } else if (ch == '<' || ch == '>' || ch == '=' || ch == '+') {
            Token t; t.type = OPERATOR; t.value = string(1, ch); t.line = line; tokens.push_back(t);
        } else if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ';' || ch == ',') {
            Token t; t.type = SYMBOL; t.value = string(1, ch); t.line = line; tokens.push_back(t);
        } else {
            cout << "Error: invalid character at line " << line << endl;
        }
    }
    file.close();
    return tokens;
}

// Simple parser for the provided grammar
bool parseProgram(vector<Token>& tokens, size_t& index);
bool parseBlock(vector<Token>& tokens, size_t& index);
bool parseStates(vector<Token>& tokens, size_t& index);

bool parseProgram(vector<Token>& tokens, size_t& index) {
    if (tokens[index].value == "Program") {
        index++;
        if (tokens[index].value == "Var") {
            index++;
            if (tokens[index].type == IDENTIFIER) {
                index++;
                if (tokens[index].value == ";") {
                    index++;
                    return parseBlock(tokens, index);
                }
            }
        }
    }
    return false;
}

bool parseBlock(vector<Token>& tokens, size_t& index) {
    if (tokens[index].value == "Start") {
        index++;
        return parseStates(tokens, index);
    }
    return false;
}

bool parseStates(vector<Token>& tokens, size_t& index) {
    while (index < tokens.size()) {
        if (tokens[index].value == "End") {
            index++;
            return true;
        } else if (tokens[index].value == "Print") {
            index++;
            if (tokens[index].value == "(") {
                index++;
                if (tokens[index].type == IDENTIFIER || tokens[index].type == INTEGER) {
                    index++;
                    if (tokens[index].value == ")") {
                        index++;
                        if (tokens[index].value == ";") {
                            index++;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            }
        } else if (tokens[index].value == "Read") {
            index++;
            if (tokens[index].value == "(") {
                index++;
                if (tokens[index].type == IDENTIFIER) {
                    index++;
                    if (tokens[index].value == ")") {
                        index++;
                        if (tokens[index].value == ";") {
                            index++;
                        } else {
                            return false;
                        }
                    } else {
                        return false;
                    }
                }
            }
        } else if (tokens[index].value == "If" || tokens[index].value == "Iteration") {
            index++;
            if (tokens[index].value == "(") {
                index++;
                if (tokens[index].type == IDENTIFIER || tokens[index].type == INTEGER) {
                    index++;
                    if (tokens[index].value == "<" || tokens[index].value == ">" || tokens[index].value == "==") {
                        index++;
                        if (tokens[index].type == IDENTIFIER || tokens[index].type == INTEGER) {
                            index++;
                            if (tokens[index].value == ")") {
                                index++;
                                if (tokens[index].value == "{") {
                                    index++;
                                    parseStates(tokens, index);
                                    if (tokens[index].value == "}") {
                                        index++;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else if (tokens[index].value == "Put") {
            index++;
            if (tokens[index].type == IDENTIFIER) {
                index++;
                if (tokens[index].value == "=") {
                    index++;
                    if (tokens[index].type == IDENTIFIER || tokens[index].type == INTEGER) {
                        index++;
                        if (tokens[index].value == ";") {
                            index++;
                        }
                    }
                }
            }
        } else {
            return false;
        }
    }
    return false;
}

// Main function
int main() {
    initializeKeywords();
    string filename = "input.txt";
    vector<Token> tokens = tokenize(filename);

    size_t index = 0;
    if (parseProgram(tokens, index)) {
        cout << "Program parsed successfully!" << endl;
    } else {
        cout << "Error parsing program!" << endl;
    }

    return 0;
}


