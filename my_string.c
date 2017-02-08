#include "my_string.h"


static char *delete_whitespaces_from_end(char *in);
static char *delete_comments(char *in, bool *multiline_comment);
static char *clearing(char *in, bool *multiline_comment);
static char* add_new_line(char *line_begin, FILE *input_file, char **next_line, bool *multiline_comment);



bool is_letter(char c)//Liera nazywam dowolny znak sporod malych i duzych liter, cyfr oraz podkreslenia.
{
    return ((c>='a' && c<='z') || (c>='A' && c<='Z') || (c>='0' && c<='9') || c=='_');
}

int string_length(const char *s)
{
    int res=0;
    while(*s++!='\0')
        res++;
    return res;
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
    char *line=malloc(10);
    if(line==NULL)
        return NULL;
    char *line_begin=line;
    size_t lenmax=10, len=lenmax;
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
    int i, last=-1;
    for(i=0; *(address_begin+i)!='\0'; i++)
    {
        if(*(address_begin+i)=='\\' || *(address_begin+i)=='/')
            last=i;
    }
    if(last==-1)
    {
        char *tmp=(char*)malloc(sizeof(char));
        *tmp='\0';
        return tmp;
    }
    char *cos=substring(address_begin, last+1, i-last);
    if(cos==NULL)
    {
        *address=address_begin;
        return NULL;
    }
    *address=cos;
    char *new_address=(char*)realloc(address_begin, (last+2)*sizeof(char));
    if(new_address==NULL)
    {
        free(address_begin);
        return NULL;
    }
    *(address_begin+last+1)='\0';
    return address_begin;
}

char *concat_multiline(char *begin, FILE *input_file, bool *multiline_comment, char **next_line)
{
    char *line_begin=clearing(begin, multiline_comment);
    if(line_begin==NULL)
    {
        return NULL;
    }
    if(*line_begin=='#')
    {
        while(*(line_begin+string_length(line_begin)-1)=='\\')
        {
            //usuwamy znak '\\' z konca linii
            *(line_begin+string_length(line_begin)-1)='\0';
            line_begin=add_new_line(line_begin, input_file, next_line, multiline_comment);
            if(line_begin==NULL)
                return NULL;
            if(*next_line!=NULL)
                return line_begin;
        }
    }
    else
    {
        while(*(line_begin+string_length(line_begin)-1)!=';'
        && *(line_begin+string_length(line_begin)-1)!='{' && *(line_begin+string_length(line_begin)-1)!='}')
        {
            line_begin=add_new_line(line_begin, input_file, next_line, multiline_comment);
            if(line_begin==NULL)
                return NULL;
            if(*next_line!=NULL)
                return line_begin;
        }
    }
    return line_begin;
}


static char *delete_whitespaces_from_end(char *in)
{
    char *line=in+string_length(in)-1;
    while(*line<=' ' && line>in)
        line--;
    if(*line<=' ')
    {
        free(in);
        line=malloc(sizeof(char));
        if(line==NULL)
            return NULL;
        *line='\0';
        return line;
    }
    else
    {
        int len=line-in+2;
        char *new_line=realloc(in, sizeof(char)*len);
        if(new_line==NULL)
        {
            free(in);
            return NULL;
        }
        *(new_line+len-1)='\0';
        return new_line;
    }

}

static char *delete_comments(char *in, bool *multiline_comment)
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

static char *clearing(char *in, bool *multiline_comment)
{
    char *line_begin=delete_comments(in, multiline_comment);
    if(line_begin==NULL)
    {
        return NULL;
    }
    in=line_begin;
    in=delete_whitespaces_from_end(line_begin);
    if(in==NULL)
    {
        free(line_begin);
        return NULL;
    }
    return in;
}

static char* add_new_line(char *line_begin, FILE *input_file, char **next_line, bool *multiline_comment)
{
    char *line=get_line(input_file);
    if(line==NULL)
    {
        free(line_begin);
        return NULL;
    }
    if(*line=='\0')
    {
        free(line);
        *next_line=malloc(sizeof(char));
        **next_line='\0';
        return line_begin;
    }
    if(*line=='#')
    {
        *next_line=line;
        return line_begin;
    }
    char *new_line=clearing(line, multiline_comment);
    free(line);
    if(new_line==NULL)
    {
        free(line_begin);
        return NULL;
    }
    line=new_line;
    if(*line!='\0')
    {
        new_line=concat(line_begin, "\n");
        free(line_begin);
        if(new_line==NULL)
        {
            return NULL;
        }
        line_begin=new_line;
    }
    new_line=concat(line_begin, line);
    free(line_begin);
    free(line);
    if(new_line==NULL)
    {
        return NULL;
    }
    return new_line;
}


