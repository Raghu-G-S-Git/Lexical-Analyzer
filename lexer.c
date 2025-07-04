#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "lexer.h"

// Function declarations to avoid implicit declaration errors
Token categorizeToken(Count *new, Token token);
int isKeyword(const char* str);
int isOperator(const char* str);
int isSpecialCharacter(Count *new, const char* str);
int isIdentifier(const char* str);

// Define known keywords for C-like language
static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct",
    "short", "signed", "unsigned", "enum", "union", "long", "typedef"
};

// Define some known identifiers (commonly used standard functions)
static const char* identifier[MAX_IDENTIFIER] = {
    "main", "printf", "scanf"
};

// Define basic operators
static const char* operators[MAX_OPERATOR] = {"+", "-", "*", "/", "%", "=", "!", "<", ">", "|", "&", "^"};

// Define special characters used in C-like syntax
static const char* specialCharacters[MAX_SPECIAL_CHARACTER] = {",", ";", "{", "}", "(", ")", "[", "]"};

// Open the file and return a file pointer, print error and exit if file not found
FILE* initializeLexer(Token *token, const char* filename) 
{
    FILE *fileptr = fopen(filename, "r");
    if (fileptr == NULL) 
    {
        printf("UNABLE TO OPEN %s FILE\n", filename);
        exit(1);
    }
    return fileptr;
}

// Reads the next token from the file and returns it
Token getNextToken(Count *new, FILE *fptr) 
{
    Token mytoken;
    mytoken.lexeme[0] = '\0';  // Initialize lexeme to empty string
    mytoken.type = UNKNOWN;
    int i = 0;
    char ch;

    // Skip whitespace characters (space, tab, newline, carriage return)
    while ((ch = fgetc(fptr)) != EOF && (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')) 
    {
        if (ch == '\n') 
        {
            new->line++;
        }
    }

    if (ch == EOF) 
    {
        mytoken.type = UNKNOWN;
        mytoken.lexeme[0] = '\0';
        return mytoken;
    }

    mytoken.lexeme[i++] = ch;  // Store first character

    // Handle single-line (//) or multi-line (/* */) comments
    if (ch == '/' && i < MAX_TOKEN_SIZE - 1) 
    {
        char next_ch = fgetc(fptr);
        if (next_ch == '/') 
        {
            mytoken.lexeme[i++] = next_ch;  // Include second '/' in lexeme
            new->comment++;
            while ((ch = fgetc(fptr)) != '\n' && ch != EOF && i < MAX_TOKEN_SIZE - 1) 
            {
                mytoken.lexeme[i++] = ch;
            }
            if (ch == '\n') 
            {
                new->line++;
                fseek(fptr, -1, SEEK_CUR);  // Unread newline
            }
            mytoken.lexeme[i] = '\0';
            mytoken.type = COMMENT;
            return mytoken;
        } 
        else if (next_ch == '*') 
        {
            mytoken.lexeme[i++] = next_ch;
            while (i < MAX_TOKEN_SIZE - 1) 
            {
                ch = fgetc(fptr);
                if (ch == EOF) break;
                mytoken.lexeme[i++] = ch;
                if (ch == '\n') new->line++;
                if (ch == '*' && (ch = fgetc(fptr)) == '/') 
                {
                    if (i < MAX_TOKEN_SIZE - 1) mytoken.lexeme[i++] = ch;
                    break;
                } 
                else 
                {
                    fseek(fptr, -1, SEEK_CUR);
                }
            }
            mytoken.lexeme[i] = '\0';
            mytoken.type = COMMENT;
            return mytoken;
        } 
        else 
        {
            fseek(fptr, -1, SEEK_CUR);  // Unread next_ch
        }
    }

    // Handle preprocessor directives like #include, #define, etc.
    if (ch == '#') 
    {
        while ((ch = fgetc(fptr)) != '\n' && ch != EOF && i < MAX_TOKEN_SIZE - 1) 
        {
            mytoken.lexeme[i++] = ch;
        }
        if (ch == '\n') 
        {
            new->line++;
            fseek(fptr, -1, SEEK_CUR);
        }
        mytoken.lexeme[i] = '\0';
        mytoken.type = PREPROCESSOR_DIRECTIVE;
        return mytoken;
    }

    // Handle keywords, identifiers, and variables
    if (isalpha(ch) || ch == '_') 
    {
        while ((ch = fgetc(fptr)) != EOF && (isalnum(ch) || ch == '_') && i < MAX_TOKEN_SIZE - 1) 
        {
            mytoken.lexeme[i++] = ch;
        }
        if (ch != EOF) 
        {
            fseek(fptr, -1, SEEK_CUR);  // Unread non-valid character
        }
        mytoken.lexeme[i] = '\0';
        mytoken = categorizeToken(new, mytoken);
        return mytoken;
    }

    // Handle string literals and single quotes
    if (ch == '"' || ch == '\'') 
    {
        char quote_type = ch;
        new->quotes++;
        while ((ch = fgetc(fptr)) != EOF && i < MAX_TOKEN_SIZE - 1) 
        {
            if (ch == '\\') 
            {  // Handle escaped characters
                mytoken.lexeme[i++] = ch;
                ch = fgetc(fptr);
                if (ch != EOF && i < MAX_TOKEN_SIZE - 1) 
                {
                    mytoken.lexeme[i++] = ch;
                    continue;
                } else if (ch == EOF) 
                {
                    mytoken.lexeme[i] = '\0';
                    fprintf(stderr, "ERROR: Unexpected EOF in string literal at line %d\n", new->line);
                    exit(1);
                }
            }
            if (ch == quote_type) 
            {
                mytoken.lexeme[i++] = ch;
                new->quotes++;
                break;
            }
            mytoken.lexeme[i++] = ch;
        }
        mytoken.lexeme[i] = '\0';
        mytoken.type = (quote_type == '"') ? STRING_LITERAL : VARIABLE;
        if (new->quotes % 2 != 0) 
        {
            fprintf(stderr, "ERROR: In line %d expecting '%c' token\n", new->line, quote_type);
            exit(1);
        }
        new->quotes = 0;
        return mytoken;
    }

    // Handle numeric constants
    if (isdigit(ch) || (ch == '.' && isdigit(fgetc(fptr)))) 
    {
        if (ch == '.') 
        {
            fseek(fptr, -1, SEEK_CUR);  // Unread digit
            mytoken.lexeme[0] = '0';    // Handle .5 as 0.5
        }
        while ((ch = fgetc(fptr)) != EOF && (isdigit(ch) || ch == '.') && i < MAX_TOKEN_SIZE - 1) 
        {
            mytoken.lexeme[i++] = ch;
        }
        if (ch != EOF) 
        {
            fseek(fptr, -1, SEEK_CUR);  // Unread non-digit
        }
        mytoken.lexeme[i] = '\0';
        mytoken.type = CONSTANT;
        return mytoken;
    }

    // Handle single-character operators or special characters
    mytoken.lexeme[i] = '\0';
    mytoken = categorizeToken(new, mytoken);
    return mytoken;
}

// Determine the type of a token based on its value
Token categorizeToken(Count *new, Token token) 
{
    if (isKeyword(token.lexeme)) token.type = KEYWORD;
    else if (isOperator(token.lexeme)) token.type = OPERATOR;
    else if (isSpecialCharacter(new, token.lexeme)) token.type = SPECIAL_CHARACTER;
    else if (isIdentifier(token.lexeme)) token.type = IDENTIFIER;
    else token.type = VARIABLE;
    return token;
}

// Check if the token is a keyword
int isKeyword(const char* str) 
{
    for (int i = 0; i < MAX_KEYWORDS; i++) 
    {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

// Check if the token is an operator
int isOperator(const char* str) {
    for (int i = 0; i < MAX_OPERATOR; i++) 
    {
        if (strcmp(str, operators[i]) == 0) return 1;
    }
    return 0;
}

// Check if the token is a special character
int isSpecialCharacter(Count *new, const char* str) 
{
    for (int i = 0; i < MAX_SPECIAL_CHARACTER; i++) 
    {
        if (strcmp(str, specialCharacters[i]) == 0) 
        {
            if (str[0] == '(') new->bracket++;
            else if (str[0] == ')') new->bracket--;
            else if (str[0] == '{') new->flower_brkt++;
            else if (str[0] == '}') new->flower_brkt--;
            return 1;
        }
    }
    return 0;
}

// Check if the token is a predefined identifier
int isIdentifier(const char* str) 
{
    for (int i = 0; i < MAX_IDENTIFIER; i++) 
    {
        if (strcmp(str, identifier[i]) == 0) return 1;
    }
    return 0;
}
