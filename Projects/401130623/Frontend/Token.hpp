#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    KEYWORD,
    IDENTIFIER,
    OPERATOR,
    NUMBER,
    END_OF_FILE,
    UNKNOWN,
};

struct token {
    TokenType type;
    std::string value;
    int line;

    token(TokenType type, const std::string& value,int line)
        : type(type), value(value), line(line) {}
};

#endif // TOKEN_H