#ifndef PREPROCESOR_H_INCLUDED
#define PREPROCESOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_string.h"
#include "define.h"
#include "include.h"

bool process(FILE *input_file);

extern void error_malloc();
extern void error_file_open(const char *name);

#endif // PREPROCESOR_H_INCLUDED
