#ifndef MY_STRING_H_INCLUDED
#define MY_STRING_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int string_length(const char *s);
bool is_letter(char c);
void string_copy(char *s1, const char *s2);
void string_append(char *s1, const char *s2);
char *add_char_to_string(char **s, char *s_begin, size_t *len,  size_t *lenmax, char c);
char *concat(const char *s1, const char *s2);
char *get_line(FILE *in);
bool compare(const char *s1, const char *s2);
char *substring(const char *s, int b, int l);
char *subword(char **s);
char *split_by_last_slash(char **address);
char *delete_comments(char *in, bool *multiline_comment);


#endif // MY_STRING_H_INCLUDED
