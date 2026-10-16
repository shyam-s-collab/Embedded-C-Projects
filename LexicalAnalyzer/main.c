#include <stdio.h>
#include "lexer.h"
int error_flag;
int main(int argc, char* argv[])
{
    if(argc<2)
    {
        printf("Usage: ./a.out <.c file>\n");
        return 0;
    }
    char filename[50];
    strcpy(filename,argv[1]);
    if(!strstr(filename, ".c"))
    {
        printf("Error: Pass .c File\n");
        return 0;
    }
    FILE* fptr=fopen(filename,"r");
    printf("open: %s", filename);
    if(fptr)
    {
        printf("File was opened successfully:\n");
    }
    else
    {
        printf("Failed to open file :");
        return 0;
    }
    printf("Parsing %s Started",filename);
    lexical_analyze(fptr);
    fclose(fptr);
    if(error_flag)
    {
        printf("Tokens cannot be generated\n");
    }
    else
    {
        printf("Tokens generated\n");
    }
    return 0;
}