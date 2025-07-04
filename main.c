/*
    Name : Raghu G S
    Date : 11-06-2025
    Description: This project implements a Lexical Analyzer in C, which reads a C source file and breaks it down into
     meaningful tokens such as keywords, identifiers, operators, constants, and special symbols. It also identifies 
     comments and string literals while performing basic syntax error checks like unmatched brackets or quotes. The 
     lexer outputs each token along with its type, providing a foundation for further stages in compiler design. The 
     project uses modular C code and is executed through a simple command-line interface.
*/

#include <stdio.h>
#include <string.h> 
#include <unistd.h>
#include <stdlib.h>
#include "lexer.h"   // Custom lexer interface

int main(int argc, char *argv[]) 
{

    Token token;  // Token structure to hold each extracted token
    Count new;
    new.line = 0;
    new.bracket = 0;
    new.flower_brkt = 0;
    new.comment = 0;
    new.quotes = 0;

    // Array to map token types to their string representations for display
    const char* statusNames[] = {
        "PREPROCESSOR_DIRECTIVE", "KEYWORD", "OPERATOR", "SPECIAL_CHARACTER",
        "CONSTANT", "IDENTIFIER", "STRING_LITERAL", "VARIABLE", "COMMENT", "UNKNOWN"
    };

    // Check if file name argument is provided
    if(argc != 2)
    {
        printf("\n---------------------------------------");
        printf("\nERROR:\nUSAGE: ./a.out <file_name.c>");
        printf("\n---------------------------------------");
    }

    // Initialize lexer and get file pointer
    FILE* fptr = initializeLexer(&token, argv[1]);

    // Move to end of file to determine file length
    fseek(fptr, 0, SEEK_END);
    int file_length = ftell(fptr);

    // Check file for bracket/quote mismatches before proceeding
    //error_check(fptr, argv);

    // Reset file pointer to start of file
    rewind(fptr);

    // Loop until all tokens are read or EOF is reached
    while ((token = getNextToken(&new,fptr)).type != UNKNOWN) 
    {
        printf("----------------------------------------------------------------------------------------------------------------------------------\n");
        printf(" \tType : %-23s\t----> \tToken : %s \n", statusNames[token.type], token.lexeme);
        
        // Stop if we've reached or gone past end of file
        if(ftell(fptr) >= file_length)
            break;

        //sleep(1);  // Optional delay for visual output pacing
    }
    printf("----------------------------------------------------------------------------------------------------------------------------------\n\n");
    
    if(new.flower_brkt % 2 != 0)
    {
        printf("\n~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~~  X  ~~~~~~~~~~~~~~~~\n");
        printf("\nERROR: In line %d expecting '}' token\n\n",(new.line - 1));
        exit(0);
    }

    return 0;
}
