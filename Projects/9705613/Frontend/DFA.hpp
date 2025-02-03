#ifndef DFA_HPP
#define DFA_HPP

#include <regex>
#include <cctype>

enum class State {
    START,
    IN_KEYWORD,
    IN_NUMBER,
    IN_OPERATOR,
    IN_DELIMITER,
    DONE
};

class DFA {
public:
    DFA() : currentState(State::START) {
        initializeRegexPatterns();
    }

    [[nodiscard]] State getCurrentState() const {
        return currentState;
    }

    void transition(char ch) {
        switch (currentState) {
            case State::START:
                if (isOperatorChar(ch)) {
                    currentState = State::IN_OPERATOR;
                } else if (std::isdigit(ch)) {
                    currentState = State::IN_NUMBER;
                } else if (isIdentifierStart(ch)) {
                    currentState = State::IN_KEYWORD;
                } else if (isDelimiterChar(ch)) {
                    currentState = State::IN_DELIMITER;
                } else {
                    currentState = State::START;
                }
                break;

            case State::IN_KEYWORD:
                if (isIdentifierBody(ch)) {
                    currentState = State::IN_KEYWORD;
                } else {
                    currentState = State::DONE;
                }
                break;

            case State::IN_NUMBER:
                if (std::isdigit(ch)) {
                    currentState = State::IN_NUMBER;
                } else {
                    currentState = State::DONE;
                }
                break;

            case State::IN_OPERATOR:
                tempOperator += ch;
                if (isRegexMatch(tempOperator, operatorRegex)) {
                    currentState = State::IN_OPERATOR;
                } else {
                    currentState = State::DONE;
                }
                break;

            case State::IN_DELIMITER:
                currentState = State::DONE;
                break;

            default:
                currentState = State::START;
                break;
        }
    }

    void reset() {
        currentState = State::START;
        tempOperator.clear();
    }

    const std::string& getTempOperator() const {
        return tempOperator;
    }

    bool isDelimiterChar(char ch) {
        return std::regex_match(std::string(1, ch), delimiterRegex);
    }

private:
    State currentState;
    std::string tempOperator;

    // Regular expressions for operators and delimiters
    std::regex operatorRegex;
    std::regex delimiterRegex;

    void initializeRegexPatterns() {
        operatorRegex = std::regex("^[+\\-*/=<>!&|^%]+$");
        delimiterRegex = std::regex(R"([(){}[\];,])");
    }

    static bool isRegexMatch(const std::string& value, const std::regex& pattern) {
        return std::regex_match(value, pattern);
    }

    static bool isIdentifierStart(char ch) {
        return std::isalpha(ch) || ch == '_';
    }

    static bool isIdentifierBody(char ch) {
        return std::isalnum(ch) || ch == '_';
    }

    static bool isOperatorChar(char ch) {
        static const std::string operators = "+-*/=<>!&|^%";
        return operators.find(ch) != std::string::npos;
    }
};

#endif