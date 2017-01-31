#include <stdio.h>

void error_file_open(const char *name)
{
    fprintf(stderr, "ERROR!!! I couldn't open file: %s.\n", name);
}

void error_long_name(const char *name)
{
    fprintf(stderr, "ERROR!!! To long file name: %s.\n", name);
}

void error_endless_name(const char *name)
{
    fprintf(stderr, "ERROR!!! I couldn't find end of file name: %s.\n", name);
}

void error_malloc()
{
    fprintf(stderr, "ERROR!!! I couldn't alloc enough memory.\n");
}

void error_unknown_directive(const char *name)
{
    fprintf(stderr, "ERROR!!! Unknown directive type: %s.\n", name);
}

void error_include(const char *name)
{
    fprintf(stderr, "ERROR!!! I couldn't find header: .\n", name);
}

void error_empty_include_name()
{
    fprintf(stderr, "ERROR!!! There apeard empty include name!\n");
}

void waring_define_exist(const char *name)
{
    fprintf(stderr, "WARNING! Define redefinition: %s\n", name);
}

void warning_no_include(const char *name)
{
    fprintf(stderr, "WARNING! I couldn't find include file: %s. Check did you add all possible paths %s.\n", name);
}

void error_endless_string(const char *name)
{
    fprintf(stderr, "ERROR!!! I couldn't find end of string: %s\n", name);
}

void error()///-----------------
{
    fprintf(stderr, "ERROR!!! I don't know what exactly is working wrong, but I'm sure, that there is something wrong");
}

void error_define_not_enough_variables(const char *name)
{
    fprintf(stderr, "ERRORR!!! I didn't get enough variables for: %s.", name);
}

void error_define_too_much_variables(const char *name)
{
    fprintf(stderr, "ERRORR!!! I got too much variables for: %s.", name);
}
