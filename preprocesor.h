#ifndef PREPROCESOR_H_INCLUDED
#define PREPROCESOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct _define
 {
    char name;
    struct _define *childs[63];
    bool exist;
    char *value;
    int value_length;

    /*bool exist_variebles;
    int variebles_number;
    char **variebles;
    int *variebles_length;*/
}_define;


extern char *concat(const char *s1, const char *s2);
extern char *getline(FILE *in);
extern bool compare(const char *s1, const char *s2);
extern char *substring(const char *s, int b, int e);
extern char *subword(const char *s);


bool open(FILE **in, FILE **out);
void close(FILE *in, FILE *out);
bool rewrite(FILE *in, FILE *out);
bool rewrite_include(char *in, FILE *out);



extern void error_malloc();
extern void error_file_open(const char *name);
extern void error_long_name(const char *name);
extern void error_endless_name(const char *name);
extern void error_unknown_directive(const char *name);
extern void error_include(const char *name);

#endif // PREPROCESOR_H_INCLUDED
