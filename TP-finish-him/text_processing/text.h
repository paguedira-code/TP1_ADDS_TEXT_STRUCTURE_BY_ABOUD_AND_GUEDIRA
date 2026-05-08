#ifndef TEXT_H
#define TEXT_H
#include "../structures.h"

int    is_delimiter       (char c);
int    is_sentence_end    (char c);
int    is_paragraph_break (char* s, int i);
int    is_end_of_string   (char* s, int i);
int    skip_delimiters    (char* s, int i, int* np, int* sf);
int    collect_word       (char* s, int i);
void   detect_newpara     (char* s, int i, char** nxt, int* np);
char*  separate_words     (char** str, int* np, int* sf);
void lowercase_string(char *str);

#endif