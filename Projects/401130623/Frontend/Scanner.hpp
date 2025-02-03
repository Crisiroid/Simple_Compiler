
#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iostream>
#include <Delimiters.hpp>
#include <Keywords.hpp>
#include <Operators.hpp>
#include <Token.hpp>

class Scanner
{
public:
    bool isKeyword(const std::string& word) {
        return std::find(Keywords::getKeywords().begin, Keywords::getKeywords().end, word) != Keywords
    }

    bool isOperator(const std::string& word) {
        return std::find(Operators::getOperators().begin, Operators::getOperator().end, word) != Operators
    }
    
    bool isDelimiter(const std::string& word) {
        return std::find(Delimiters::getDelimiters().begin, Delimiters::getDelimiter().end, word) != Delimiters
    }
    std::vector<Token> scan(const std::string& code, int line) {
        std::vector<Token> tokens;
        size_t i = 0;
        while (i < code.length()) 
        {
            if (std::isspace(code[i])) {
                ++i;
                continue;
            }

            if (std::isalpha(code[i])) {
                size_t start = i;
                while (i < code.length() && (std::isalnum(code[i]) || code[i]=='_')){
                    ++i;
                }
                std::string word = code.substr(start, i-start);

                if (isKeyword(word)) {
                    tokens.emplace_back(TokenType::KEYWORD, word, line);
                }
                else {
                    tokens.emplace_back(TokenType::IDENTIFIER, word, line);
                }
            }

            else if (std::isdigit(code[i])) {
                size_t start = i;
                while (i < code.length() && std::isdigit(code[i])){
                    ++i;
                }
                std::string number = code.substr(start, i-start);
                tokens.emplace_back(TokenType::NUMBER, word, line); 
            }
            else if (isOperator(std::string( 1, code[i]))) {
                size_t start = i;
                ++i;
                if (i < code.length() && isOperator( code.substr( start, 2))){
                    tokens.emplace_back(TokenType::OPERATOR, code.substr( start, 2), line);
                    ++i; 
                }
                else {
                    tokens.emplace_back(TokenType::OPERATOR, code.substr( start, 1), line);
                }
            }
            else {
                tokens.emplace_back(TokenType::UNKNOWN, std::string( 1, code[i]), line);
                ++i;
            }
        }
        return tokens;
    }
};

#endif // SCANNER_HPP