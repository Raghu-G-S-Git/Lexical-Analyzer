#ifndef LEXER_H
#define LEXER_H

// Maximum limits for different token types
#define MAX_KEYWORDS 27                  // Total number of recognized keywords
#define MAX_IDENTIFIER 3                // Recognized standard identifiers like 'main', 'printf', etc.
#define MAX_OPERATOR 12                 // Supported operators like +, -, *, etc.
#define MAX_SPECIAL_CHARACTER 8         // Special characters like (), {}, ;, etc.
#define MAX_TOKEN_SIZE 100              // Maximum length of a token (identifier, keyword, etc.)

// Enum to represent all possible types of tokens a lexer can return
typedef enum {
    PREPROCESSOR_DIRECTIVE,    // e.g., #include <stdio.h>
    KEYWORD,                   // e.g., int, float, if, else, etc.
    OPERATOR,                  // e.g., +, -, =, *, etc.
    SPECIAL_CHARACTER,         // e.g., (, ), {, }, ;, etc.
    CONSTANT,                  // e.g., numeric values like 123
    IDENTIFIER,                // e.g., main, printf, scanf
    STRING_LITERAL,            // e.g., "Hello, World!"
    VARIABLE,                  // user-defined variable names
    COMMENT,                   // // or /* */ style comments
    UNKNOWN                    // Any unrecognized or malformed token
} TokenType;

// Token structure containing the actual lexeme and its type
typedef struct 
{
    char lexeme[MAX_TOKEN_SIZE];  // Actual string of the token
    TokenType type;               // Type of the token
} Token;

typedef struct 
{
    int line ;
    int bracket;
    int flower_brkt;
    int comment;
    int quotes;
} Count;


// Function declarations used in lexer.c
FILE* initializeLexer(Token *token, const char* filename);   // Opens file and checks validity
Token getNextToken(Count *new, FILE *fptr);                              // Reads next token from file
Token categorizeToken(Count *new, Token token);                          // Assigns type to the token based on content

// Helper functions to classify token types
int isKeyword(const char* str);
int isOperator(const char* str);
int isSpecialCharacter(Count *new, const char* str);
int isIdentifier(const char* str);

void error_check(Count *new, FILE *fptr);                  // Checks for syntax mismatches in input file

#endif
