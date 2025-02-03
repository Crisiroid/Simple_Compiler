#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <regex>

using namespace std;

struct Token {
    string type;
    string value;
    int line;
    int column;
};

class LexicalAnalyzer {
private:
    vector<Token> tokens;
    string delimiters = " \\t\\n\\r,;(){}=+-<>";

    bool isIdentifier(const string &word) {
        return regex_match(word, regex("^[a-zA-Z]{1,5}$"));
    }

    bool isInteger(const string &word) {
        return regex_match(word, regex("^[0-9]+$"));
    }

    string getTokenType(const string &word) {
        if (isIdentifier(word)) return "Identifier";
        if (isInteger(word)) return "Integer";
        if (word == "If" || word == "Iteration" || word == "Put" || word == "Read" || word == "Print" || word == "Var") return "Keyword";
        if (word == "+" || word == "-" || word == "=" || word == ">" || word == "<" || word == "==") return "Operator";
        return "Unknown";
    }

public:
    void analyze(const string &code) {
        tokens.clear();
        string token;
        int line = 1, column = 0;
        for (size_t i = 0; i < code.size(); ++i) {
            char c = code[i];
            column++;
            if (delimiters.find(c) != string::npos) {
                if (!token.empty()) {
                    string type = getTokenType(token);
                    tokens.push_back({type, token, line, column - static_cast<int>(token.size())});
                    token.clear();
                }
                if (c != ' ' && c != '\\t' && c != '\\n' && c != '\\r') {
                    string symbol(1, c);
                    tokens.push_back({"Symbol", symbol, line, column});
                }
                if (c == '\\n') {
                    line++;
                    column = 0;
                }
            } else {
                token += c;
            }
        }
        if (!token.empty()) {
            string type = getTokenType(token);
            tokens.push_back({type, token, line, column - static_cast<int>(token.size())});
        }
    }

    const vector<Token> &getTokens() const {
        return tokens;
    }
};

struct ASTNode {
    string value;
    vector<shared_ptr<ASTNode>> children;
    ASTNode(const string &val) : value(val) {}
};

class Parser {
private:
    vector<Token> tokens;
    int currentIndex;
    Token currentToken;

    void advance() {
        if (currentIndex < tokens.size()) {
            currentToken = tokens[currentIndex++];
        }
    }

    bool match(const string &type) {
        return currentToken.type == type;
    }

    void error(const string &message) {
        cerr << "Syntax Error: " << message << " at line " << currentToken.line << ", column " << currentToken.column << endl;
        exit(1);
    }



    shared_ptr<ASTNode> parseProgram() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Program");
        if (match("Keyword") && currentToken.value == "Program") {
            advance();
            node->children.push_back(parseVars());
            node->children.push_back(parseBlocks());
            if (match("Keyword") && currentToken.value == "end") {
                advance();
            } else {
                error("Expected 'end' after Program block.");
            }
        } else {
            error("Expected 'Program' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseVars() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Vars");
        if (match("Keyword") && currentToken.value == "Var") {
            advance();
            if (match("Identifier")) {
                node->children.push_back(make_shared<ASTNode>(currentToken.value));
                advance();
                if (match("Symbol") && currentToken.value == ";") {
                    advance();
                    node->children.push_back(parseVars());
                } else {
                    error("Expected ';' after variable declaration.");
                }
            } else {
                error("Expected Identifier after 'Var'.");
            }
        }
        return node;
    }

    shared_ptr<ASTNode> parseBlocks() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Blocks");
        if (match("Keyword") && currentToken.value == "Start") {
            advance();
            node->children.push_back(parseStates());
            if (match("Keyword") && currentToken.value == "End") {
                advance();
            } else {
                error("Expected 'End' after block.");
            }
        } else {
            error("Expected 'Start' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseStates() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("States");
        node->children.push_back(parseState());
        node->children.push_back(parseStatesPrime());
        return node;
    }


    shared_ptr<ASTNode> parseStatesPrime() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("States'");
        if (match("Keyword") || match("Identifier")) {
            node->children.push_back(parseStates());
        }
        return node;
    }



    shared_ptr<ASTNode> parseState() {
        if (match("Keyword") && (currentToken.value == "If" || currentToken.value == "Iteration" ||
            currentToken.value == "Put" || currentToken.value == "Read" || currentToken.value == "Print")) {
            if (currentToken.value == "If") {
                return parseIf();
            } else if (currentToken.value == "Iteration") {
                return parseLoop();
            } else if (currentToken.value == "Put") {
                return parseAssign();
            } else if (currentToken.value == "Read") {
                return parseIn();
            } else if (currentToken.value == "Print") {
                return parseOut();
            }
        }
        return nullptr;
    }

    shared_ptr<ASTNode> parseMStates() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("MStates");
        if (match("Keyword") && (currentToken.value == "If" || currentToken.value == "Iteration" ||
            currentToken.value == "Put" || currentToken.value == "Read" || currentToken.value == "Print")) {
            node->children.push_back(parseStates());
        }
        return node;
    }

    shared_ptr<ASTNode> parseOut() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Out");
        if (match("Keyword") && currentToken.value == "Print") {
            advance();
            if (match("Symbol") && currentToken.value == "(") {
                advance();
                node->children.push_back(parseExpr());
                if (match("Symbol") && currentToken.value == ")") {
                    advance();
                    if (match("Symbol") && currentToken.value == ";") {
                        advance();
                    } else {
                        error("Expected ';' after Print statement.");
                    }
                } else {
                    error("Expected ')' after expression in Print.");
                }
            } else {
                error("Expected '(' after Print.");
            }
        } else {
            error("Expected 'Print' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseIn() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("In");
        if (match("Keyword") && currentToken.value == "Read") {
            advance();
            if (match("Symbol") && currentToken.value == "(") {
                advance();
                if (match("Identifier")) {
                    node->children.push_back(make_shared<ASTNode>(currentToken.value));
                    advance();
                    if (match("Symbol") && currentToken.value == ")") {
                        advance();
                        if (match("Symbol") && currentToken.value == ";") {
                            advance();
                        } else {
                            error("Expected ';' after Read statement.");
                        }
                    } else {
                        error("Expected ')' after Identifier in Read.");
                    }
                } else {
                    error("Expected Identifier in Read.");
                }
            } else {
                error("Expected '(' after Read.");
            }
        } else {
            error("Expected 'Read' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseIf() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("If");
        if (match("Keyword") && currentToken.value == "If") {
            advance();
            if (match("Symbol") && currentToken.value == "(") {
                advance();
                node->children.push_back(parseExpr());
                if (match("Operator")) {
                    advance();
                    node->children.push_back(parseExpr());
                    if (match("Symbol") && currentToken.value == ")") {
                        advance();
                        if (match("Symbol") && currentToken.value == "{") {
                            advance();
                            node->children.push_back(parseState());
                            if (match("Symbol") && currentToken.value == "}") {
                                advance();
                            } else {
                                error("Expected '}' after If body.");
                            }
                        } else {
                            error("Expected '{' after If condition.");
                        }
                    } else {
                        error("Expected ')' after condition in If.");
                    }
                } else {
                    error("Expected operator in If condition.");
                }
            } else {
                error("Expected '(' after If.");
            }
        } else {
            error("Expected 'If' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseLoop() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Loop");
        if (match("Keyword") && currentToken.value == "Iteration") {
            advance();
            if (match("Symbol") && currentToken.value == "(") {
                advance();
                node->children.push_back(parseExpr());
                if (match("Operator")) {
                    advance();
                    node->children.push_back(parseExpr());
                    if (match("Symbol") && currentToken.value == ")") {
                        advance();
                        if (match("Symbol") && currentToken.value == "{") {
                            advance();
                            node->children.push_back(parseState());
                            if (match("Symbol") && currentToken.value == "}") {
                                advance();
                            } else {
                                error("Expected '}' after Loop body.");
                            }
                        } else {
                            error("Expected '{' after Loop condition.");
                        }
                    } else {
                        error("Expected ')' after condition in Loop.");
                    }
                } else {
                    error("Expected operator in Loop condition.");
                }
            } else {
                error("Expected '(' after Iteration.");
            }
        } else {
            error("Expected 'Iteration' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseAssign() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Assign");
        if (match("Keyword") && currentToken.value == "Put") {
            advance();
            if (match("Identifier")) {
                node->children.push_back(make_shared<ASTNode>(currentToken.value));
                advance();
                if (match("Operator") && currentToken.value == "=") {
                    advance();
                    node->children.push_back(parseExpr());
                    if (match("Symbol") && currentToken.value == ";") {
                        advance();
                    } else {
                        error("Expected ';' after Assignment.");
                    }
                } else {
                    error("Expected '=' in Assignment.");
                }
            } else {
                error("Expected Identifier after 'Put'.");
            }
        } else {
            error("Expected 'Put' keyword.");
        }
        return node;
    }

    shared_ptr<ASTNode> parseExpr() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Expr");
        node->children.push_back(parseR());
        node->children.push_back(parseExprPrime());
        return node;
    }



    shared_ptr<ASTNode> parseExprPrime() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("Expr'");
        if (match("Operator") && (currentToken.value == "+" || currentToken.value == "-")) {
            node->children.push_back(make_shared<ASTNode>(currentToken.value));
            advance();
            node->children.push_back(parseR());
            node->children.push_back(parseExprPrime());
        }
        return node;
    }



    shared_ptr<ASTNode> parseR() {
        shared_ptr<ASTNode> node = make_shared<ASTNode>("R");
        if (match("Identifier")) {
            node->children.push_back(make_shared<ASTNode>(currentToken.value));
            advance();
        } else if (match("Integer")) {
            node->children.push_back(make_shared<ASTNode>(currentToken.value));
            advance();
        } else {
            error("Expected Identifier or Integer.");
        }
        return node;
    }

public:
    Parser(const vector<Token> &tokens) : tokens(tokens), currentIndex(0) {
        advance();
    }

    shared_ptr<ASTNode> parse() {
        return parseProgram();
    }
};

int main() {
    string code;
    cout << "Enter the file path: ";
    string filePath;
    cin >> filePath;

    ifstream inputFile(filePath);
    if (!inputFile) {
        cerr << "Error: Unable to open file." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        code += line + '\n';
    }
    inputFile.close();

    LexicalAnalyzer lexicalAnalyzer;
    lexicalAnalyzer.analyze(code);
    const vector<Token>& tokens = lexicalAnalyzer.getTokens();

    cout << "Tokens:" << endl;
    for (const Token& token : tokens) {
        cout << "Type: " << token.type << ", Value: " << token.value << ", Line: " << token.line << ", Column: " << token.column << endl;
    }

    Parser parser(tokens);
    shared_ptr<ASTNode> ast = parser.parse();

    cout << "Parsing completed successfully!" << endl;

    return 0;
}
