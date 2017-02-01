#ifndef PREPROCESOR_H_INCLUDED
#define PREPROCESOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "my_string.h"
#include "define.h"
#include "include.h"
#include "errors.h"




//funkcje ogolne
char *open(FILE **in, FILE **out);
void close(FILE *in, FILE *out);
bool cosik();
bool rewrite(FILE *in, FILE *out);


#endif // PREPROCESOR_H_INCLUDED
