#ifndef PREPROCESOR_H_INCLUDED
#define PREPROCESOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "my_string.h"


typedef _Bool bool;

typedef struct _define
 {
    struct _define *childs[63];
    bool exist;
    char *value;
    int value_length;

    bool variables_exist;
    int variables_number;
    /******************************/
    int variables_occur_number;//liczba wystapien zmiennych na roznych pozycjach
    int *variables_positions;//pozycje, na ktorych pojawi sie jakas zmienna
    int *variables_occur;//jaka zamienna wystepuje na danej pozycji
    /**
    int **variables_positions;//pozycje, na ktorych pojawi sie dana zmienna
    int *variables_occur;//liczba wystapien danej zmiennej
    */
}_define;

typedef struct _include
{
    struct _include *childs[128];
    bool exist;
}_include;

//funkcje ogolne
char *open(FILE **in, FILE **out);
void close(FILE *in, FILE *out);
bool cosik();
bool rewrite(FILE *in, FILE *out, _define *root, _include *includes_list, char *source);

//obsluga definow
_define* alloc_define();
void destroy_define(_define *root);
_define *go_to_define(char *in, _define *root);
char* rewrite_define(char *in, _define *root);
bool add_define(char *line, _define *root);
char** read_variables_names(char **line, int *number);
bool add_variables_to_define(_define *root, char *line, char **variables_names);
char** read_variables_values(char **line, int number, char *define_name);
char* rewrite_define_with_variables(_define *root, char **variables_names);


//obsluga includow
_include* alloc_include();
void destroy_include(_include *root);
bool rewrite_include(char *in, FILE *out, _define *define_list, _include *includes_list, char *source);
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
extern void error_endless_string(const char *name);
void error_define_not_enough_variables(const char *name);
void error_define_too_much_variables(const char *name);

extern void waring_define_exist(const char *name);
extern void warning_no_include(const char *name);

extern void error();///------------
#endif // PREPROCESOR_H_INCLUDED
