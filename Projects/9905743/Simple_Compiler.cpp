#include <iostream>
#include <fstream>
#include <regex>
#include <vector>
#include <string>
#include <map>
#include <stack>
#include <memory>
#include <stdexcept>
using namespace std;

// Define token types
enum TokenType {
    KEYWORD, IDENTIFIER, INTEGER, SYMBOL, END_OF_FILE
};

// Token structure
struct Token {
    TokenType type;
    string value;
    int line;
    int column;
};

// AST Node structure
struct ASTNode {
    string type;
    string value;
    vector<shared_ptr<ASTNode>> children;

    ASTNode(string type, string value = "") : type(type), value(value) {}

    void addChild(shared_ptr<ASTNode> child) {
        children.push_back(child);
    }

    void print(int depth = 0) {
        for (int i = 0; i < depth; ++i) cout << "  ";
        cout << type << ": " << value << endl;
        for (auto &child : children) {
            child->print(depth + 1);
        }
    }
};

// Lexer: Lexical Analyzer
class Lexer {
public:
    explicit Lexer(const string &filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filename);
        }
        string line;
        while (getline(file, line)) {
            lines.push_back(line);
        }
        file.close();
    }

    vector<Token> tokenize() {
        vector<Token> tokens;
        regex identifier_regex("[a-zA-Z_][a-zA-Z0-9_]*");
        regex integer_regex("\\d+");
        regex keyword_regex("\\b(Program|Var|Start|End|Print|Read|If|Iteration|Put)\\b");
        regex symbol_regex("[;{}()=<>+\-]");

        for (size_t line_num = 0; line_num < lines.size(); ++line_num) {
            size_t col = 0;
            string &line = lines[line_num];

            while (col < line.size()) {
                char c = line[col];

                // Skip whitespace
                if (isspace(c)) {
                    ++col;
                    continue;
                }

                // Match keywords
                smatch match;
                string remaining = line.substr(col);
                if (regex_search(remaining, match, keyword_regex) && match.position() == 0) {
                    tokens.push_back({KEYWORD, match.str(), static_cast<int>(line_num + 1), static_cast<int>(col + 1)});
                    col += match.length();
                } else if (regex_search(remaining, match, identifier_regex) && match.position() == 0) {
                    string identifier = match.str();
                    if (identifier.length() > 5) {
                        cerr << "Error: Identifier '" << identifier << "' exceeds maximum length of 5 at line "
                             << line_num + 1 << ", column " << col + 1 << endl;
                        exit(1);
                    }
                    tokens.push_back({IDENTIFIER, identifier, static_cast<int>(line_num + 1), static_cast<int>(col + 1)});
                    col += match.length();
                } else if (regex_search(remaining, match, integer_regex) && match.position() == 0) {
                    tokens.push_back({INTEGER, match.str(), static_cast<int>(line_num + 1), static_cast<int>(col + 1)});
                    col += match.length();
                } else if (regex_search(remaining, match, symbol_regex) && match.position() == 0) {
                    tokens.push_back({SYMBOL, match.str(), static_cast<int>(line_num + 1), static_cast<int>(col + 1)});
                    col += match.length();
                } else {
                    cerr << "Error: Unrecognized token at line " << line_num + 1 << ", column " << col + 1 << endl;
                    exit(1);
                }
            }
        }

        tokens.push_back({END_OF_FILE, "EOF", static_cast<int>(lines.size()), 0});
        return tokens;
    }

private:
    vector<string> lines;
};

// Parser: Syntactic Analyzer
class Parser {
public:
    explicit Parser(const vector<Token> &tokens) : tokens(tokens), index(0) {}

    shared_ptr<ASTNode> parse() {
        return parseProgram();
    }

private:
    vector<Token> tokens;
    size_t index;

    shared_ptr<ASTNode> parseProgram() {
        auto node = make_shared<ASTNode>("Program");
        expect(KEYWORD, "Program");
        node->addChild(parseVars());
        node->addChild(parseBlocks());
        expect(KEYWORD, "end");
        return node;
    }

    shared_ptr<ASTNode> parseVars() {
        auto node = make_shared<ASTNode>("Vars");
        while (match(KEYWORD, "Var")) {
            auto varNode = make_shared<ASTNode>("Var");
            varNode->addChild(make_shared<ASTNode>("Identifier", expect(IDENTIFIER).value));
            expect(SYMBOL, ";");
            node->addChild(varNode);
        }
        return node;
    }

    shared_ptr<ASTNode> parseBlocks() {
        auto node = make_shared<ASTNode>("Blocks");
        expect(KEYWORD, "Start");
        node->addChild(parseStates());
        expect(KEYWORD, "End");
        return node;
    }

    shared_ptr<ASTNode> parseStates() {
        auto node = make_shared<ASTNode>("States");
        do {
            node->addChild(parseState());
        } while (lookAhead(KEYWORD, "If") || lookAhead(KEYWORD, "Iteration") || lookAhead(KEYWORD, "Print") || lookAhead(KEYWORD, "Read") || lookAhead(KEYWORD, "Put"));
        return node;
    }

    shared_ptr<ASTNode> parseState() {
        if (lookAhead(KEYWORD, "If")) {
            return parseIf();
        } else if (lookAhead(KEYWORD, "Iteration")) {
            return parseLoop();
        } else if (lookAhead(KEYWORD, "Print")) {
            return parseOut();
        } else if (lookAhead(KEYWORD, "Read")) {
            return parseIn();
        } else if (lookAhead(KEYWORD, "Put")) {
            return parseAssign();
        } else {
            cerr << "Error: Unexpected token " << currentToken().value << " at line " << currentToken().line << endl;
            exit(1);
        }
    }

    shared_ptr<ASTNode> parseIf() {
        auto node = make_shared<ASTNode>("If");
        expect(KEYWORD, "If");
        expect(SYMBOL, "(");
        node->addChild(parseExpr());
        node->addChild(parseOperator());
        node->addChild(parseExpr());
        expect(SYMBOL, ")");
        expect(SYMBOL, "{");
        node->addChild(parseStates());
        expect(SYMBOL, "}");
        return node;
    }

    shared_ptr<ASTNode> parseLoop() {
        auto node = make_shared<ASTNode>("Loop");
        expect(KEYWORD, "Iteration");
        expect(SYMBOL, "(");
        node->addChild(parseExpr());
        node->addChild(parseOperator());
        node->addChild(parseExpr());
        expect(SYMBOL, ")");
        expect(SYMBOL, "{");
        node->addChild(parseStates());
        expect(SYMBOL, "}");
        return node;
    }

    shared_ptr<ASTNode> parseOut() {
        auto node = make_shared<ASTNode>("Print");
        expect(KEYWORD, "Print");
        expect(SYMBOL, "(");
        node->addChild(parseExpr());
        expect(SYMBOL, ")");
        expect(SYMBOL, ";");
        return node;
    }

    shared_ptr<ASTNode> parseIn() {
        auto node = make_shared<ASTNode>("Read");
        expect(KEYWORD, "Read");
        expect(SYMBOL, "(");
        node->addChild(make_shared<ASTNode>("Identifier", expect(IDENTIFIER).value));
        expect(SYMBOL, ")");
        expect(SYMBOL, ";");
        return node;
    }

    shared_ptr<ASTNode> parseAssign() {
        auto node = make_shared<ASTNode>("Assign");
        expect(KEYWORD, "Put");
        node->addChild(make_shared<ASTNode>("Identifier", expect(IDENTIFIER).value));
        expect(SYMBOL, "=");
        node->addChild(parseExpr());
        expect(SYMBOL, ";");
        return node;
    }

    shared_ptr<ASTNode> parseExpr() {
        auto node = make_shared<ASTNode>("Expr");
        node->addChild(parseR());
        while (lookAhead(SYMBOL, "+") || lookAhead(SYMBOL, "-")) {
            node->addChild(make_shared<ASTNode>("Operator", advance().value));
            node->addChild(parseR());
        }
        return node;
    }

    shared_ptr<ASTNode> parseR() {
        if (lookAhead(IDENTIFIER)) {
            return make_shared<ASTNode>("Identifier", advance().value);
        } else if (lookAhead(INTEGER)) {
            return make_shared<ASTNode>("Integer", advance().value);
        } else {
            cerr << "Error: Expected Identifier or Integer at line " << currentToken().line << endl;
            exit(1);
        }
    }

    shared_ptr<ASTNode> parseOperator() {
                if (lookAhead(SYMBOL, "<") || lookAhead(SYMBOL, ">") || lookAhead(SYMBOL, "==")) {
            return make_shared<ASTNode>("Operator", advance().value);
        } else {
            cerr << "Error: Expected operator (<, >, ==) at line " << currentToken().line << endl;
            exit(1);
        }
    }

    Token expect(TokenType type, const string &value = "") {
        if (currentToken().type == type && (value.empty() || currentToken().value == value)) {
            return advance();
        } else {
            cerr << "Error: Expected " << value << " at line " << currentToken().line << ", column " << currentToken().column << endl;
            exit(1);
        }
    }

    Token advance() {
        if (index < tokens.size()) {
            return tokens[index++];
        } else {
            return tokens.back();
        }
    }

    bool lookAhead(TokenType type, const string &value = "") {
        if (index < tokens.size()) {
            return tokens[index].type == type && (value.empty() || tokens[index].value == value);
        }
        return false;
    }

    Token currentToken() {
        if (index < tokens.size()) {
            return tokens[index];
        } else {
            return tokens.back();
        }
    }
};

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <source_file>" << endl;
        return 1;
    }

    try {
        // Lexer phase
        Lexer lexer(argv[1]);
        vector<Token> tokens = lexer.tokenize();

        // Print tokens
        cout << "Tokens:" << endl;
        for (const auto &token : tokens) {
            cout << token.value << " (Line: " << token.line << ", Column: " << token.column << ")" << endl;
        }

        // Parser phase
        Parser parser(tokens);
        shared_ptr<ASTNode> ast = parser.parse();

        // Print AST
        cout << "\nAbstract Syntax Tree (AST):" << endl;
        ast->print();
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
