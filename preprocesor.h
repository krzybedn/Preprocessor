#ifndef PREPROCESOR_H_INCLUDED
#define PREPROCESOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

typedef _Bool bool;

typedef struct _define
 {
    struct _define *childs[63];
    bool exist;
    char *value;
    int value_length;

    bool exist_variebles;
    /*int variebles_number;
    char **variebles;
    int *variebles_length;*/
}_define;

typedef struct _include
{
    struct _include *childs[128];
    bool exist;
}_include;

extern char *concat(const char *s1, const char *s2);
extern char *getline(FILE *in);
extern bool compare(const char *s1, const char *s2);
extern char *substring(const char *s, int b, int e);
extern char *subword(char **s);
extern char *add_char_to_string(char **s, char *s_begin, size_t *len,  size_t *lenmax, char c);
extern bool is_letter(char c);


//funkcje ogolne
bool open(FILE **in, FILE **out);
void close(FILE *in, FILE *out);
bool cosik();
bool rewrite(FILE *in, FILE *out, _define *root, _include *includes_list);

//obsluga definow
_define* alloc_define();
void destroy_define(_define *root);
_define *go_to_define(char *in, _define *root);
char* rewrite_define(char *in, _define *root, bool *multiline_comment);
bool add_define(char *line, _define *root);

//obsluga includow
_include* alloc_include();
void destroy_include(_include *root);
bool rewrite_include(char *in, FILE *out, _define *define_list, _include *includes_list);
bool exist_incude(_include *include_root, char *name);
bool add_include(_include *include_root, char *name);

//bool copy_define(_define *d1, _define *d2);
//bool merge_define(_define *d1, _define *d2);


extern void error_malloc();
extern void error_file_open(const char *name);
extern void error_long_name(const char *name);
extern void error_endless_name(const char *name);
extern void error_unknown_directive(const char *name);
extern void error_include(const char *name);
extern void error_empty_include_name();

extern void waring_define_exist(const char *name);
extern void warning_no_include(const char *name);


#endif // PREPROCESOR_H_INCLUDED
