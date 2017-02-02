#include "my_string.h"

int string_length(const char *s)
{
    int res=0;
    while(*s++!='\0')
        res++;
    return res;
}

bool is_letter(char c)//Liera nazywam dowolny znak sporod malych i duzych liter, cyfr oraz podkreslenia
{
    return ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_');
}

void string_copy(char *s1, const char *s2)
{
    while((*s1++=*s2++)!='\0');
}

void string_append(char *s1, const char *s2)
{
    while(*s1!='\0')
        s1++;
    while((*s1++=*s2++)!='\0');
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
    char *result=(char*)malloc(string_length(s1)+string_length(s2)+10);

    if(result==NULL)
    {
        return NULL;
    }
    string_copy(result, s1);
    string_append(result, s2);
    return result;
}

char *get_line(FILE *in)
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
    //sam znak '\0' oznacza koniec ciagu znakow, dlatego chcemy inczaczej oznaczyc linie puste
    if(c=='\n' && line==line_begin)
        *line++='\n';
    *line = '\0';
    return line_begin;
}

bool compare(const char *s1, const char *s2)
{
    while(*s1!='\0' && *s2!='\0')
    {
        if(*s1++!=*s2++)
            return 0;
    }
    if(*s1!=*s2)
        return 0;
    else
        return 1;
}

char *substring(const char *s, int b, int l)
{
    char *result=malloc(l*sizeof(char));
    if(result==NULL)
        return NULL;
    char *result_copy=result;
    s=s+b;
    for(int i=0; i<l && *s!='\0'; i++)
    {
        *result++=*s++;
    }
    *result='\0';
    return result_copy;
}

char *subword(char **s)
{
    char *word=(char*)malloc(10*sizeof(char));
    if(word==NULL)
        return NULL;
    char *word_begin=word;
    size_t lenmax=10, len=lenmax;

    while(is_letter(**s))
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

char *split_by_last_slash(char **address)
{
    char *address_begin=*address;
    int i=0, last=-1;
    while(*(address_begin+i)!='\0')
    {
        if(*(address_begin+i)=='\\' || *(address_begin+i)=='/')
            last=i;
        i++;
    }
    if(last==-1)
    {
        char *tmp=(char*)malloc(sizeof(char));
        *tmp='\0';
        return tmp;
    }
    *address=substring(address_begin, last+1, i-last-1);
    if(*address==NULL)
    {
        *address=address_begin;
        return NULL;
    }
    char *new_address=(char*)realloc(address_begin, (last+1)*sizeof(char));
    if(new_address==NULL)
    {
        free(address_begin);
        return NULL;
    }
    *(new_address+last+1)='\0';
    return new_address;
}

char *delete_comments(char *in, bool *multiline_comment)
{
    char *line=malloc(10*sizeof(char));
    if(line==NULL)
        return NULL;
    char *line_begin=line;
    size_t len=10, len_max=10;
    while(*in!='\0')
    {
        if(!*multiline_comment && *in=='/')
        {
            if(*(in+1)=='/')
            {
                *line='\0';
                return line_begin;
            }
            else if(*(in+1)=='*')
            {
                *multiline_comment=1;
                in+=2;
            }
            else
            {
                line_begin=add_char_to_string(&line, line_begin, &len, &len_max, *in++);
                if(line_begin==NULL)
                {
                    return NULL;
                }
            }
        }
        else if(*multiline_comment)
        {
            if(*in=='*' && *(in+1)=='/')
            {
                *multiline_comment=0;
                in+=2;
            }
            else
            {
                in++;
            }
        }
        else
        {
            line_begin=add_char_to_string(&line, line_begin, &len, &len_max, *in++);
            if(line_begin==NULL)
            {
                return NULL;
            }
        }
    }
    *line='\0';
    return line_begin;
}
