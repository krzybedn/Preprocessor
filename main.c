#include "preprocesor.h"



int main()
{
    FILE *in, *out;
    if(open(&in, &out))
    {
        return 1;
    }
    rewrite(in, out);
    //rewrite_include(in, out);



    close(in, out);
    return 0;
}
