#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

typedef enum {
    TOKEN_VAR,
    TOKEN_IDENTIFIER,
    TOKEN_SEMICOLON,
    TOKEN_START,
    TOKEN_END,
    TOKEN_PRINT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_READ,
    TOKEN_IF,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_ITERATION,
    TOKEN_PUT,
    TOKEN_EQ,
    TOKEN_LT, // <
    TOKEN_GT, // >
    TOKEN_EQEQ, // ==
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_TIMES,
    TOKEN_INTEGER,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

typedef struct {
    TokenType type;
    char value[100];
} Token;

Token getNextToken(char *input, int *index) {
    Token token;
    int i = *index;

    // Skip whitespace
    while (isspace(input[i])) {
        i++;
    }

    if (isalpha(input[i])) {
        int j = 0;
        while (isalnum(input[i])) {
            token.value[j++] = input[i++];
        }
        token.value[j] = '\0';
        if (strcmp(token.value, "Var") == 0) {
            token.type = TOKEN_VAR;
        } else if (strcmp(token.value, "Start") == 0) {
            token.type = TOKEN_START;
        } else if (strcmp(token.value, "End") == 0) {
            token.type = TOKEN_END;
        } else if (strcmp(token.value, "Print") == 0) {
            token.type = TOKEN_PRINT;
        } else if (strcmp(token.value, "Read") == 0) {
            token.type = TOKEN_READ;
        } else if (strcmp(token.value, "If") == 0) {
            token.type = TOKEN_IF;
        } else if (strcmp(token.value, "Iteration") == 0) {
            token.type = TOKEN_ITERATION;
        } else if (strcmp(token.value, "Put") == 0) {
            token.type = TOKEN_PUT;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
    } else if (isdigit(input[i])) {
        int j = 0;
        while (isdigit(input[i])) {
            token.value[j++] = input[i++];
        }
        token.value[j] = '\0';
        token.type = TOKEN_INTEGER;
    } else if (input[i] == ';') {
        token.type = TOKEN_SEMICOLON;
        token.value[0] = ';';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '(') {
        token.type = TOKEN_LPAREN;
        token.value[0] = '(';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == ')') {
        token.type = TOKEN_RPAREN;
        token.value[0] = ')';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '{') {
        token.type = TOKEN_LBRACE;
        token.value[0] = '{';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '}') {
        token.type = TOKEN_RBRACE;
        token.value[0] = '}';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '=') {
        if (input[i + 1] == '=') {
            token.type = TOKEN_EQEQ;
            token.value[0] = '=';
            token.value[1] = '=';
            token.value[2] = '\0';
            i += 2;
        } else {
            token.type = TOKEN_EQ;
            token.value[0] = '=';
            token.value[1] = '\0';
            i++;
        }
    } else if (input[i] == '<') {
        token.type = TOKEN_LT;
        token.value[0] = '<';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '>') {
        token.type = TOKEN_GT;
        token.value[0] = '>';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '+') {
        token.type = TOKEN_PLUS;
        token.value[0] = '+';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '-') {
        token.type = TOKEN_MINUS;
        token.value[0] = '-';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '*') {
        token.type = TOKEN_TIMES;
        token.value[0] = '*';
        token.value[1] = '\0';
        i++;
    } else if (input[i] == '\0') {
        token.type = TOKEN_EOF;
        token.value[0] = '\0';
    } else {
        token.type = TOKEN_ERROR;
        sprintf(token.value, "Invalid character: %c", input[i]);
        i++;
    }

    *index = i;
    return token;
}

Token currentToken;

void getNextTokenWrapper(char *input, int *index) {
    currentToken = getNextToken(input, index);
}

void match(TokenType expectedType, char *input, int *index) {
    if (currentToken.type == expectedType) {
        getNextTokenWrapper(input, index);
    } else {
        fprintf(stderr, "Syntax error: Expected %d, but got %d\n", expectedType, currentToken.type);
        exit(1);
    }
}

void program(char *input, int *index);
void statement_list(char *input, int *index);
void statement(char *input, int *index);
void expression(char *input, int *index);
void condition(char *input, int *index);

void program(char *input, int *index) {
    while (currentToken.type == TOKEN_VAR) {
        match(TOKEN_VAR, input, index);
        match(TOKEN_IDENTIFIER, input, index);
        match(TOKEN_SEMICOLON, input, index);
    }
    match(TOKEN_START, input, index);
    statement_list(input, index);
    match(TOKEN_END, input, index);
}

void statement_list(char *input, int *index) {
    if (currentToken.type == TOKEN_PRINT ||
        currentToken.type == TOKEN_READ ||
        currentToken.type == TOKEN_IF ||
        currentToken.type == TOKEN_ITERATION ||
        currentToken.type == TOKEN_PUT) {
        statement(input, index);
        statement_list(input, index);
    }
    // Epsilon production: do nothing
}

void statement(char *input, int *index) {
    if (currentToken.type == TOKEN_PRINT) {
        match(TOKEN_PRINT, input, index);
        match(TOKEN_LPAREN, input, index);
        expression(input, index);
        match(TOKEN_RPAREN, input, index);
        match(TOKEN_SEMICOLON, input, index);
    } else if (currentToken.type == TOKEN_READ) {
        match(TOKEN_READ, input, index);
        match(TOKEN_LPAREN, input, index);
        match(TOKEN_IDENTIFIER, input, index);
        match(TOKEN_RPAREN, input, index);
        match(TOKEN_SEMICOLON, input, index);
    } else if (currentToken.type == TOKEN_PUT) {
        match(TOKEN_PUT, input, index);
        match(TOKEN_IDENTIFIER, input, index);
        match(TOKEN_EQ, input, index);
        expression(input, index);
        match(TOKEN_SEMICOLON, input, index);
    } else if (currentToken.type == TOKEN_IF) {
        match(TOKEN_IF, input, index);
        match(TOKEN_LPAREN, input, index);
        condition(input, index);
        match(TOKEN_RPAREN, input, index);
        match(TOKEN_LBRACE, input, index);
        statement_list(input, index);
        match(TOKEN_RBRACE, input, index);
    } else if (currentToken.type == TOKEN_ITERATION) {
        match(TOKEN_ITERATION, input, index);
        match(TOKEN_LPAREN, input, index);
        condition(input, index);
        match(TOKEN_RPAREN, input, index);
        match(TOKEN_LBRACE, input, index);
        statement_list(input, index);
        match(TOKEN_RBRACE, input, index);
    } else {
        fprintf(stderr, "Syntax error: Unexpected token %d\n", currentToken.type);
        exit(1);
    }
}

void expression(char *input, int *index) {
    // Simplified expression parsing for demonstration purposes
    if (currentToken.type == TOKEN_IDENTIFIER || currentToken.type == TOKEN_INTEGER) {
        getNextTokenWrapper(input, index);
        if (currentToken.type == TOKEN_PLUS || currentToken.type == TOKEN_MINUS || currentToken.type == TOKEN_TIMES) {
            getNextTokenWrapper(input, index);
            expression(input, index);
        }
    } else {
        fprintf(stderr, "Syntax error in expression\n");
        exit(1);
    }
}

void condition(char *input, int *index) {
    expression(input, index);
    if (currentToken.type == TOKEN_LT || currentToken.type == TOKEN_GT || currentToken.type == TOKEN_EQEQ) {
        getNextTokenWrapper(input, index);
        expression(input, index);
    } else {
        fprintf(stderr, "Syntax error in condition\n");
        exit(1);
    }
}

int main() {
    char input[1024];
    int index = 0;

    printf("Enter your code (end with 'End'):\n");
    while (fgets(input + index, sizeof(input) - index, stdin)) {
        index += strlen(input + index);
        if (strstr(input, "End") != NULL) {
            break;
        }
    }

    index = 0;
    currentToken.type = TOKEN_ERROR; // Initialize currentToken to a safe default
    getNextTokenWrapper(input, &index);
    program(input, &index);
    printf("Parsing successful!\n");
    return 0;
}