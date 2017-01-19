#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

char *concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1)+strlen(s2)+1);

    if(result==NULL)
    {
        return NULL;
    }

    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *getline(FILE *in)
{
    char *line=malloc(100), *line_begin=line;
    size_t lenmax=100, len=lenmax;
    int c;

    if(line==NULL)
        return NULL;

    while((c=getc(in))!=EOF && c!='\n')
    {
        if(--len==0)
        {
            len=lenmax;
            char *line_new=realloc(line_begin, lenmax*=2);

            if(line_new==NULL)
            {
                free(line_begin);
                return NULL;
            }
            line=line_new+(line-line_begin);
            line_begin=line_new;
        }
        *line++ = c;
    }
    if(c=='\n')
        *line++='\n';
    *line = '\0';
    return line_begin;
}

bool compare(const char *s1, const char *s2)
{
    int i=0;
    while(*(s1+i)!='\0' && *(s2+i)!='\0')
    {
        if(*(s1+i)!=*(s2+i))
            return 0;
        i++;
    }
    if(*(s1+i)!=*(s2+i))
        return 0;
    else
        return 1;
}

char *substring(const char *s, int b, int l)
{
    char *result=malloc((l)*sizeof(char)), *result_copy=result;
    if(result==NULL)
        return NULL;
    s=s+b;
    for(int i=0; i<l && *s!='\0'; i++)
    {
        *result++=*(s++);
        printf("%s %s\n", result_copy);
    }
    *result='\0';
    return result_copy;
}

char *subword(const char *s)
{
    char *word=malloc(100), *word_begin=word;
    size_t lenmax=100, len=lenmax;
    if(word==NULL)
        return NULL;

    while(*s!='\0' && ((*s>='a' && *s<='z') || (*s>='A' && *s<='Z') || *s=='_'))
    {
        if(--len==0)
        {
            len=lenmax;
            char *word_new=realloc(word_begin, lenmax*=2);

            if(word_new==NULL)
            {
                free(word_begin);
                return NULL;
            }
            word=word_new+(word-word_begin);
            word_begin=word_new;
        }
        *word++=*(s++);
    }
    *word = '\0';
    return word_begin;

}




