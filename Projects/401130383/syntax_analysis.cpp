#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <fstream>
using namespace std;


vector<pair<string, string>> token_rules = {
    {"KEYWORD", R"(^\s*\b(Program|end|Var|Start|End|Print|Read|if|Iteration|Put)\b)"},
    {"IDENTIFIER", R"(^\s*([a-zA-Z_]\w*))"},
    {"NUMBER", R"(^\s*\b(\d+)\b)"},
    {"OPERATOR", R"(^\s*(=|<|>|==|\+|-))"},
    {"PUNCTUATION", R"(^\s*(;|\(|\)|\{|\}))"},
    {"BWS", R"(^\s*)"}
}; 


struct Token {
    string value;
    string type;
    int line;
};


vector<Token> lexers(string code) {
    int position = 0;
    int line = 1;
    vector<Token> tokens;
    
    while (position < code.length()) {
        smatch match;
        bool matched = false;
        string sub = code.substr(position);

        for (int i = 0; i < token_rules.size(); i++) {
            regex pattern(token_rules[i].second);
            if (regex_search(sub, match, pattern)) {
                // Check line
                string matched_str = match[0].str();
                for (char c : matched_str) {
                    if (c == '\n') {
                        line++;  
                    }
                }
                // Add token
                if (token_rules[i].first != "BWS") {
                    tokens.push_back({match[1], token_rules[i].first, line});
                }
                // push position
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

// #######################################################################
/*Token program_t {"program", "keyword"};
Token var_t {"Var", "keyword"};*/

/*void sync(vector<string> followSet, bool type = false) {
    if (!type) {
        while (currentIndex < tokens.size()) {
            for (string follow : followSet) {
                if (tokens[currentIndex].value == follow) {
                    return;
                }
            }
            currentIndex ++;
        }
    }
    else {
        while (currentIndex < tokens.size()) {
            for (string follow : followSet) {
                if (tokens[currentIndex].type == follow) {
                    return;
                }
            }
            currentIndex ++;
        }
    }
}*/


/*struct Token {
    string value;
    string type;
    constructor
    Token(string v, string t) : value(v), type(t) {}
};*/


int currentIndex = 0;
vector<Token> tokens;
bool error = false;


void error_print(string message) {
    cout << "line " << tokens[currentIndex].line << ": " << message << " but it's <" << tokens[currentIndex].value << ">\n";
}


bool match(Token expected, string error_text) {
    if (expected.type != "NUMBER" && expected.type != "IDENTIFIER") {
        if (currentIndex < tokens.size() && tokens[currentIndex].value == expected.value) {
            //cout << tokens[currentIndex].line << ": " << endl; //
            currentIndex ++;
            return true;
        }
        error_print(error_text); //
        error = true;
        return false;
    }
    else {
        if (currentIndex < tokens.size() && tokens[currentIndex].type == expected.type) {
            //cout << tokens[currentIndex].line << ": " << endl; //
            currentIndex ++;
            return true;
        }
        error_print(error_text); //
        error = true;
        return false;
    }
}


void sync(vector<string> followSet) {
    while (currentIndex < tokens.size()) {
        for (string follow : followSet) {
            if ((tokens[currentIndex].value == follow) || (tokens[currentIndex].type == follow)) {
                return;
            }
        }
        if (currentIndex + 1 >= tokens.size()) return;
        currentIndex++;
    }
}


bool vars();
bool block();
bool states();
bool state();
bool m_states();
bool out();
bool in();
bool _if();
bool loop();
bool assign();
bool o();
bool expr();
bool expr_p();
bool r();


bool s() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression. s" << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Program") {
        match(Token{"Program", "KEYWORD"}, "it must be <Program>");
        if (!vars()){return false;}
        if (!block()) {return false;}
        match(Token{"end", "KEYWORD"}, "it must be <end>");
        return true;
    }
    
    else {
        error = true;
        error_print("Expected <Program> at the beginning");
        sync({"Program"});
        return true;
    }
}


bool vars() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Var") {
        match(Token{"Var", "KEYWORD"}, "it must be <Var>");
        match(Token{"", "IDENTIFIER"}, "it must be <IDENTIFIER>");
        match(Token{";", "PUNCTUATION"}, "it must be <;>");
        if (!vars()){return false;}
        
        return true;
    }

    else if (tokens[currentIndex].value == "Start"){
        return true;
    }

    else {
        error = true;
        //currentIndex ++;
        error_print("Expected 'Var' or 'Start");
        sync({"Start"});
        return true;
    }
}


bool block() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Start"){
        match(Token{"Start", "KEYWORD"}, "it must be <Start>");
        if (!states()) {return false;}
        match(Token{"End", "KEYWORD"}, "it must be <End>");
        
        return true;
    }
    else {
        error = true;
        vector<string> follow = {"If", "Read", "Print", "Put", "Iteration", "end", "End"};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        //currentIndex ++;
        error_print("Expected <Start> for block definition");
        sync({"If", "Read", "Print", "Put", "Iteration", "end", "End"});
        return true;
    }
}


bool states() {
    if (!state() || !m_states()) {return false;}
    return true;
    
}


bool state() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Start") {return block();}
        
    else if (tokens[currentIndex].value == "If") {return _if();}

    else if (tokens[currentIndex].value == "Read") {return in();}

    else if (tokens[currentIndex].value == "Print") {return out();}

    else if (tokens[currentIndex].value == "Put") {return assign();}

    else if (tokens[currentIndex].value == "Iteration") {return loop();}
    
    else {
        error = true;
        vector<string> follow = {"End"};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        //currentIndex ++;
        error_print("Unknown statement");
        sync({"End"});
        return true;
    }
}


bool m_states() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    string token = tokens[currentIndex].value; 
    
    if (token=="Start" || token=="If" || token=="Read" || token=="Print" || token=="Put" || token=="Iteration") {    
        return states();
    }
    
    else if (token == "End") {
        return true;
    }        

    else {
        error = true;
        /*vector<string> follow = {"End"};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }*/
        //currentIndex ++;
        error_print("Expected a statement or <End>");
        sync({"End"}); 
        return true;
    }
}


bool out() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Print") {
        match(Token{"Print", "KEYWORD"}, "it must be <Print>");
        match(Token{"(", "PUNCTUATION"}, "it must be <(>");
        if (!expr()) {return false;}
        match(Token{")", "PUNCTUATION"}, "it must be <)>");
        match(Token{";", "PUNCTUATION"}, "it must be <;>");
                            
        return true;
    }
    else {
        error = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ";"};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        //currentIndex ++;
        error_print("it must be <Print>");
        sync({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}


bool in() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Read") {
        match(Token{"Read", "KEYWORD"}, "it must be <Read>");
        match(Token{"(", "PUNCTUATION"}, "it must be <(>");
        match(Token{"", "IDENTIFIER"}, "it must be <IDENTIFIER>");
        match(Token{")", "PUNCTUATION"}, "it must be <)>");
        match(Token{";", "PUNCTUATION"}, "it must be <;>");
        return true;
    }

    else {
        error = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ";"};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        error_print("it must be <Read>");
        //currentIndex ++;
        sync({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}


bool _if() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "If") {
        match(Token{"If", "KEYWORD"}, "it must be <If>");
        match(Token{"(", "PUNCTUATION"}, "it must be <(>");
        if (!expr()) {return false;}
        if (!o()) {return false;}
        if (!expr()) {return false;}
        match(Token{")", "PUNCTUATION"}, "it must be <)>");
        match(Token{"{", "PUNCTUATION"}, "it must be <{>");
        if (!state()) {return false;}
        match(Token{"}", "PUNCTUATION"}, "it must be <}>");

        return true;
    }

    else {
        error = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ""};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        error_print("it must be <If>");
        //currentIndex ++;
        sync({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}


bool loop() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Iteration") {
        match(Token{"Iteration", "KEYWORD"}, "it must be <Iteration>");
        match(Token{"(", "PUNCTUATION"}, "it must be <(>");
        if (!expr()) {return false;}
        if (!o()) {return false;}
        if (!expr()) {return false;}
        match(Token{")", "PUNCTUATION"}, "it must be <)>");
        match(Token{"{", "PUNCTUATION"}, "it must be <{>");
        if (!state()) {return false;}
        match(Token{"}", "PUNCTUATION"}, "it must be <}>");
        return true;
    }
    
    else {
        error = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ""};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        error_print("it must be <Iteration>");
        //currentIndex ++;
        sync({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}


bool assign() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "Put") {
        match(Token{"Put", "KEYWORD"}, "it must be <Put>");
        match(Token{"", "IDENTIFIER"}, "it must be <IDENTIFIER>");
        match(Token{"=", "OPERATOR"}, "it must be <=>");
        if (!expr()) {return false;}
        match(Token{";", "PUNCTUATION"}, "it must be <;>");
        return true;
        }

    else {
        error = true;
        vector<string> follow = {"Start", "If", "Read", "Print", "Put", "Iteration", "End", ";"};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        error_print("it must be <Put>");
        //currentIndex ++;
        sync({"Start", "If", "Read", "Print", "Put", "Iteration", "End"});
        return true;
    }
}



bool o() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].value == "<") {
        match(Token{"<", "OPERATOR"}, "it must be <OPERATOR>");
        return true;
    }
    
    else if (tokens[currentIndex].value == ">") {
        match(Token{">", "OPERATOR"}, "it must be <OPERATOR>");
        return true;
    }
    
    else if (tokens[currentIndex].value == "==") {
        match(Token{"==", "OPERATOR"}, "it must be <OPERATOR>");
        return true;
    }
        //return true;

    else {
        error = true;
        vector<string> follow = {"NUMBER", "IDENTIFIER"};
        for (string f : follow) {
            if (tokens[currentIndex].type == f) {
                return true;
            }
        }
        error_print("it must be <OPERATOR>");
        //currentIndex ++;
        error_print("it must be <OPERATOR>");
        sync({"NUMBER", "IDENTIFIER"});
        return true;
    }
}


bool expr() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].type == "NUMBER" || tokens[currentIndex].type == "IDENTIFIER") {
        return r() && expr_p();
    }

    else {
        error = true;
        vector<string> follow = {";", ")", "<", ">", "=="};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        error_print("Expected an expression");
        //currentIndex ++;
        sync({";", ")", "<", ">", "=="});
        return true;
    }
}


bool expr_p() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    string token = tokens[currentIndex].value;
    if (token == "+") {
        match(Token{"+", "OPERATOR"}, "");
        if (!r()) {return false;}
        if (!expr_p()){return false;}
        
        return true;         
    }
        
    else if (token == "-") {
        match(Token{"-", "OPERATOR"}, "");
        if (!r()) {return false;}
        if (!expr_p()){return false;}
        
        return true;
    }
    
    else if (token==")" || token==";" || token=="<" || token==">" || token=="==") {
        return true; 
    }

    else {
        error = true;
        /*vector<string> follow = {";", ")", "<", ">", "=="};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }*/
        //currentIndex ++;
        error_print("Expected <+>, <->, or end of expression");
        error_print("it must be <OPERATOR>");
        sync({";", ")", "<", ">", "=="});
        return true;
    }
}


bool r() {
    if (currentIndex >= tokens.size()) {
        cout << "Error: Unexpected end of expression." << endl;
        error = true;
        return false;
    }

    if (tokens[currentIndex].type == "NUMBER") {
        match(Token{"", "NUMBER"}, "");
        return true;
    }
    
    else if (tokens[currentIndex].type == "IDENTIFIER") {
        match(Token{"", "IDENTIFIER"}, "");
        return true;
    }
    
    else {
        error = true;
        vector<string> follow = {";", ")", "+", "-", "<", ">", "=="};
        for (string f : follow) {
            if (tokens[currentIndex].value == f) {
                return true;
            }
        }
        //currentIndex ++;
        error_print("it must be <NUMBER> or <IDENTIFIER>");
        sync({";", ")", "+", "-", "<", ">", "=="});
        return true;
    }
}


int main() {
    string code, temp;
    ifstream codefile("code.txt");
    while (getline(codefile, temp)) {
        code += temp + " \n";
    }
    cout << "\n\n---------- Lexical Analyzer(Lexer) ----------\n\n";    
    tokens = lexers(code);
    for (Token token : tokens) {
        cout << token.line << ". " << token.value << " - " << token.type << endl;
    }
    cout << "\n\n---------- Syntactic Analyzer(Parser) ----------\n\n",
    s();
    cout <<"Error: " << error << '\n';

    return 0;
}
