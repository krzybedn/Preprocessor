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

extern bool process(FILE *input_file);


bool init_include(const char *address);
void destroy_include();
bool process_include(const char *in);
bool add_include(const char *name);

#endif // INCLUDE_H_INCLUDED
