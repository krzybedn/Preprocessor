#ifndef INCLUDE_H_INCLUDED
#define INCLUDE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_string.h"
#include "errors.h"

typedef struct _include
{
    struct _include *childs[128];
    bool exist;
}_include;

extern bool expand(FILE *input_file);


bool init_include(char *address);
void destroy_include();
bool expand_include(char *in);
bool add_include(char *name);

#endif // INCLUDE_H_INCLUDED
