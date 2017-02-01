#include "define.h"

static _define *root;

static _define* alloc_define();
static void destroy_single_define(_define *element);
static void destroy_all_defines(_define *element);

static _define *go_to_define(char *in);
static _define *create_way_to_define(char *in);
static char** read_variables_names(char **line, int *number);
static bool add_variables_to_define(_define *root, char *line, char **variables_names);
static char** read_variables_values(char **line, int number, char *define_name);
static char* expand_define_with_variables(_define *root, char **variables_names);

//funkcja pozwalajaca na inicjalizacje root
bool init_define()
{
    if(root!=NULL)
        destroy_define();
    root=alloc_define();
    if(root==NULL)
        return 1;
    else
        return 0;
}

//konstruktor struktury _define
static _define* alloc_define()
{
    _define *new_define=malloc(sizeof(_define));
    if(new_define==NULL)
    {
        error_malloc();
        return NULL;
    }
    for(int i=0; i<63; i++)
        new_define->childs[i]=NULL;
    new_define->exist=0;
    new_define->variables_positions=NULL;
    new_define->variables_occur=NULL;
    return new_define;
}

//desktruktor calego drzewa _define
static void destroy_all_defines(_define *element)
{
    if(element==NULL)
        return;
    for(int i=0; i<63; i++)
        destroy_all_defines(element->childs[i]);
    destroy_single_define(element);
    free(element);
}

//funkcja usuwajaca cala zawartosc _define
static void destroy_single_define(_define *element)
{
    if(element->exist)
    {
        element->exist=0;
        if(element->value!=NULL)
            free(element->value);
        if(element->variables_positions!=NULL)
            free(element->variables_positions);
        if(element->variables_occur!=NULL)
            free(element->variables_occur);
    }
}

//funkcja wywolujaca destruktor na root
void destroy_define()
{
    destroy_all_defines(root);
    root=NULL;
}

//fukcja pomocnicza sluzaca do przejscia gotowego drzewa strukury _define i zwrocenie danego defina
static _define *go_to_define(char *in)
{
    _define *element=root;
    while(element!=NULL && *in!='\0')
    {
        if(*in>='a' && *in<='z')
            element=element->childs[*in-'a'];
        else if(*in>='A' && *in<='Z')
            element=element->childs[26+*in-'A'];
        else if(*in>='0' && *in<='9')
            element=element->childs[52+*in-'0'];
        else
            element=element->childs[62];
        in++;
    }
    return element;
}

static _define *create_way_to_define(char *in)
{
    _define *element=root;
    while(element!=NULL && is_letter(*in))
    {
        if(*in>='a' && *in<='z')
        {
            if(element->childs[*in-'a']==NULL)
            {
                if((element->childs[*in-'a']=alloc_define())==NULL)
                    return NULL;
            }
            element=element->childs[*in-'a'];
        }
        else if(*in>='A' && *in<='Z')
        {
            if(element->childs[26+*in-'A']==NULL)
            {
                if((element->childs[26+*in-'A']=alloc_define())==NULL)
                    return NULL;
            }
            element=element->childs[26+*in-'A'];
        }
        else if(*in>='0' && *in<='9')
        {
            if(element->childs[52+*in-'0']==NULL)
            {
                if((element->childs[52+*in-'0']=alloc_define())==NULL)
                    return NULL;
            }
            element=element->childs[52+*in-'0'];
        }
        else if(*in=='_')
        {
            if(element->childs[62]==NULL)
            {
                if((element->childs[62]=alloc_define())==NULL)
                    return NULL;
            }
            element=element->childs[62];
        }
        in++;
    }
    return element;
}

//fukcja wywolywana w celu sprawdzenia, czy w danej linii wystepuja uzycia juz zdefiniowanych makrodyrektyw #define
char* expand_define(char *in)///---
{
    char *line=malloc(sizeof(char)*10);
    if(line==NULL)
    {
        return NULL;
    }
    char *line_begin=line;
    size_t lenmax=10, len=lenmax;
    while(*in!='\0')
    {
        _define *element=root;
        while(*in!='\0' && !is_letter(*in))
        {
            line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *in++);
            if(line_begin==NULL)
            {
                return NULL;
            }
        }
        if(*in=='\0')
        {
            *line='\0';
            return line_begin;
        }
        char *word=subword(&in);
        if(word==NULL)
        {
            free(line_begin);
            return NULL;
        }
        char *word_begin=word;
        element=go_to_define(word);
        if(element!=NULL && element->exist)
        {
            if(element->variables_exist)
            {
                if(*in!='(')
                {
                    error_define_not_enough_variables(word);
                    free(line_begin);
                    free(word);
                    return NULL;
                }
                in++;

                char **variables_begin=read_variables_values(&in, element->variables_number, word);
                if(variables_begin==NULL)
                {
                    free(word);
                    free(line_begin);
                    return NULL;
                }

                char *expanded_define=expand_define_with_variables(element, variables_begin);
                for(char **variables=variables_begin; variables-variables_begin<element->variables_number; variables++)
                    free(*variables);
                free(variables_begin);
                if(expanded_define==NULL)
                {
                    free(word_begin);
                    free(line_begin);
                    return NULL;
                }
                *line='\0';
                char *new_line=concat(line_begin, expanded_define);
                if(new_line==NULL)
                {
                    free(word);
                    free(line_begin);
                    free(expanded_define);
                    return NULL;
                }
                lenmax=string_length(new_line)+1;
                len=1;
                line=new_line+lenmax-1;
                line_begin=new_line;
            }
            else
            {
                *line='\0';
                char *new_line=concat(line_begin, element->value);
                free(line_begin);
                if(new_line==NULL)
                {
                    free(word_begin);
                    return NULL;
                }
                lenmax=string_length(new_line)+1;
                len=1;
                line=new_line+lenmax-1;
                line_begin=new_line;
            }
        }
        else
        {
            *line='\0';
            char *new_line=concat(line_begin, word_begin);
            free(line_begin);
            if(new_line==NULL)
            {
                free(word_begin);
                return NULL;
            }
            lenmax=string_length(new_line)+1;
            len=1;
            line=new_line+lenmax-1;
            line_begin=new_line;
        }
        free(word_begin);
    }
    *line='\0';
    return line_begin;
}

//funkcja wywolywana po znaleznieniu dyrektywy #define w celu dodania nowej definicji do listy
bool add_define(char *line)
{
    while(*line<=' ')
        line++;
    char *name=subword(&line), *name_begin=name;
    _define *element=create_way_to_define(name);
    if(element==NULL)
    {
        error_malloc();
        return 1;
    }

    char *new_line=expand_define(line);
    if(new_line==NULL)
    {
        error_malloc();
        free(name_begin);
        return 1;
    }
    line=new_line;

    if(element->exist)
        waring_define_exist(name_begin);
    element->exist=1;
    if(*line=='(')
    {
        char **variables_begin;
        variables_begin=read_variables_names(&line, &element->variables_number);
        if(variables_begin==NULL)
        {
            free(name_begin);
            error_malloc();
            return 1;
        }
        if(line=='\0')
        {
            error_endless_name(name_begin);
            free(name_begin);
            return 1;
        }
        element->variables_exist=1;
        if(add_variables_to_define(element, line, variables_begin))
        {
            free(name_begin);
            error_malloc();
            return 1;
        }
    }
    else
    {
        element->variables_exist=0;
        while(*line!='\0' && *line<=' ')
            line++;
        element->value=malloc(sizeof(char)*20);
        if(element->value==NULL)
        {
            free(name_begin);
            error_malloc();
            return 1;
        }
        char *word=element->value;
        size_t lenmax=20, len=lenmax;
        while(*line!='\0')
        {
            element->value=add_char_to_string(&word, element->value, &len, &lenmax, *line++);
            if(element->value==NULL)
            {
                free(name_begin);
                error_malloc();
                return 1;
            }
        }
        element->value_length=lenmax-len;
    }

    return 0;
}

//funkcja ma za zadanie wczytac zmienne, tkore beda uzywane w danej definicji i zwraca tablice zawierajaca ich nazwy
char** read_variables_names(char **line, int *number)
{
    char **variables_begin=malloc(10*sizeof(char*));
    if(variables_begin==NULL)
    {
        return NULL;
    }
    size_t variables_max=10, variables_num=10;
    char **variables=variables_begin;
    (*line)++;
    while(**line!='\0' && **line!=')')
    {
        if(--variables_num==0)
        {
            char **variables_new=realloc(variables_begin, variables_max*2);
            if(variables_new==NULL)
            {
                for(char** i=variables_begin; i<variables_begin+variables_max-variables_num+1; i++)
                    free(i);
                free(variables_begin);
                return NULL;
            }
            variables_max*=2;
            variables=variables_new+(variables-variables_begin);
        }
        *variables=subword(line);
        if(*variables==NULL)
        {
            for(char** i=variables_begin; i<variables_begin+variables_max-variables_num; i++)
                free(i);
            free(variables_begin);
            return NULL;
        }
        variables++;
        while(**line!='\0' && **line<=' ')
            (*line)++;
        if(**line==',')
            (*line)++;
        while(**line!='\0' && **line<=' ')
            (*line)++;
    }
    *number=variables_max-variables_num;
    while(**line!='\0' && **line<=' ')
        (*line)++;
    (*line)++;
    while(**line!='\0' && **line<=' ')
        (*line)++;
    return variables_begin;
}

///----
bool add_variables_to_define(_define *element, char *line, char **variables_names)
{
    element->variables_positions=(int*)malloc(10*sizeof(int*));
    if(element->variables_positions==NULL)
    {
        return 1;
    }
    element->variables_occur=(int*)malloc(10*sizeof(int));
    if(element->variables_occur==NULL)
    {
        return 1;
    }
    element->value=(char*)malloc(10*sizeof(char));
    if(element->value==NULL)
    {
        return 1;
    }
    size_t len=10, lenmax=10;
    int variables_max=10;
    element->variables_occur_number=0;
    char *word=element->value;
    while(*line!='\0')
    {
        while(!is_letter(*line))
        {
            element->value=add_char_to_string(&word, element->value, &len, &lenmax, *line++);
            if(element->value==NULL)
            {
                return 1;
            }
        }
        char *name=subword(&line);
        if(name==NULL)
        {
            return 1;
        }
        for(int i=0; i<element->variables_number; i++)
        {
            if(compare(name, *(variables_names+i)))
            {
                if(element->variables_occur_number==variables_max)
                {
                    int *new_pos=(int*)realloc(element->variables_positions, variables_max*sizeof(int));
                    if(new_pos==NULL)
                    {
                        free(element->variables_positions);
                        return 1;
                    }
                    element->variables_positions=new_pos;

                    int *new_occ=(int*)realloc(element->variables_occur, variables_max*sizeof(int));
                    if(new_occ==NULL)
                    {
                        free(element->variables_occur);
                        return 1;
                    }
                    element->variables_occur=new_occ;
                    variables_max*=2;
                }
                *(element->variables_occur+element->variables_occur_number)=i;
                *(element->variables_positions+element->variables_occur_number)=len;
                element->value=add_char_to_string(&word, element->value, &len, &lenmax, '\n');
                element->variables_occur_number++;
                //wiemy ze cala zawartosc defina ma jedna linijke, mozemy wiec wykorzystac zank '\n' jako znak zmiennej
                if(element->value==NULL)
                {
                    return 1;
                }
            }
        }
    }
    element->value_length=len;
    return 0;
}

char** read_variables_values(char **line, int number, char *define_name)
{
    char **variables_begin=malloc(number*sizeof(char*));
    if(variables_begin==NULL)
    {
        return NULL;
    }
    char **variables=variables_begin;
    int variables_number=0;
    while(**line!='\0' && variables_number<number && **line!=')')
    {
        while(**line!='\0' && **line<=' ')
            (*line)++;
        *variables=(char*)malloc(10*sizeof(char));
        if(*variables==NULL)
        {
            error_malloc();
            for(variables=variables_begin; variables-variables_begin<variables_number-1; variables++)
                free(*variables);
            free(variables_begin);
            return NULL;
        }
        int brackets=0;
        char *name=*(variables);
        size_t len=10, len_max=10;
        while(**line!='\0' && !(brackets==0 && (**line==')' || **line==',' )))
        {
            if(**line=='(')
                brackets++;
            else if(**line==')')
                brackets--;
            *variables=add_char_to_string(&name, *variables, &len, &len_max, **line);
            if(*variables==NULL)
            {
                error_malloc();
                for(variables=variables_begin; variables-variables_begin<variables_number-1; variables++)
                    free(*variables);
                free(variables_begin);
                return NULL;
            }
            (*line)++;
        }
        *name='\0';
        name=expand_define(*variables);
        if(name==NULL)
        {
            for(variables=variables_begin; variables-variables_begin<variables_number; variables++)
                free(*variables);
            free(variables_begin);
            return NULL;
        }
        *variables=name;
        variables++;
        if(**line==',')
            (*line)++;
        variables_number++;
    }
    if(**line=='\0')
    {
        error_define_not_enough_variables(define_name);
        for(variables=variables_begin; variables-variables_begin<number; variables++)
            free(*variables);
        free(variables);
        return NULL;
    }
    if(**line==')' && variables_number!=number)
    {
        if(variables_number<number)
            error_define_not_enough_variables(define_name);
        else
            error_define_too_much_variables(define_name);
        for(variables=variables_begin; variables-variables_begin<number; variables++)
            free(*variables);
        free(variables);
        return NULL;
    }
    (*line)++;

    return variables_begin;
}

char* expand_define_with_variables(_define *element, char **variables_names)
{
    char *line_begin=(char*)malloc(10*sizeof(char));
    if(line_begin==NULL)
    {
        error_malloc();
        return NULL;
    }
    char *line=line_begin;
    size_t len=10, len_max=len;
    int variables_number=0;
    char *in=element->value;
    while(*in!='\0')
    {
        if(*in=='\n')
        {
            char *variable=*(variables_names+*(element->variables_occur+variables_number));
            int var_len=string_length(variable);
            *line='\0';
            char *new_line=concat(line_begin, variable);
            if(new_line==NULL)
            {
                error_malloc();
                free(line_begin);
                return NULL;
            }
            line=new_line+(line-line_begin)+var_len;
            *line='s';
            len_max=string_length(new_line)+1;
            len=1;
            free(line_begin);
            line_begin=new_line;
            variables_number++;
        }
        else
        {
            line_begin=add_char_to_string(&line, line_begin, &len, &len_max, *in);
            if(line_begin==NULL)
            {
                error_malloc();
                return NULL;
            }
        }
        in++;
    }
    *line='\0';
    return line_begin;
}

