#ifndef STRING_H
#define STRING_H

void int_to_str(int n, char str[]);
void int_to_hex_str(int n, char str[]);
void byte_converter(int n, char str[]); //used for displaying whole byte in hex
void reverse(char s[]);
int strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);

#endif