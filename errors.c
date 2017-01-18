#include <stdio.h>

void error_file_open(const char *name)
{
    fprintf(stderr, "ERROR!!! I couldn't find end on include: %s", name);
}

void error_long_name(const char *name)
{
    fprintf(stderr, "ERROR!!! To long file name: %s", name);
}

void error_endless_name(const char *name)
{
    fprintf(stderr, "ERROR!!! Didn't find end of file name: %s", name);
}

void error_malloc()
{
    fprintf(stderr, "ERROR!!! I couldn't alloc enough memory");
}

void error_unknown_directive(const char *name)
{
    fprintf(stderr, "ERROR!!! Unkown directive type: %s", name);
}

void error_include(const char *name)
{
    fprintf(stderr, "ERROR!!! I couldn't find header: ", name);
}



