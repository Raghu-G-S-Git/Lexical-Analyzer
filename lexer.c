#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "lexer.h"

// Define known keywords for C-like language
static const char* keywords[MAX_KEYWORDS] = {
    "int", "float", "return", "if", "else", "while", "for", "do", "break", "continue",
    "char", "double", "void", "switch", "case", "default", "const", "static", "sizeof", "struct",
    "short", "signed", "unsigned", "enum", "union", "long", "typedef"
};

// Define some known identifiers (commonly used standard functions)
static const char* identifier[MAX_IDENTIFIER] = {
    "main","printf","scanf"
};

// Define basic operators
static const char* operators[MAX_OPERATOR] = {"+","-","*","/","%","=","!","<",">","|","&","^"};

// Define special characters used in C-like syntax
static const char* specialCharacters[MAX_SPECIAL_CHARACTER] = {",",";","{","}","(",")","[","]"};

// Open the file and return a file pointer, print error and exit if file not found
FILE* initializeLexer(Token *token, const char* filename)
{
    FILE *fileptr = fopen(filename, "r");
    if(fileptr == NULL)
    {
        printf("UNABLE TO OPEN %s FILE\n", filename);
        exit(0);
    }
    return fileptr;
}

// Reads the next token from the file and returns it
Token getNextToken(Count *new, FILE *fptr)
{
    Token mytoken;
    
    int i = 0;
    char ch;

    // Skip whitespace characters (space, tab, newline, carriage return)
    do {
        
        ch = fgetc(fptr);
        mytoken.lexeme[0] = ch;
        if(ch == '\n')
        {   
            new->line++;
            error_check(new, fptr);
        }
            

    } while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    i++;

    // Handle single-line (//) or multi-line (/* */) comments
    if(mytoken.lexeme[0] == '/' && ((mytoken.lexeme[i] = fgetc(fptr)) == '/' || mytoken.lexeme[i] == '*'))
    {
        if(mytoken.lexeme[i] == '/')
        {
            new->comment++;
            i++;
            while((ch = fgetc(fptr)) != '\n')  // read until newline
            {
                mytoken.lexeme[i++] = ch;
                if(ch == '\n')
                    new->line++;
            }
            fseek(fptr,-1,SEEK_CUR);
            mytoken.lexeme[i] = '\0';
            mytoken.type = COMMENT;
            return mytoken;
        }
        else if(mytoken.lexeme[i] == '*')
        {
            i++;
            while((ch = fgetc(fptr)) != '/')  // read until closing '/'
            {
                if(ch == EOF)
                    break;
                mytoken.lexeme[i++] = ch;
                if(ch == '\n')
                    new->line++;
            }
            mytoken.lexeme[i] = ch;  // add final '/'
            mytoken.lexeme[i+1] = '\0';
            mytoken.type = COMMENT;
            return mytoken;
        }
    }

    // Handle preprocessor directives like #include, #define, etc.
    else if (mytoken.lexeme[0] == '#')
    {
        while((ch = fgetc(fptr)) != '\n')
        {
            mytoken.lexeme[i++] = ch;
        }
        new->line++;
        fseek(fptr,-1,SEEK_CUR);
        mytoken.lexeme[i] = '\0';
        mytoken.type = PREPROCESSOR_DIRECTIVE;
        return mytoken;
    }

    // Handle keywords, identifiers, and variables
    else if(isalpha(ch) || ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n')
    {
        while((ch = fgetc(fptr)) != ' ')
        {
            if(!isalnum(ch) && ch != '_')
            {
                fseek(fptr, -1, SEEK_CUR);  // unread non-alnum character
                break;
            }
            mytoken.lexeme[i++] = ch;
        }
        mytoken.lexeme[i] = '\0';
        mytoken = categorizeToken(new, mytoken);
        return mytoken;
    }

    // Handle string literals and single special characters/operators
    else if(!isalnum(ch))
    {
        
        if(ch != '"')
        {
            if(ch == '\'')
            {
                new->quotes++;
                while((ch = fgetc(fptr)) != ' ')
                {
                    if(ch == '\'')
                    {
                        new->quotes++;
                        
                        mytoken.lexeme[i++] = ch;
                        
                        break;
                    }
                    
                    mytoken.lexeme[i++] = ch;
                }
                mytoken.lexeme[i] = '\0';
                mytoken.type = VARIABLE;
                if(new->quotes % 2 != 0)
                {
                    printf("\n~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~\n");
                    printf("\nERROR: In line %d expecting '\'' token\n\n",new->line);
                    exit(0);
                }
                new->quotes = 0;
                return mytoken;
            }
            mytoken.lexeme[1] = '\0';  // single char lexeme
            mytoken = categorizeToken(new, mytoken);
        }
        else  // handle string literal
        {
            new->quotes++;
            while((ch = fgetc(fptr)) != ')')
            {
                if(ch == '"')
                {
                    new->quotes++;
                    
                    mytoken.lexeme[i++] = ch;
                    
                    break;
                }
                    
                mytoken.lexeme[i++] = ch;
            }
            
            //mytoken.lexeme[i++] = '"';  // include closing quote
            mytoken.lexeme[i] = '\0';
            mytoken.type = STRING_LITERAL;

            //fseek(fptr, -1, SEEK_CUR);
            
            if(new->quotes % 2 != 0)
            {
                printf("\n~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~\n");
                printf("\nERROR: In line %d expecting '\"' token\n\n",new->line);
                exit(0);
            }
            new->quotes = 0;
        }
        return mytoken;
    }

    // Handle numeric constants
    else if(isdigit(ch))
    {
        while((ch = fgetc(fptr)) >= '0' && ch <= '9' || ch =='.')
        {
            mytoken.lexeme[i++] = ch;
        }
        mytoken.lexeme[i] = '\0';
        fseek(fptr, -1, SEEK_CUR);  // unread the non-digit
        mytoken.type = CONSTANT;
        return mytoken;
    }

    // If nothing matches, mark it as UNKNOWN
    else
    {
        mytoken.type = UNKNOWN;
        return mytoken;
    }
}

// Determine the type of a token based on its value
Token categorizeToken(Count *new, Token token)
{
    if(isKeyword(token.lexeme)) token.type = KEYWORD;
    else if(isOperator(token.lexeme)) token.type = OPERATOR;
    else if(isSpecialCharacter(new, token.lexeme)) token.type = SPECIAL_CHARACTER;
    else if(isIdentifier(token.lexeme)) token.type = IDENTIFIER;
    else token.type = VARIABLE;
    return token;
}

// Check if the token is a keyword
int isKeyword(const char* str)
{
    for(int i = 0; i < MAX_KEYWORDS; i++)
    {
        if(strcmp(str, keywords[i]) == 0)
            return 1;
    }
    return 0;
}

// Check if the token is an operator
int isOperator(const char* str)
{
    for(int i = 0; i < MAX_OPERATOR; i++)
    {
        if(strcmp(str, operators[i]) == 0)
            return 1;
    }
    return 0;
}

// Check if the token is a special character
int isSpecialCharacter(Count *new, const char* str)
{
    for(int i = 0; i < MAX_SPECIAL_CHARACTER; i++)
    {
        if(strcmp(str, specialCharacters[i]) == 0)
        {
            if(str[0] == '(' || str[0] == ')')
                new->bracket++;
            if(str[0] == '{' || str[0] == '}')
                new->flower_brkt++;
            return 1;
        }
    }
    return 0;
}

// Check if the token is a predefined identifier
int isIdentifier(const char* str)
{
    for(int i = 0; i < MAX_IDENTIFIER; i++)
    {
        if(strcmp(str, identifier[i]) == 0)
            return 1;
    }
    return 0;
}

void error_check(Count *new, FILE *fptr)
{
    char ch;
    fseek(fptr,-3,SEEK_CUR);
    ch = fgetc(fptr);
    
    //printf("cmt -> %d\n",new->comment);
    
    
    if(ch != ';' && (new->comment != 1))
    {
        
        //fseek(fptr,2,SEEK_CUR);
        //ch = fgetc(fptr);
        //printf(" 2-> %c\n",ch);
        if(ch == '{' || ch =='\n' || ch == '/' || ch == '>' )
        {
            fseek(fptr,2,SEEK_CUR);
            return;
        }
        fseek(fptr,2,SEEK_CUR);
        ch = fgetc(fptr);
        if(ch == '{' || ch == '\t' )
        {
            fseek(fptr,-1,SEEK_CUR);
            return;
        }
        
        
        printf("\n~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~\n");
        printf("\nERROR: In line %d expecting ';' token\n\n",(new->line - 1));
        exit(0);
    }
    else
    {
        fseek(fptr, 2, SEEK_CUR);
        if(ch = fgetc(fptr) == '}')
        {
            fseek(fptr, -1, SEEK_CUR);
            return;
        }    
    }
    if(new->bracket % 2 != 0)
    {
        printf("\n~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~\n");
        printf("\nERROR: In line %d expecting ')' token\n\n",(new->line - 1));
        exit(0);
    }
    new->comment = 0;

    fseek(fptr, 3, SEEK_CUR);

    return;
    
}
