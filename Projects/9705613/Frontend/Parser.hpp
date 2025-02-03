#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <stack>
#include <vector>
#include "Token.hpp"

enum class NonTerminal { S, VARS, BLOCKS, STATES, STATE, IF, IN, OUT, ASSIGN, LOOP, EXPR, R, O };
enum class Terminal { PROGRAM, VAR, START, END, IF, ITERATION, PRINT, READ, PUT, IDENTIFIER, NUMBER, OPERATOR, DELIMITER, END_OF_FILE };

inline Terminal getTerminal(const Token& token) {
    if (token.value == "Program") return Terminal::PROGRAM;
    if (token.value == "Var") return Terminal::VAR;
    if (token.value == "Start") return Terminal::START;
    if (token.value == "End") return Terminal::END;
    if (token.value == "If") return Terminal::IF;
    if (token.value == "Iteration") return Terminal::ITERATION;
    if (token.value == "Print") return Terminal::PRINT;
    if (token.value == "Read") return Terminal::READ;
    if (token.value == "Put") return Terminal::PUT;
    if (token.type == TokenType::IDENTIFIER) return Terminal::IDENTIFIER;
    if (token.type == TokenType::NUMBER) return Terminal::NUMBER;
    if (token.type == TokenType::OPERATOR) return Terminal::OPERATOR;
    if (token.type == TokenType::DELIMITER) return Terminal::DELIMITER;
    return Terminal::END_OF_FILE;
}

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0) {}

    bool Parse() {
        parseStack.push(NonTerminal::S);
        return parseS();
    }

private:
    const std::vector<Token>& tokens;
    int currentTokenIndex;
    std::stack<NonTerminal> parseStack;

    [[nodiscard]] Token currentToken() const {
        return tokens[currentTokenIndex];
    }

    void advance() {
        if (currentTokenIndex < tokens.size() - 1) currentTokenIndex++;
    }

    bool match(Terminal expected) {
        if (getTerminal(currentToken()) == expected) {
            advance();
            return true;
        } else {
            std::cerr << "Syntax Error: Expected " << getTerminalName(expected) << " but got " << currentToken().value << " at line " << currentToken().line << std::endl;
            return false;
        }
    }

    static std::string getTerminalName(Terminal terminal) {
        switch (terminal) {
            case Terminal::PROGRAM: return "Program";
            case Terminal::VAR: return "Var";
            case Terminal::START: return "Start";
            case Terminal::END: return "End";
            case Terminal::IF: return "If";
            case Terminal::ITERATION: return "Iteration";
            case Terminal::PRINT: return "Print";
            case Terminal::READ: return "Read";
            case Terminal::PUT: return "Put";
            case Terminal::IDENTIFIER: return "Identifier";
            case Terminal::NUMBER: return "Number";
            case Terminal::OPERATOR: return "Operator";
            case Terminal::DELIMITER: return "Delimiter";
            case Terminal::END_OF_FILE: return "End of File";
            default: return "Unknown";
        }
    }

    bool parseS() {
        if (!match(Terminal::PROGRAM)) return false;
        if (!parseVARS()) return false;
        if (!parseBLOCKS()) return false;
        if (!match(Terminal::END)) return false;
        return true;
    }

    bool parseVARS() {
        if (getTerminal(currentToken()) == Terminal::VAR) {
            if (!match(Terminal::VAR)) return false;
            if (!match(Terminal::IDENTIFIER)) return false;
            if (!match(Terminal::DELIMITER)) return false; // Assuming ';' is a delimiter
            return parseVARS();
        }
        return true; // Epsilon
    }

    bool parseBLOCKS() {
        if (!match(Terminal::START)) return false;
        if (!parseSTATES()) return false;
        if (!match(Terminal::END)) return false;
        return true;
    }

    bool parseSTATES() {
        if (!parseSTATE()) return false;
        return parseM_STATES();
    }

    bool parseSTATE() {
        Terminal term = getTerminal(currentToken());
        if (term == Terminal::START) return parseBLOCKS();
        if (term == Terminal::IF) return parseIF();
        if (term == Terminal::READ) return parseIN();
        if (term == Terminal::PRINT) return parseOUT();
        if (term == Terminal::PUT) return parseASSIGN();
        if (term == Terminal::ITERATION) return parseLOOP();
        return false;
    }

    bool parseM_STATES() {
        if (getTerminal(currentToken()) == Terminal::START ||
            getTerminal(currentToken()) == Terminal::IF ||
            getTerminal(currentToken()) == Terminal::READ ||
            getTerminal(currentToken()) == Terminal::PRINT ||
            getTerminal(currentToken()) == Terminal::PUT ||
            getTerminal(currentToken()) == Terminal::ITERATION) {
            return parseSTATES();
        }
        return true; // Epsilon
    }

    bool parseIF() {
        if (!match(Terminal::IF)) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming '(' is a delimiter
        if (!parseEXPR()) return false;
        if (!parseO()) return false;
        if (!parseEXPR()) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming ')' is a delimiter
        if (!match(Terminal::DELIMITER)) return false; // Assuming '{' is a delimiter
        if (!parseSTATE()) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming '}' is a delimiter
        return true;
    }

    bool parseIN() {
        if (!match(Terminal::READ)) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming '(' is a delimiter
        if (!match(Terminal::IDENTIFIER)) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming ')' is a delimiter
        if (!match(Terminal::DELIMITER)) return false; // Assuming ';' is a delimiter
        return true;
    }

    bool parseOUT() {
        if (!match(Terminal::PRINT)) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming '(' is a delimiter
        if (!parseEXPR()) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming ')' is a delimiter
        if (!match(Terminal::DELIMITER)) return false; // Assuming ';' is a delimiter
        return true;
    }

    bool parseASSIGN() {
        if (!match(Terminal::PUT)) return false;
        if (!match(Terminal::IDENTIFIER)) return false;
        if (!match(Terminal::OPERATOR)) return false; // Assuming '=' is an operator
        if (!parseEXPR()) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming ';' is a delimiter
        return true;
    }

    bool parseLOOP() {
        if (!match(Terminal::ITERATION)) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming '(' is a delimiter
        if (!parseEXPR()) return false;
        if (!parseO()) return false;
        if (!parseEXPR()) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming ')' is a delimiter
        if (!match(Terminal::DELIMITER)) return false; // Assuming '{' is a delimiter
        if (!parseSTATE()) return false;
        if (!match(Terminal::DELIMITER)) return false; // Assuming '}' is a delimiter
        return true;
    }

    bool parseEXPR() {
        if (!parseR()) return false;
        Terminal term = getTerminal(currentToken());
        if (term == Terminal::OPERATOR) {
            if (!match(Terminal::OPERATOR)) return false;
            return parseEXPR();
        }
        return true;
    }

    bool parseR() {
        Terminal term = getTerminal(currentToken());
        if (term == Terminal::IDENTIFIER || term == Terminal::NUMBER) {
            advance();
            return true;
        }
        return false;
    }

    bool parseO() {
        if (!match(Terminal::OPERATOR)) return false;
        return true;
    }
};

#endif