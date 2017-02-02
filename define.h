#ifndef DEFINE_H_INCLUDED
#define DEFINE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_string.h"
#include "errors.h"

typedef struct _define
 {
    struct _define *childs[63];
    bool exist;
    char *value;

    bool variables_exist;
    int variables_number;
    /******************************/
    int variables_occur_number;//liczba wystapien zmiennych na roznych pozycjach
    int *variables_occur;//jaka zamienna wystepuje na danej pozycji
}_define;


bool init_define();
void destroy_define();
char* expand_define(char *in);
bool add_define(char *line);

#endif // DEFINE_H_INCLUDED
