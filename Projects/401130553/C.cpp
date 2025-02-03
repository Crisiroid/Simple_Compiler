#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
using namespace std;

vector<pair<string, string>> tokenPatterns = {
    {"KEYWORD", R"(^\s*\b(Program|end|Var|Start|End|Print|Read|if|Iteration|Put)\b)"},
    {"IDENTIFIER", R"(^\s*([a-zA-Z_]\w*))"},
    {"NUMBER", R"(^\s*\b(\d+)\b)"},
    {"OPERATOR", R"(^\s*(=|<|>|==|\+|-))"},
    {"PUNCTUATION", R"(^\s*(;|\(|\)|\{|\}))"},
    {"WHITESPACE", R"(^\s*)"}
};

struct Token {
    string value;
    string type;
    int line;
};

vector<Token> tokenize(string code) {
    int position = 0;
    int line = 1;
    vector<Token> tokens;

    while (position < code.length()) {
        smatch match;
        bool matched = false;
        string sub = code.substr(position);

        for (int i = 0; i < tokenPatterns.size(); i++) {
            regex pattern(tokenPatterns[i].second);
            if (regex_search(sub, match, pattern)) {

                string matchedStr = match[0].str();
                for (char c : matchedStr) {
                    if (c == '\n') {
                        line++;
                    }
                }

                if (tokenPatterns[i].first != "WHITESPACE") {
                    tokens.push_back({match[1], tokenPatterns[i].first, line});
                }

                position += match.length(0);
                matched = true;
                break;
            }
        }

        if (!matched) {
            tokens.push_back({string(1, code[position]), "ERROR", line});
            position++;
        }
    }

    return tokens;
}

int currentTokenIndex = 0;
vector<Token> tokens;
bool hasError = false;

void reportError(string message) {
    cout << "Error at line " << tokens[currentTokenIndex].line << ": " << message << " (Found: '" << tokens[currentTokenIndex].value << "')" << endl;
}

bool matchToken(Token expected, string errorMessage) {
    if (expected.type != "NUMBER" && expected.type != "IDENTIFIER") {
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].value == expected.value) {
            currentTokenIndex++;
            return true;
        }
        reportError(errorMessage);
        hasError = true;
        return false;
    } else {
        if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == expected.type) {
            currentTokenIndex++;
            return true;
        }
        reportError(errorMessage);
        hasError = true;
        return false;
    }
}

void synchronize(vector<string> followSet) {
    while (currentTokenIndex < tokens.size()) {
        for (string follow : followSet) {
            if ((tokens[currentTokenIndex].value == follow) || (tokens[currentTokenIndex].type == follow)) {
                return;
            }
        }
        if (currentTokenIndex + 1 >= tokens.size()) return;
        currentTokenIndex++;
    }
}

bool parseVariables();
bool parseBlock();
bool parseStatements();
bool parseStatement();
bool parseMultipleStatements();
bool parseOutput();
bool parseInput();
bool parseIf();
bool parseLoop();
bool parseAssignment();
bool parseOperator();
bool parseExpression();
bool parseExpressionPrime();
bool parseFactor();

bool parseProgram() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected a program declaration." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Program") {
        matchToken(Token{"Program", "KEYWORD"}, "Expected 'Program' keyword");
        if (!parseVariables()) { return false; }
        if (!parseBlock()) { return false; }
        matchToken(Token{"end", "KEYWORD"}, "Expected 'end' keyword to close the program");
        return true;
    } else {
        hasError = true;
        reportError("Expected 'Program' keyword at the beginning");
        synchronize({"Program"});
        return true;
    }
}

bool parseVariables() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected variable declaration." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Var") {
        matchToken(Token{"Var", "KEYWORD"}, "Expected 'Var' keyword");
        matchToken(Token{"", "IDENTIFIER"}, "Expected an identifier for variable declaration");
        matchToken(Token{";", "PUNCTUATION"}, "Expected ';' after variable declaration");
        if (!parseVariables()) { return false; }
        return true;
    } else if (tokens[currentTokenIndex].value == "Start") {
        return true;
    } else {
        hasError = true;
        reportError("Expected 'Var' or 'Start'");
        synchronize({"Start"});
        return true;
    }
}

bool parseBlock() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected a block of statements." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Start") {
        matchToken(Token{"Start", "KEYWORD"}, "Expected 'Start' keyword");
        if (!parseStatements()) { return false; }
        matchToken(Token{"End", "KEYWORD"}, "Expected 'End' keyword to close the block");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"If", "Read", "Print", "Put", "Iteration", "end", "End"};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected 'Start' keyword to begin a block");
        synchronize({"If", "Read", "Print", "Put", "Iteration", "end", "End"});
        return true;
    }
}

bool parseStatements() {
    if (!parseStatement() || !parseMultipleStatements()) { return false; }
    return true;
}

bool parseStatement() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected a statement." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Start") { return parseBlock(); }
    else if (tokens[currentTokenIndex].value == "If") { return parseIf(); }
    else if (tokens[currentTokenIndex].value == "Read") { return parseInput(); }
    else if (tokens[currentTokenIndex].value == "Print") { return parseOutput(); }
    else if (tokens[currentTokenIndex].value == "Put") { return parseAssignment(); }
    else if (tokens[currentTokenIndex].value == "Iteration") { return parseLoop(); }
    else {
        hasError = true;
        vector<string> follow = {"End"};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Unknown statement");
        synchronize({"End"});
        return true;
    }
}

bool parseMultipleStatements() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected more statements or 'End'." << endl;
        hasError = true;
        return false;
    }

    string token = tokens[currentTokenIndex].value;

    if (token == "Start" || token == "If" || token == "Read" || token == "Print" || token == "Put" || token == "Iteration") {
        return parseStatements();
    } else if (token == "End") {
        return true;
    } else {
        hasError = true;
        reportError("Expected a statement or 'End'");
        synchronize({"End"});
        return true;
    }
}

bool parseOutput() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an output statement." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Print") {
        matchToken(Token{"Print", "KEYWORD"}, "Expected 'Print' keyword");
        matchToken(Token{"(", "PUNCTUATION"}, "Expected '(' after 'Print'");
        if (!parseExpression()) { return false; }
        matchToken(Token{")", "PUNCTUATION"}, "Expected ')' after expression");
        matchToken(Token{";", "PUNCTUATION"}, "Expected ';' to end the statement");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ";"};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected 'Print' keyword");
        synchronize({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}

bool parseInput() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an input statement." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Read") {
        matchToken(Token{"Read", "KEYWORD"}, "Expected 'Read' keyword");
        matchToken(Token{"(", "PUNCTUATION"}, "Expected '(' after 'Read'");
        matchToken(Token{"", "IDENTIFIER"}, "Expected an identifier for input");
        matchToken(Token{")", "PUNCTUATION"}, "Expected ')' after identifier");
        matchToken(Token{";", "PUNCTUATION"}, "Expected ';' to end the statement");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ";"};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected 'Read' keyword");
        synchronize({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}

bool parseIf() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an 'if' statement." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "If") {
        matchToken(Token{"If", "KEYWORD"}, "Expected 'If' keyword");
        matchToken(Token{"(", "PUNCTUATION"}, "Expected '(' after 'If'");
        if (!parseExpression()) { return false; }
        if (!parseOperator()) { return false; }
        if (!parseExpression()) { return false; }
        matchToken(Token{")", "PUNCTUATION"}, "Expected ')' after condition");
        matchToken(Token{"{", "PUNCTUATION"}, "Expected '{' to start the block");
        if (!parseStatement()) { return false; }
        matchToken(Token{"}", "PUNCTUATION"}, "Expected '}' to end the block");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ""};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected 'If' keyword");
        synchronize({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}

bool parseLoop() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an 'Iteration' statement." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Iteration") {
        matchToken(Token{"Iteration", "KEYWORD"}, "Expected 'Iteration' keyword");
        matchToken(Token{"(", "PUNCTUATION"}, "Expected '(' after 'Iteration'");
        if (!parseExpression()) { return false; }
        if (!parseOperator()) { return false; }
        if (!parseExpression()) { return false; }
        matchToken(Token{")", "PUNCTUATION"}, "Expected ')' after condition");
        matchToken(Token{"{", "PUNCTUATION"}, "Expected '{' to start the block");
        if (!parseStatement()) { return false; }
        matchToken(Token{"}", "PUNCTUATION"}, "Expected '}' to end the block");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ""};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected 'Iteration' keyword");
        synchronize({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}

bool parseAssignment() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an assignment statement." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "Put") {
        matchToken(Token{"Put", "KEYWORD"}, "Expected 'Put' keyword");
        matchToken(Token{"", "IDENTIFIER"}, "Expected an identifier for assignment");
        matchToken(Token{"=", "OPERATOR"}, "Expected '=' for assignment");
        if (!parseExpression()) { return false; }
        matchToken(Token{";", "PUNCTUATION"}, "Expected ';' to end the statement");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ";"};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected 'Put' keyword");
        synchronize({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}

bool parseOperator() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an operator." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].value == "<") {
        matchToken(Token{"<", "OPERATOR"}, "Expected '<' operator");
        return true;
    } else if (tokens[currentTokenIndex].value == ">") {
        matchToken(Token{">", "OPERATOR"}, "Expected '>' operator");
        return true;
    } else if (tokens[currentTokenIndex].value == "==") {
        matchToken(Token{"==", "OPERATOR"}, "Expected '==' operator");
        return true;
    } else {
        hasError = true;
        vector<string> follow = {"NUMBER", "IDENTIFIER"};
        for (string f : follow) {
            if (tokens[currentTokenIndex].type == f) {
                return true;
            }
        }
        reportError("Expected an operator (<, >, or ==)");
        synchronize({"NUMBER", "IDENTIFIER"});
        return true;
    }
}

bool parseExpression() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an expression." << endl;
        hasError = true;
        return false;
    }

    if (tokens[currentTokenIndex].type == "NUMBER" || tokens[currentTokenIndex].type == "IDENTIFIER") {
        return parseFactor() && parseExpressionPrime();
    } else {
        hasError = true;
        vector<string> follow = {";", ")", "<", ">", "=="};
        for (string f : follow) {
            if (tokens[currentTokenIndex].value == f) {
                return true;
            }
        }
        reportError("Expected an expression (number or identifier)");
        synchronize({";", ")", "<", ">", "=="});
        return true;
    }
}

bool parseExpressionPrime() {
    if (currentTokenIndex >= tokens.size()) {
        cout << "Error: Unexpected end of input. Expected an expression continuation." << endl;
        hasError = true;
        return false;
    }

    string token = tokens[currentTokenIndex].value;
    if (token == "+") {
        matchToken(Token{"+", "OPERATOR"}, "Expected '+' operator");
        if (!parseFactor()) { return false; }
        if (!parseExpressionPrime()) { return false; }
        return true;
    } else if (token == "-") {
        matchToken(Token{"-", "OPERATOR"}, "Expected '-' operator");
        if (!parseFactor()) { return false; }
        if (!parseExpressionPrime()) { return false; }
        return true;
    } else if (token == ")" || token == ";" || token == "<" || token == ">" || token == "==") {
        return true;
    } else {
        hasError = true;
        reportError("Expected '+', '-', or end of expression");
        synchronize({";", ")", "<", ">", "=="});
        return true;
}
   }  
   
int main() {
    // خواندن کد از فایل
    ifstream inputFile("input.txt");
    if (!inputFile.is_open()) {
        cout << "Error: Could not open input file." << endl;
        return 1;
    }

    string code((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
    inputFile.close();

    // توکن‌بندی کد
    tokens = tokenize(code);

    // شروع تجزیه و تحلیل نحوی
    if (parseProgram()) {
        if (!hasError) {
            cout << "Parsing completed successfully. No errors found." << endl;
        } else {
            cout << "Parsing completed with errors." << endl;
        }
    } else {
        cout << "Parsing failed due to errors." << endl;
    }

    return 0;
}