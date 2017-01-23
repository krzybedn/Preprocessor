#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool is_letter(char c)//Liera zazywam dowolny znak z gropy litery male i duze, cyfry oraz podkreslenia
{
    return ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_');
}

char *add_char_to_string(char **s, char *s_begin, size_t *len,  size_t *lenmax, char c)
{
    if(--(*len)==0)
    {
        *len=*lenmax;
        char *s_new=realloc(s_begin, sizeof(char)*(*lenmax*=2));
        if(s_new==NULL)
        {
            free(s_begin);
            return NULL;
        }
        *s=s_new+(*s-s_begin);
        s_begin=s_new;
    }
    **s=c;
    (*s)++;
    return s_begin;
}

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
    char *line=malloc(100);
    if(line==NULL)
        return NULL;

    char *line_begin=line;
    size_t lenmax=100, len=lenmax;
    int c;

    while((c=getc(in))!=EOF && c!='\n')
    {
        line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, c);
        if(line_begin==NULL)
        {
            return NULL;
        }
    }

    if(c=='\n' && line==line_begin)
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

char *subword(char **s)
{
    char *word=malloc(sizeof(char)*10), *word_begin=word;
    size_t lenmax=10, len=lenmax;
    if(word==NULL)
        return NULL;

    while(**s!='\0' && is_letter(**s))
    {
        word_begin=add_char_to_string(&word, word_begin, &len, &lenmax, **s);
        if(word_begin==NULL)
        {
            return NULL;
        }
        (*s)++;
    }
    *word = '\0';
    return word_begin;

}




