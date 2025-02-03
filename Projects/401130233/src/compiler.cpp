#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <unordered_set>
#include <unordered_map>
#include <memory>

// --------------------------------------------------------------------------
// Token Types and Token Structure
// --------------------------------------------------------------------------

enum class TokenType {
    // Keywords
    KW_IF, KW_PRINT, KW_READ, KW_ITERATION, KW_PUT, KW_VAR, KW_START, KW_END, KW_PROGRAM,
    // Operators
    OP_PLUS, OP_MINUS, OP_LT, OP_GT, OP_EQEQ, OP_ASSIGN,
    // Delimiters
    DELIM_LBRACE, DELIM_RBRACE, DELIM_LPAREN, DELIM_RPAREN, DELIM_SEMICOLON,
    // Literals
    IDENTIFIER, INTEGER,
    // Special
    END_OF_FILE,
    // Error token
    ERROR
};

std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case TokenType::KW_IF: return "KW_IF";
        case TokenType::KW_PRINT: return "KW_PRINT";
        case TokenType::KW_READ: return "KW_READ";
        case TokenType::KW_ITERATION: return "KW_ITERATION";
        case TokenType::KW_PUT: return "KW_PUT";
        case TokenType::KW_VAR: return "KW_VAR";
        case TokenType::KW_START: return "KW_START";
        case TokenType::KW_END: return "KW_END";
        case TokenType::KW_PROGRAM: return "KW_PROGRAM";
        case TokenType::OP_PLUS: return "OP_PLUS";
        case TokenType::OP_MINUS: return "OP_MINUS";
        case TokenType::OP_LT: return "OP_LT";
        case TokenType::OP_GT: return "OP_GT";
        case TokenType::OP_EQEQ: return "OP_EQEQ";
        case TokenType::OP_ASSIGN: return "OP_ASSIGN";
        case TokenType::DELIM_LBRACE: return "DELIM_LBRACE";
        case TokenType::DELIM_RBRACE: return "DELIM_RBRACE";
        case TokenType::DELIM_LPAREN: return "DELIM_LPAREN";
        case TokenType::DELIM_RPAREN: return "DELIM_RPAREN";
        case TokenType::DELIM_SEMICOLON: return "DELIM_SEMICOLON";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::END_OF_FILE: return "EOF";
        default: return "ERROR";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType type, const std::string& lexeme, int line, int column)
        : type(type), lexeme(lexeme), line(line), column(column) {}
    
    void print() const {
        std::cout << "Token(" << tokenTypeToString(type) << ", \"" 
                  << lexeme << "\", line: " << line << ", col: " << column << ")\n";
    }
};

// --------------------------------------------------------------------------
// Lexical Analyzer (Phase 1)
// --------------------------------------------------------------------------

class Lexer {
public:
    Lexer(const std::string& filename) : filename(filename), line(1), column(0) {
        loadFile();
        keywords = {
            {"If", TokenType::KW_IF},
            {"Print", TokenType::KW_PRINT},
            {"Read", TokenType::KW_READ},
            {"Iteration", TokenType::KW_ITERATION},
            {"Put", TokenType::KW_PUT},
            {"Var", TokenType::KW_VAR},
            {"Start", TokenType::KW_START},
            {"End", TokenType::KW_END},
            {"Program", TokenType::KW_PROGRAM}
        };
    }
    
    std::vector<Token> tokenize() {
        std::vector<Token> tokens;
        while (currentIndex < source.size()) {
            char currentChar = peek();
            if (std::isspace(currentChar)) {
                consumeWhitespace();
            } else if (std::isalpha(currentChar)) {
                tokens.push_back(consumeIdentifierOrKeyword());
            } else if (std::isdigit(currentChar)) {
                tokens.push_back(consumeInteger());
            } else {
                tokens.push_back(consumeOperatorOrDelimiter());
            }
        }
        tokens.emplace_back(TokenType::END_OF_FILE, "EOF", line, column);
        return tokens;
    }
    
private:
    std::string filename;
    std::string source;
    size_t currentIndex = 0;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;
    
    void loadFile() {
        std::ifstream inFile(filename);
        if (!inFile) {
            std::cerr << "Error: Could not open file: " << filename << "\n";
            exit(EXIT_FAILURE);
        }
        std::stringstream buffer;
        buffer << inFile.rdbuf();
        source = buffer.str();
    }
    
    char peek() const {
        return currentIndex < source.size() ? source[currentIndex] : '\0';
    }
    
    char advance() {
        char c = source[currentIndex++];
        if (c == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }
        return c;
    }
    
    void consumeWhitespace() {
        while (currentIndex < source.size() && std::isspace(peek()))
            advance();
    }
    
    Token consumeIdentifierOrKeyword() {
        int tokenLine = line, tokenColumn = column;
        std::string lexeme;
        while (currentIndex < source.size() && (std::isalnum(peek()) || peek() == '_')) {
            lexeme.push_back(advance());
        }
        if (keywords.find(lexeme) != keywords.end()) {
            return Token(keywords[lexeme], lexeme, tokenLine, tokenColumn);
        }
        std::string id = lexeme.substr(0, 5);
        if (lexeme.size() > 5) {
            std::cerr << "Lexical Warning at line " << tokenLine << ", col " << tokenColumn 
                      << ": Identifier '" << lexeme 
                      << "' truncated to '" << id << "'\n";
        }
        return Token(TokenType::IDENTIFIER, id, tokenLine, tokenColumn);
    }
    
    Token consumeInteger() {
        int tokenLine = line, tokenColumn = column;
        std::string lexeme;
        while (currentIndex < source.size() && std::isdigit(peek())) {
            lexeme.push_back(advance());
        }
        return Token(TokenType::INTEGER, lexeme, tokenLine, tokenColumn);
    }
    
    Token consumeOperatorOrDelimiter() {
        int tokenLine = line, tokenColumn = column;
        char c = advance();
        switch(c) {
            case '+': return Token(TokenType::OP_PLUS, "+", tokenLine, tokenColumn);
            case '-': return Token(TokenType::OP_MINUS, "-", tokenLine, tokenColumn);
            case '<': return Token(TokenType::OP_LT, "<", tokenLine, tokenColumn);
            case '>': return Token(TokenType::OP_GT, ">", tokenLine, tokenColumn);
            case '=':
                if (peek() == '=') {
                    advance();
                    return Token(TokenType::OP_EQEQ, "==", tokenLine, tokenColumn);
                } else {
                    return Token(TokenType::OP_ASSIGN, "=", tokenLine, tokenColumn);
                }
            case '{': return Token(TokenType::DELIM_LBRACE, "{", tokenLine, tokenColumn);
            case '}': return Token(TokenType::DELIM_RBRACE, "}", tokenLine, tokenColumn);
            case '(': return Token(TokenType::DELIM_LPAREN, "(", tokenLine, tokenColumn);
            case ')': return Token(TokenType::DELIM_RPAREN, ")", tokenLine, tokenColumn);
            case ';': return Token(TokenType::DELIM_SEMICOLON, ";", tokenLine, tokenColumn);
            default:
                std::cerr << "Lexical Error at line " << tokenLine << ", col " << tokenColumn
                          << ": Unexpected character '" << c << "'\n";
                return Token(TokenType::ERROR, std::string(1, c), tokenLine, tokenColumn);
        }
    }
};

// --------------------------------------------------------------------------
// Abstract Syntax Tree 
// --------------------------------------------------------------------------

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

void printIndent(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "  ";
}

class ProgramNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> varDecls;
    std::unique_ptr<ASTNode> block;

    ProgramNode(std::vector<std::unique_ptr<ASTNode>> vars, std::unique_ptr<ASTNode> blk)
        : varDecls(std::move(vars)), block(std::move(blk)) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Program\n";
        printIndent(indent + 1);
        std::cout << "Variables:\n";
        for (const auto& var : varDecls) {
            var->print(indent + 2);
        }
        printIndent(indent + 1);
        std::cout << "Block:\n";
        if (block) block->print(indent + 2);
    }
};

class VarDeclNode : public ASTNode {
public:
    std::string identifier;
    VarDeclNode(const std::string& id) : identifier(id) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "VarDecl: " << identifier << "\n";
    }
};

class BinaryExprNode : public ASTNode {
public:
    std::string op;
    std::unique_ptr<ASTNode> left;
    std::unique_ptr<ASTNode> right;
    
    BinaryExprNode(const std::string& op,
                   std::unique_ptr<ASTNode> left,
                   std::unique_ptr<ASTNode> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "BinaryExpr: " << op << "\n";
        if (left) left->print(indent + 1);
        if (right) right->print(indent + 1);
    }
};

class IntLiteralNode : public ASTNode {
public:
    std::string value;
    IntLiteralNode(const std::string& val) : value(val) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "IntLiteral: " << value << "\n";
    }
};

class IdentifierNode : public ASTNode {
public:
    std::string name;
    IdentifierNode(const std::string& name) : name(name) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Identifier: " << name << "\n";
    }
};

class PrintNode : public ASTNode {
public:
    std::unique_ptr<ASTNode> expr;
    PrintNode(std::unique_ptr<ASTNode> expr) : expr(std::move(expr)) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Print\n";
        if (expr) expr->print(indent + 1);
    }
};

class ReadNode : public ASTNode {
public:
    std::string identifier;
    ReadNode(const std::string& id) : identifier(id) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Read: " << identifier << "\n";
    }
};

class BlockNode : public ASTNode {
public:
    std::vector<std::unique_ptr<ASTNode>> statements;
    
    BlockNode(std::vector<std::unique_ptr<ASTNode>> stmts)
        : statements(std::move(stmts)) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Block\n";
        for (const auto& stmt : statements) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

// --------------------------------------------------------------------------
// Syntax Analyzer (Parser) - Phase 2
// --------------------------------------------------------------------------

class Parser {
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}
    
    std::unique_ptr<ASTNode> parse() {
        if (!match(TokenType::KW_PROGRAM)) {
            error(peek(), "Expected 'Program' at start");
            return nullptr;
        }
        auto varDecls = parseVars();
        auto blockNode = parseBlocks();
        if (!match(TokenType::KW_END)) {
            error(peek(), "Expected final 'End'");
            return nullptr;
        }
        return std::make_unique<ProgramNode>(std::move(varDecls), std::move(blockNode));
    }
    
private:
    const std::vector<Token>& tokens;
    size_t current;
    
    bool isAtEnd() const {
        return tokens[current].type == TokenType::END_OF_FILE;
    }
    
    const Token& peek() const {
        return tokens[current];
    }
    
    const Token& previous() const {
        return tokens[current - 1];
    }
    
    const Token& advance() {
        if (!isAtEnd()) current++;
        return previous();
    }
    
    bool check(TokenType type) const {
        if (isAtEnd()) return false;
        return peek().type == type;
    }
    
    bool match(TokenType type) {
        if (check(type)) {
            advance();
            return true;
        }
        return false;
    }
    
    void error(const Token& token, const std::string& message) {
        std::cerr << "Syntax Error at line " << token.line << ", col " << token.column
                  << ": " << message << " (found '" << token.lexeme << "')\n";
    }
    
    std::vector<std::unique_ptr<ASTNode>> parseVars() {
        std::vector<std::unique_ptr<ASTNode>> varDecls;
        while (match(TokenType::KW_VAR)) {
            if (!check(TokenType::IDENTIFIER)) {
                error(peek(), "Expected identifier after Var");
                break;
            }
            std::string varName = advance().lexeme;
            varDecls.push_back(std::make_unique<VarDeclNode>(varName));
            if (!match(TokenType::DELIM_SEMICOLON)) {
                error(peek(), "Expected ; after variable");
                break;
            }
        }
        return varDecls;
    }
    
    std::unique_ptr<ASTNode> parseBlocks() {
        if (!match(TokenType::KW_START)) {
            error(peek(), "Expected Start");
            return nullptr;
        }
        auto statements = parseStates();
        if (!match(TokenType::KW_END)) {
            error(peek(), "Expected End");
            return nullptr;
        }
        return std::make_unique<BlockNode>(std::move(statements));
    }
    
    std::vector<std::unique_ptr<ASTNode>> parseStates() {
        std::vector<std::unique_ptr<ASTNode>> stmts;
        while (isStateStart(peek().type)) {
            auto state = parseState();
            if (state)
                stmts.push_back(std::move(state));
        }
        return stmts;
    }
    
    bool isStateStart(TokenType type) {
        return type == TokenType::KW_PRINT || type == TokenType::KW_READ ||
               type == TokenType::KW_START;
    }
    
    std::unique_ptr<ASTNode> parseState() {
        if (check(TokenType::KW_PRINT)) {
            return parseOut();
        } else if (check(TokenType::KW_READ)) {
            return parseIn();
        } else if (check(TokenType::KW_START)) {
            return parseBlocks();
        } else {
            error(peek(), "Unexpected statement");
            advance();
            return nullptr;
        }
    }
    
    std::unique_ptr<ASTNode> parseOut() {
        if (!match(TokenType::KW_PRINT)) return nullptr;
        if (!match(TokenType::DELIM_LPAREN)) {
            error(peek(), "Expected ( after Print");
            return nullptr;
        }
        auto expr = parseExpr();
        if (!match(TokenType::DELIM_RPAREN)) {
            error(peek(), "Expected ) after expression");
            return nullptr;
        }
        if (!match(TokenType::DELIM_SEMICOLON)) {
            error(peek(), "Expected ; after Print");
            return nullptr;
        }
        return std::make_unique<PrintNode>(std::move(expr));
    }
    
    std::unique_ptr<ASTNode> parseIn() {
        if (!match(TokenType::KW_READ)) return nullptr;
        if (!match(TokenType::DELIM_LPAREN)) {
            error(peek(), "Expected ( after Read");
            return nullptr;
        }
        if (!check(TokenType::IDENTIFIER)) {
            error(peek(), "Expected identifier in Read");
            return nullptr;
        }
        std::string id = advance().lexeme;
        if (!match(TokenType::DELIM_RPAREN)) {
            error(peek(), "Expected ) after identifier");
            return nullptr;
        }
        if (!match(TokenType::DELIM_SEMICOLON)) {
            error(peek(), "Expected ; after Read");
            return nullptr;
        }
        return std::make_unique<ReadNode>(id);
    }
    
    std::unique_ptr<ASTNode> parseExpr() {
        auto node = parseR();
        while (check(TokenType::OP_PLUS) || check(TokenType::OP_MINUS)) {
            std::string op = advance().lexeme;
            auto right = parseR();
            node = std::make_unique<BinaryExprNode>(op, std::move(node), std::move(right));
        }
        return node;
    }
    
    std::unique_ptr<ASTNode> parseR() {
        if (check(TokenType::IDENTIFIER)) {
            std::string id = advance().lexeme;
            return std::make_unique<IdentifierNode>(id);
        }
        if (check(TokenType::INTEGER)) {
            std::string val = advance().lexeme;
            return std::make_unique<IntLiteralNode>(val);
        }
        error(peek(), "Expected identifier or number");
        return nullptr;
    }
};

// --------------------------------------------------------------------------
// Semantic Analyzer (Phase 3)
// --------------------------------------------------------------------------

class SemanticAnalyzer {
public:
    SemanticAnalyzer(ASTNode* root) : root(root) {}
    
    void analyze() {
        analyzeNode(root);
    }
    
private:
    ASTNode* root;
    std::unordered_set<std::string> symbolTable;
    
    void analyzeNode(ASTNode* node) {
        if (!node) return;
        
        if (auto program = dynamic_cast<ProgramNode*>(node)) {
            // Process variable declarations first
            for (const auto& varDecl : program->varDecls) {
                if (auto var = dynamic_cast<VarDeclNode*>(varDecl.get())) {
                    if (symbolTable.count(var->identifier)) {
                        std::cerr << "Semantic Error: Duplicate variable '"
                                  << var->identifier << "'\n";
                    } else {
                        symbolTable.insert(var->identifier);
                    }
                }
            }
            // Process main block
            analyzeNode(program->block.get());
            return;
        }
        
        if (auto block = dynamic_cast<BlockNode*>(node)) {
            for (const auto& stmt : block->statements) {
                analyzeNode(stmt.get());
            }
            return;
        }
        
        if (auto id = dynamic_cast<IdentifierNode*>(node)) {
            if (!symbolTable.count(id->name)) {
                std::cerr << "Semantic Error: Undeclared variable '"
                          << id->name << "'\n";
            }
            return;
        }
        
        if (auto read = dynamic_cast<ReadNode*>(node)) {
            if (!symbolTable.count(read->identifier)) {
                std::cerr << "Semantic Error: Reading undeclared variable '"
                          << read->identifier << "'\n";
            }
            return;
        }
        
        if (auto binExpr = dynamic_cast<BinaryExprNode*>(node)) {
            analyzeNode(binExpr->left.get());
            analyzeNode(binExpr->right.get());
            return;
        }
        
        if (auto print = dynamic_cast<PrintNode*>(node)) {
            analyzeNode(print->expr.get());
            return;
        }
    }
};

// --------------------------------------------------------------------------
// Main Function
// --------------------------------------------------------------------------

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source_file>\n";
        return 1;
    }
    
    Lexer lexer(argv[1]);
    auto tokens = lexer.tokenize();
    
    std::cout << "=== Tokens ===\n";
    for (const auto& t : tokens) t.print();
    
    Parser parser(tokens);
    auto ast = parser.parse();
    
    if (ast) {
        std::cout << "\n=== AST ===\n";
        ast->print();
        
        std::cout << "\n=== Semantic Analysis ===\n";
        SemanticAnalyzer analyzer(ast.get());
        analyzer.analyze();
    }
    
    std::cout << "\nCompilation completed.\n";
    return 0;
}
