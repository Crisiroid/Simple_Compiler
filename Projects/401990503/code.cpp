#include <iostream>
#include <vector>
#include <string>

enum TokenType {
    IDENTIFIER, INTEGER, KEYWORD, OPERATOR, SYMBOL, UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
};

// مثال: توکن‌های کلمات کلیدی
std::vector<std::string> keywords = {"if", "else", "var", "print", "read"};

bool isKeyword(const std::string& str) {
    for (const auto& keyword : keywords) {
        if (str == keyword) {
            return true;
        }
    }
    return false;
}

Token lex(const std::string& input) {
    if (input.length() <= 5 && isalpha(input[0])) {
        if (isKeyword(input)) {
            return {KEYWORD, input};
        }
        return {IDENTIFIER, input};
    }
    
    // شناسایی اعداد (Integer)
    bool isInteger = true;
    for (char c : input) {
        if (!isdigit(c)) {
            isInteger = false;
            break;
        }
    }
    if (isInteger) {
        return {INTEGER, input};
    }
    
    // شناسایی عملگرها (Operator)
    if (input == "=" || input == "<" || input == ">" || input == "==") {
        return {OPERATOR, input};
    }
    
    // شناسایی نمادها (Symbol)
    if (input == ";" || input == "(" || input == ")" || input == "{" || input == "}") {
        return {SYMBOL, input};
    }

    // در غیر این صورت، شناسه نامعلوم (UNKNOWN)
    return {UNKNOWN, input};
}

int main() {
    std::string input = "if var x < 10;";
    std::vector<Token> tokens;

    std::string current_token;
    for (char ch : input) {
        if (ch == ' ' || ch == ';' || ch == '(' || ch == ')') {
            if (!current_token.empty()) {
                tokens.push_back(lex(current_token));
                current_token.clear();
            }
            if (ch != ' ') {
                tokens.push_back(lex(std::string(1, ch)));
            }
        } else {
            current_token += ch;
        }
    }
    if (!current_token.empty()) {
        tokens.push_back(lex(current_token));
    }

    for (const auto& token : tokens) {
        std::cout << "Type: " << token.type << ", Value: " << token.value << std::endl;
    }

    return 0;
}
