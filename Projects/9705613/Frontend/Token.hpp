#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <utility>

enum class TokenType {
    IDENTIFIER,
    KEYWORD,
    NUMBER,
    OPERATOR,
    DELIMITER,
    INVALID
};

class Token {
public:
    TokenType type;
    std::string value;
    int line;

    Token(TokenType type, std::string  value, int line)
        : type(type), value(std::move(value)), line(line) {}

    // Method to return a string representation of the token type
    [[nodiscard]] std::string getTypeAsString() const {
        switch (type) {
            case TokenType::IDENTIFIER: return "IDENTIFIER";
            case TokenType::KEYWORD: return "KEYWORD";
            case TokenType::NUMBER: return "NUMBER";
            case TokenType::OPERATOR: return "OPERATOR";
            case TokenType::DELIMITER: return "DELIMITER";
            default: return "UNKNOWN";
        }
    }
};

#endif // TOKEN_HPP
