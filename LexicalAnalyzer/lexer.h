#ifndef HEADER_H
#define HEADER_H

#include<stdio.h>
#include<string.h>

void lexical_analyze(FILE* fptr);
void check_type(char* token);
int is_keyword(char* token);
int is_digit(char* token);
int is_special(char ch);
int is_operator(char token);
int is_assignment(char ch);
int is_symbol(char ch);
int is_octal(char* number);
int is_bin(char* number);
int is_hexa(char* number);


#endif
