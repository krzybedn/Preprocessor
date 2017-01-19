#include "preprocesor.h"

bool open(FILE **in, FILE **out)
{
    char str[1001]={0};
    scanf("%s",str);
    if(str[1000]!=0)
    {
        error_long_name(str);
        return 1;
    }
    *in = fopen(str, "r");
    if(*in == NULL)
    {
        error_file_open(str);
        return 1;
    }
    for(int i=0; i<=1000; i++)
        str[i]=0;
    scanf("%s", str);
    if(str[1000]!=0)
    {
        error_long_name(str);
        return 1;
    }
    *out = fopen(str, "w");
    if(*out == NULL)
    {
        error_file_open(str);
        return 1;
    }
    return 0;
}

void close(FILE *in, FILE *out)
{
    fclose(in);
    fclose(out);
}

void read_whitespace(FILE *in)
{
    int c;
    while((c=getc(in))!=EOF && c<' ');
    ungetc(c, in);
}

bool rewrite(FILE *in, FILE *out)
{
    //_define *root;
    char *line;
    while((line=getline(in))!=NULL && *line!='\0')
    {
        //tutaj uzywamy definow

        if(line[0]=='#')
        {
            char *type=subword(line+1);
            if(type==NULL)
                error_malloc();
            if(compare(type, "include"))
            {
                rewrite_include(line+8, out);
            }
            else if(compare(type, "define"))
            {
               /// _define new_define=create_define
            }
            else
            {
                fprintf(out, "%s\n",line);
                /*error_unknown_directive(type);
                free(type);
                return 1;*/
            }
            free(type);
        }
        else
        {
            fprintf(out, "%s",line);
        }
    }
    if(line==NULL)
    {
        error_malloc();
        return 1;
    }
    return 0;
}


bool rewrite_include(char *in, FILE *out)
{
    while(*in<=' ')
        in++;
    if(*in=='\0')
    {
        ///error
        return 1;
    }

    char *adress;
    FILE *header;
    if(*in=='"')
    {
        in++;
        char name[1000]={0};
        int i=0;
        while(*in!='\0' && *in!='"' && i<1000)
            name[i++]=*(in++);
        if(i==1000)
        {
            error_long_name(name);
            return 1;
        }
        if(*in=='\0')
        {
            error_endless_name(name);
            return 1;
        }
        adress=name;
        header=fopen(adress, "r");
        if(header == NULL)
        {
            error_file_open(adress);
            return 1;
        }
    }
    else if(*in=='<')
    {
        in++;
        char name[1000]={0};
        int i=0;
        while(*in!='\0' && *in!='>' && i<1000)
            name[i++]=*(in++);
        if(i==1000)
        {
            error_long_name(name);
            return 1;
        }
        if(*in=='\0')
        {
            error_endless_name(name);
            return 1;
        }
        bool tmp=1;
        FILE *includes=fopen("include.txt", "r");
        if(includes == NULL)
        {
            error_file_open("include.txt");
            return 1;
        }

        do
        {
            char *include_adress=getline(includes);
            if(*include_adress=='\0')
            {
                error_include(name);
                return 1;
            }
            adress=concat(include_adress, name);
            free(include_adress);

            if(adress!=NULL)
            {
                header=fopen(adress, "r");
                if(header == NULL)
                {
                    error_file_open(adress);
                    return 1;
                }
                else
                {
                    tmp=0;
                }
            }
        }while(tmp);

    }
    free(adress);

    /*char c;
    while((c=getc(header))!=EOF)
        putc(c, out);//*/
    rewrite(header, out);
    fclose(header);
    return 0;
}







