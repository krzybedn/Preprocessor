/*
 *	Krzysztof Bednarek
 */

 #ifndef INCLUDE_H_INCLUDED


#define INCLUDE_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_string.h"

typedef struct _include
{
    struct _include *childs[128];
    bool exist;
}_include;

extern bool process(FILE *input_file);


bool init_include(const char *address);
void destroy_include();
bool add_include(const char *name);
bool process_include(const char *in);


extern void error_malloc();
extern void error_file_open(const char *name);
extern void error_endless_name(const char *name);

extern void error_include_type(char type);
extern void error_empty_include_name();
extern void warning_no_include(const char *name);

#endif // INCLUDE_H_INCLUDED
