#ifndef MY_STRING_H_INCLUDED
#define MY_STRING_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int string_length(const char *s);
void string_copy(char *s1, const char *s2);
void string_append(char *s1, const char *s2);
char *add_char_to_string(char **s, char *s_begin, size_t *len,  size_t *lenmax, char c);
char *concat(const char *s1, const char *s2);
char *get_line(FILE *in);
char *substring(const char *s, int b, int l);
char *subword(char **s);
bool is_letter(char c);
bool compare(const char *s1, const char *s2);
char *split_by_last_slash(char **address);
char *delete_comments(char *in, bool *multiline_comment);
char *delete_whitespaces_from_end(char *in);

#endif // MY_STRING_H_INCLUDED
