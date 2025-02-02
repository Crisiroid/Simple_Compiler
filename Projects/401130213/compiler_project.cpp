#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <windows.h>
#include <conio.h>
#include <iomanip>
#include <fstream>
#include <streambuf>

#define defcolor 7
#define graycolor 8
#define greencolor 2
#define bluecolor 9
#define yellowcolor 14
#define brightwhitecolor 15
#define redcolor 4

using namespace std;

HANDLE hand = GetStdHandle(STD_OUTPUT_HANDLE);
void gotoxy(short x, short y) {
    COORD pos = { x,y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

enum Token_Type {
    PROGRAM, VAR, IDENTIFIER, START, END, PROGRAM_END, PRINT, READ, IF, ITERATION, PUT,
    ASSIGN, LPAREN, RPAREN, LBRACE, RBRACE, SEMICOLON, PLUS, MINUS, LESS, GREATER, EQUAL,
    INTEGER, UNKNOWN
};

const char* tokenTypeNames[] = {
    "PROGRAM", "VAR", "IDENTIFIER", "START", "END", "PROGRAM_END", "PRINT", "READ", "IF", "ITERATION", "PUT",
    "ASSIGN", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON", "PLUS", "MINUS", "LESS", "GREATER", "EQUAL",
    "INTEGER", "UNKNOWN", "PUNCTUATION"
};

std::string getTokenName(Token_Type type) {
    return tokenTypeNames[type];
}

map<string, Token_Type> keywords = {
    {"Program", PROGRAM}, {"Var", VAR}, {"Start", START}, {"End", END}, {"end", PROGRAM_END},
    {"Print", PRINT}, {"Read", READ}, {"If", IF}, {"Iteration", ITERATION}, {"Put", PUT}
};

struct Token {
    Token_Type type;
    string value;
    int line = 1;
};


Token getNextToken(const string& input, size_t& pos, int& current_line) {
    while (pos < input.length() && isspace(input[pos])) {
        string temp;
        temp = temp + input[pos];
        if (temp == "\n") {
            current_line += 1;
        }
        pos++;
    }

    if (pos == input.length()) return { UNKNOWN, "", current_line };

    char currentChar = input[pos];

    if (isalpha(currentChar)) {
        string identifier;
        while (pos < input.length() && isalnum(input[pos])) {
            identifier += input[pos++];
        }
        if (keywords.find(identifier) != keywords.end()) {
            return { keywords[identifier], identifier, current_line };
        }
        else {
            return { IDENTIFIER, identifier, current_line };
        }
    }

    if (isdigit(currentChar)) {
        string number;
        while (pos < input.length() && isdigit(input[pos])) {
            number += input[pos++];
        }
        return { INTEGER, number, current_line };
    }

    switch (currentChar) {
    case '=':
        if (input[pos + 1] == '=') {
            pos += 2;
            return { EQUAL, "==", current_line };
        }
        else {
            pos++;
            return { ASSIGN, "=", current_line };
        }
    case '+': pos++; return { PLUS, "+" , current_line };
    case '-': pos++; return { MINUS, "-", current_line };
    case '<': pos++; return { LESS, "<", current_line };
    case '>': pos++; return { GREATER, ">", current_line };
    case '(': pos++; return { LPAREN, "(", current_line };
    case ')': pos++; return { RPAREN, ")", current_line };
    case '{': pos++; return { LBRACE, "{", current_line };
    case '}': pos++; return { RBRACE, "}", current_line };
    case ';': pos++; return { SEMICOLON, ";", current_line };
    default:
        pos++;
        return { UNKNOWN, string(1, currentChar), current_line };
    }
}


class Parser {
public:
    Parser(const vector<Token>& tokens, bool parserAccept) : tokens(tokens), currentTokenIndex(0), parserAccept(parserAccept) {}

    void parse() {
        SetConsoleTextAttribute(hand, bluecolor);
        cout << "\nPlease enter any button to continue ... \n\n";
        SetConsoleTextAttribute(hand, defcolor);
        _getch();
        S();
        cout << "\n\nParsing complete." << endl;
        if (parserAccept) {
            SetConsoleTextAttribute(hand, bluecolor);
            cout << "\n --- Parser accepted the given string!\n";
        }
        else {
            SetConsoleTextAttribute(hand, redcolor);
            cout << "\n --- Parser rejected the given string!";
        }
        SetConsoleTextAttribute(hand, defcolor);
    }

private:
    vector<Token> tokens;
    size_t currentTokenIndex;
    bool parserAccept;

    void parserReject() {
        parserAccept = false;
    }

    Token getNextToken() {
        if (currentTokenIndex < tokens.size()) {
            return tokens[currentTokenIndex++];
        }
        return { UNKNOWN, "" };
    }

    void S() {
        expect(PROGRAM);
        vars();
        blocks();
        expect(PROGRAM_END);
    }

    void vars() {
        if (lookahead(VAR)) {
            expect(VAR);
            expect(IDENTIFIER);
            expect(SEMICOLON);
            vars();
        }
    }

    void blocks() {
        expect(START);
        states();
        expect(END);
    }

    void states() {
        state();
        m_states();
    }

    void state() {
        if (lookahead(START)) {
            blocks();
        }
        else if (lookahead(IF)) {
            if_state();
        }
        else if (lookahead(READ)) {
            in_state();
        }
        else if (lookahead(PRINT)) {
            out_state();
        }
        else if (lookahead(PUT)) {
            assign_state();
        }
        else if (lookahead(ITERATION)) {
            loop_state();
        }
    }

    void m_states() {
        //if (lookahead(START) || lookahead(IF) || lookahead(READ) || lookahead(PRINT) || lookahead(PUT) || lookahead(ITERATION) )
        if (lookahead(START) || lookahead(IF) || lookahead(READ) || lookahead(PRINT) || lookahead(PUT) || lookahead(ITERATION)) {
            states();
        }
    }

    void out_state() {
        expect(PRINT);
        expect(LPAREN);
        expr();
        expect(RPAREN);
        expect(SEMICOLON);
    }

    void in_state() {
        expect(READ);
        expect(LPAREN);

        expect(IDENTIFIER);
        expect(RPAREN);
        expect(SEMICOLON);
    }

    void if_state() {
        expect(IF);
        expect(LPAREN);
        expr();
        o();
        expr();
        expect(RPAREN);
        expect(LBRACE);
        state();
        expect(RBRACE);
    }

    void loop_state() {
        expect(ITERATION);
        expect(LPAREN);
        expr();
        o();
        expr();
        expect(RPAREN);
        expect(LBRACE);
        state();
        expect(RBRACE);
    }

    void assign_state() {
        expect(PUT);
        expect(IDENTIFIER);
        expect(ASSIGN);
        expr();
        expect(SEMICOLON);
    }

    void o() {
        if (lookahead(LESS)) {
            expect(LESS);
        }
        else if (lookahead(GREATER)) {
            expect(GREATER);
        }
        else if (lookahead(EQUAL)) {
            expect(EQUAL);
        }
    }

    void expr() {
        r();
        if (lookahead(PLUS)) {
            expect(PLUS);
            expr();
        }
        else if (lookahead(MINUS)) {
            expect(MINUS);
            expr();
        }
    }

    void r() {
        if (lookahead(IDENTIFIER)) {
            expect(IDENTIFIER);
        }
        else if (lookahead(INTEGER)) {
            expect(INTEGER);
        }
    }

    bool lookahead(Token_Type type) {
        if (currentTokenIndex < tokens.size()) {
            return tokens[currentTokenIndex].type == type;
        }
        return false;
    }

    void expect(Token_Type type) {
        Token token = getNextToken();
        if (token.type != type) {
            SetConsoleTextAttribute(hand, redcolor);
            cout << "Syntax error: expected " << tokenTypeNames[type] << ", got " << tokenTypeNames[token.type] << ", at token index of " << currentTokenIndex << ", at token line of " << token.line << endl;
            SetConsoleTextAttribute(hand, defcolor);
            parserReject();
            //exit(1);
        }
        else {
            cout << "match(" << tokenTypeNames[token.type] << ")\n";
        }
    }

    const char* tokenTypeNames[25] = {
        "PROGRAM", "VAR", "IDENTIFIER", "START", "END", "PROGRAM_END", "PRINT", "READ", "IF", "ITERATION", "PUT",
        "ASSIGN", "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMICOLON", "PLUS", "MINUS", "LESS", "GREATER", "EQUAL",
        "INTEGER", "UNKNOWN", "PUNCTUATION"
    };
};


string mainMenu() {
    int midwayPointX = 60;
    int midwayPointY = 10;

    system("MODE 102,60");
    string message = "Compiler Final Project";
    gotoxy(midwayPointX - message.length(), midwayPointY);
    SetConsoleTextAttribute(hand, yellowcolor);
    cout << message;
    message = "Designed and programmed by Kiamehr Behnia";
    gotoxy(midwayPointX - (message.length() / 1.25) + 2, midwayPointY + 1);
    cout << message;

    SetConsoleTextAttribute(hand, bluecolor);
    gotoxy(0, midwayPointY + 3);
    cout << R"(                                           .
                                          / \
                                          | |
                                          |.|
                                          |.|
                                          |:|      __
                                        ,_|:|_,   /  )
                                          (Oo    / _I_
                                           +\ \  || __|
                                              \ \||___|
                                                \ /.:.\-\
                                                 |.:. /-----\
                                                 |___|:COMP.:|
                                                 /   |:ILER::|
                                                |_____\  :::/
                                                 | |  \ \:/
                                                 | |   | |
                                                 \ /   | \__
                                                 / |   \____\
                                                 `-')";
    _getch();
    SetConsoleTextAttribute(hand, defcolor);
    system("CLS");


    message = "Please select one of the commands: ";
    gotoxy(midwayPointX - message.length() / 1.5, midwayPointY);
    SetConsoleTextAttribute(hand, yellowcolor);
    cout << message;

    message = "1- Write your own string. ";
    gotoxy(midwayPointX - 20, midwayPointY + 2);
    SetConsoleTextAttribute(hand, greencolor);
    cout << message;

    message = "2- Read from a file. ";
    gotoxy(midwayPointX - 20, midwayPointY + 3);
    SetConsoleTextAttribute(hand, bluecolor);
    cout << message;

    message = "3- quit. ";
    gotoxy(midwayPointX - 20, midwayPointY + 4);
    SetConsoleTextAttribute(hand, redcolor);
    cout << message;
    gotoxy(midwayPointX - 20, midwayPointY + 5);

    SetConsoleTextAttribute(hand, defcolor);
    string option;
    string input;
    cin >> option;

    while (true) {
        if (option == "1") {
            gotoxy(midwayPointX - 20, midwayPointY + 8);
            cout << "Please enter a string: \n";
            cin >> input;
            return input;
        }

        else if (option == "2") {
            string filename = "input.txt";
            ifstream f(filename);
            SetConsoleTextAttribute(hand, graycolor);
            gotoxy(midwayPointX - 20, midwayPointY + 8);
            cout << "Reading from " << filename << ":\n\n";
            SetConsoleTextAttribute(hand, defcolor);
            if (!f.is_open()) {
                cerr << "Error opening the file!";
                exit(0);
            }
            string str((istreambuf_iterator<char>(f)),
                istreambuf_iterator<char>());
            cout << str;
            _getch();
            return str;
        }

        else if (option == "3") {
            exit(0);
        }
    }



}

int main() {
    string input;
    input = mainMenu();
    system("CLS");
    //string input = "Program Var \n x +; Start Print(x); End end";
    //string input;
    //cout << "Please enter your desired text: ";
    //cin >> input;
    size_t pos = 0;
    Token token;
    vector<Token> tokens;


    while (input.find("\\n") != string::npos)
        input.replace(input.find("\\n"), 3, "\n");

    //if ("Program Var \n x +; Start Print(x); End end;" == input) {
    //    cout << "AY!\n";
    //}

    int current_line = 1;
    SetConsoleTextAttribute(hand, yellowcolor);
    cout << "Input: \n\n" << "`\n";
    SetConsoleTextAttribute(hand, brightwhitecolor);
    cout << input << "\n\n";
    SetConsoleTextAttribute(hand, yellowcolor);
    cout << "`\n\n\n";
    cout << "Tokenized input:\n\n";
    SetConsoleTextAttribute(hand, defcolor);
    int text_color = yellowcolor;
    cout << " __________________________________________________________________________________________________\n";
    while ((token = getNextToken(input, pos, current_line)).type != UNKNOWN) {
        SetConsoleTextAttribute(hand, defcolor);
        cout << setw(10) << "| Token Type: " << setw(15);
        SetConsoleTextAttribute(hand, text_color);
        cout << getTokenName(token.type);
        SetConsoleTextAttribute(hand, defcolor);
        cout << "|" << setw(25) << "| Value: " << setw(15);
        SetConsoleTextAttribute(hand, text_color);
        cout << token.value + "|" << setw(25);
        SetConsoleTextAttribute(hand, defcolor);
        cout << "| Token line: " << setw(5);
        SetConsoleTextAttribute(hand, text_color);
        cout << to_string(token.line);
        SetConsoleTextAttribute(hand, defcolor);
        cout << "|" << endl;
        tokens.push_back(token);
    }
    cout << " --------------------------------------------------------------------------------------------------\n";

    //vector<Token> tokens = {
    //    {PROGRAM, "Program"}, {VAR, "Var"}, {IDENTIFIER, "x"}, {SEMICOLON, ";"}, {START, "Start"},
    //    {PRINT, "Print"}, {LPAREN, "("}, {IDENTIFIER, "x"}, {RPAREN, ")"}, {SEMICOLON, ";"},
    //    {END, "End"}, {PROGRAM_END, "end"}
    //};

    Parser parser(tokens, true);
    parser.parse(); // Call the S() function to start parsing

    return 0;
}
