#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <vector>
#include <algorithm>
#include "Token.hpp"
#include "Keywords.hpp"
#include "DFA.hpp"

class Scanner {
public:
    Scanner() : dfa() {}
    std::vector<Token> scan(const std::string& line, int& lineNumber) {
        std::vector<Token> tokens;
        std::string tokenValue;
        dfa.reset();

        for (char ch : line) {
            if (std::isspace(ch)) {
                if (!tokenValue.empty()) {
                    processAndAddToken(tokenValue, lineNumber, tokens);
                    tokenValue.clear();
                }
                dfa.reset();
                continue;
            }

            if (dfa.isDelimiterChar(ch)) {
                if (!tokenValue.empty()) {
                    processAndAddToken(tokenValue, lineNumber, tokens);
                    tokenValue.clear();
                }

                tokens.emplace_back(TokenType::DELIMITER, std::string(1, ch), lineNumber);
                dfa.reset();
                continue;
            }

            dfa.transition(ch);
            tokenValue += ch;

            if (dfa.getCurrentState() == State::DONE) {
                processAndAddToken(tokenValue, lineNumber, tokens);
                tokenValue.clear();
                dfa.reset();
            }
        }

        if (!tokenValue.empty()) {
            processAndAddToken(tokenValue, lineNumber, tokens);
        }

        return tokens;
    }

private:
    DFA dfa;

    void processAndAddToken(std::string& tokenValue, int line, std::vector<Token>& tokens) {
        TokenType type = identifyTokenType(tokenValue);

        // Check identifier length
        if (type == TokenType::IDENTIFIER) {
            if (tokenValue.length() > 5) {
                std::cerr << "Lexical Error: Identifier '" << tokenValue << "' exceeds 5 characters at line " << line << std::endl;
                return;
            }
        }

        tokens.emplace_back(type, tokenValue, line);
    }

    TokenType identifyTokenType(const std::string& value) {
        switch (dfa.getCurrentState()) {
            case State::IN_KEYWORD:
                if (isKeyword(value)) {
                    return TokenType::KEYWORD;
                }
                return TokenType::IDENTIFIER;

            case State::IN_NUMBER:
                return TokenType::NUMBER;

            case State::IN_OPERATOR:
                return TokenType::OPERATOR;

            case State::IN_DELIMITER:
                return TokenType::DELIMITER;

            default:
                return TokenType::IDENTIFIER;
        }
    }

    static bool isKeyword(const std::string& value) {
        return std::find(Keywords::getKeywords().begin(), Keywords::getKeywords().end(), value) != Keywords::getKeywords().end();
    }
};

#endif