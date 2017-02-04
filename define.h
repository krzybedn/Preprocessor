#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_string.h"

typedef struct _define
 {
    struct _define *childs[63];
    bool exist;
    char *value;

    bool variables_exist;
    int variables_number;
    /******************************/
    int variables_occur_number;//liczba wystapien zmiennych na roznych pozycjach
    int *variebles_position;
    int *variables_occur;//jaka zamienna wystepuje na danej pozycji
}_define;


bool init_define();
void destroy_define();
char* process_define(char *in);
bool add_define(char *line);


extern void error_endless_string(const char *name);
extern void error_malloc();

extern void error_define_not_enough_variables(const char *name);
extern void error_define_too_much_variables(const char *name);
extern void warning_define_exist(const char *name);

#endif // DEFINE_H_INCLUDED
