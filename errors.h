#ifndef ERRORS_H_INCLUDED
#define ERRORS_H_INCLUDED

#include <stdio.h>

void error_malloc();
void error_file_open(const char *name);

void error_endless_name(const char *name);
void error_endless_string(const char *name);

void error_include(const char *name);
void error_include_type(char type);
void error_empty_include_name();

void error_define_not_enough_variables(const char *name);
void error_define_too_much_variables(const char *name);

void waring_define_exist(const char *name);
void warning_no_include(const char *name);


#endif // ERRORS_H_INCLUDED
