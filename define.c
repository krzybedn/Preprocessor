#include "define.h"

static _define *root;

static _define* alloc_define();
static void destroy_single_define(_define *element);
static void destroy_all_defines(_define *element);

static _define *go_to_define(const char *in);
static char** read_variables_names(char **line, int *number, bool *VA_ARGS);
bool add_variable_occur_to_list(_define *element, int *variables_max, int i, int position);
static bool add_value_to_define_with_variables(_define *root, char *line, char **variables_names);

static char* process_define_with_variables(const _define *root, char **variables_names);
static _define *create_way_to_define(const char *in);
static char** read_variables_values(char **in, const int number, const char *define_name, bool VA_ARGS);

/****************************************************************************/
/******************* INICJALIZACJA ******************************************/
//Funkcja pozwalajaca na inicjalizacje drzewa _define.
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

//Konstruktor struktury _define.
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
    new_define->variables_occur=NULL;
    new_define->VA_ARGS=0;
    return new_define;
}

/****************************************************************************/
/******************* DESTRUKCJA *********************************************/
//Funkcja wywolujaca destruktor na drzewie _define.
void destroy_define()
{
    destroy_all_defines(root);
    root=NULL;
}

//Destruktor calego drzewa _define.
static void destroy_all_defines(_define *element)
{
    if(element==NULL)
        return;
    for(int i=0; i<63; i++)
        destroy_all_defines(element->childs[i]);
    destroy_single_define(element);
    free(element);
}

//Dunkcja usuwajaca cala zawartosc pojedynczego elemcntu _define.
static void destroy_single_define(_define *element)
{
    if(element->exist)
    {
        element->exist=0;
        if(element->value!=NULL)
            free(element->value);
        if(element->variables_occur!=NULL)
            free(element->variables_occur);
    }
}

/****************************************************************************/
/******************* DODAWANIE **********************************************/
//Funkcja wywolywana po znaleznieniu nowej dyrektywy #define w celu dodania jej do listy.
bool add_define(char *in)
{
    while(*in!='\0' && *in<=' ')
        in++;
    char *name=subword(&in);
    if(name==NULL)
    {
        error_malloc();
        return 1;
    }
    char *name_begin=name;
    _define *element=create_way_to_define(name);
    if(element==NULL)
    {
        error_malloc();
        return 1;
    }


    if(element->exist)
        warning_define_exist(name_begin);
    element->exist=1;
    if(*in=='(')
    {
        char **variables_begin=read_variables_names(&in, &element->variables_number, &element->VA_ARGS);
        if(variables_begin==NULL)
        {
            error_malloc();
            free(name_begin);
            return 1;
        }
        element->variables_exist=1;
        if(in=='\0')
        {
            error_endless_string(name_begin);
            free(name_begin);
            return 1;
        }
        if(add_value_to_define_with_variables(element, in, variables_begin))
        {
            error_malloc();
            free(name_begin);
            return 1;
        }
    }
    else
    {
        element->variables_exist=0;///osobna funkcja??
        while(*in!='\0' && *in<=' ')
            in++;
        element->value="\0";
        char *new_word=concat(element->value, in);
        if(new_word==NULL)
        {
            error_malloc();
            free(name_begin);
            element->value=NULL;
            return NULL;
        }
        element->value=new_word;
        new_word=process_define(element->value);
        if(new_word==NULL)
        {
            error_malloc();
            free(name_begin);
            element->value=NULL;
            return NULL;
        }
        element->value=new_word;
    }
    free(name_begin);
    return 0;
}

//Fukcja pomocnicza szukajaca danego defina w juz stworzonym drzewie.
static _define *go_to_define(const char *in)
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

//Funkcja ma za zadanie wczytac zmienne, ktore beda uzywane w danej definicji.
//Nastepnie zwraca tablice zawierajaca ich nazwy.
char** read_variables_names(char **in, int *number, bool *VA_ARGS)
{
    char **variables_begin=malloc(10*sizeof(char*));
    if(variables_begin==NULL)
    {
        return NULL;
    }
    size_t variables_max=10, variables_num=0;
    char **variables=variables_begin;
    (*in)++;
    while(**in!='\0' && **in<=' ')
        (*in)++;
    while(**in!='\0' && **in!=')')
    {
        if(++variables_num==variables_max)
        {
            char **variables_new=realloc(variables_begin, sizeof(char*)*(variables_max*=2));
            if(variables_new==NULL)
            {
                for(char** variables=variables_begin; variables<variables_begin+variables_num-1; variables++)
                    free(variables);
                free(variables_begin);
                return NULL;
            }
            variables=variables_new+variables_num;
            variables_begin=variables_new;
        }
        *variables=subword(in);
        if(*variables==NULL)
        {
            for(char** variables=variables_begin; variables<variables_begin+variables_num-1; variables++)
                free(variables);
            free(variables_begin);
            return NULL;
        }
        if(**variables=='\0' && **in=='.' && *(*in+1)=='.' && *(*in+2)=='.')
        {
            *variables=concat(*variables, "__VA_ARGS__");
            if(*variables==NULL)
            {
                for(char** variables=variables_begin; variables<variables_begin+variables_num-1; variables++)
                    free(variables);
                free(variables_begin);
                return NULL;
            }
            *VA_ARGS=1;
            (*in)+=3;
        }
        variables++;
        while(**in!='\0' && **in<=' ')
            (*in)++;
        if(**in==',')
            (*in)++;
        while(**in!='\0' && **in<=' ')
            (*in)++;
    }
    *number=variables_num;
    while(**in!='\0' && **in<=' ')
        (*in)++;
    (*in)++;
    while(**in!='\0' && **in<=' ')
        (*in)++;
    return variables_begin;
}

//Funkcja dodaje do danej definicji jej rozwiniecie, oraz zapisuje w jaki konkretnie sposob ma podstwaic zmienne
bool add_value_to_define_with_variables(_define *element, char *in, char **variables_names)
{
    element->variables_occur=(int*)malloc(10*sizeof(int));
    if(element->variables_occur==NULL)
    {
        return 1;
    }
    element->variebles_position=(int*)malloc(10*sizeof(int));
    if(element->variebles_position==NULL)
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
    char *line=element->value;
    while(*in!='\0')
    {
        while(*in!='\0' && !is_letter(*in))
        {
            element->value=add_char_to_string(&line, element->value, &len, &lenmax, *in++);
            if(element->value==NULL)
            {
                return 1;
            }
        }
        char *name=subword(&in);
        if(name==NULL)
        {
            return 1;
        }
        bool name_is_variable=0;
        for(int i=0; !name_is_variable && i<element->variables_number; i++)
        {
            if(compare(name, *(variables_names+i)))
            {
                if(add_variable_occur_to_list(element, &variables_max, i, line-element->value))
                {
                    return 1;
                }
                name_is_variable=1;
            }
        }
        if(!name_is_variable)
        {
            if(compare(name, "VA_ARGS"))
            {
                if(add_variable_occur_to_list(element, &variables_max, element->variables_number, line-element->value))
                {
                    return 1;
                }
                name_is_variable=1;
            }
            else
            {
                *line='\0';
                char *new_line=concat(element->value, name);
                if(new_line==NULL)
                {
                    return 1;
                }
                lenmax=string_length(new_line)+1;
                line=new_line+lenmax-1;
                len=1;
                free(element->value);
                element->value=new_line;
            }
        }
    }
    *line='\0';
    return 0;
}

bool add_variable_occur_to_list(_define *element, int *variables_max, int i, int position)
{
    if(element->variables_occur_number==*variables_max)
    {
        (*variables_max)*=2;
        int *new_occ=(int*)realloc(element->variables_occur, (*variables_max)*sizeof(int));
        if(new_occ==NULL)
        {
            return 1;
        }
        element->variables_occur=new_occ;
        int *new_pos=(int*)realloc(element->variebles_position, (*variables_max)*sizeof(int));
        if(new_pos==NULL)
        {
            return 1;
        }
        element->variebles_position=new_pos;
    }
    *(element->variables_occur+element->variables_occur_number)=i;
    *(element->variebles_position+element->variables_occur_number)=position;
    element->variables_occur_number++;
    if(element->value==NULL)
    {
        return 1;
    }
    return 0;
}

/****************************************************************************/
/******************* UZYCIE *************************************************/
//Fukcja sprawdza, czy w podanej linii wystepuja uzycia juz zdefiniowanych makrodyrektyw #define.
//Jezeli tak, zostaja zastapione zdefiniowana wczesciej wartoscia.
char* process_define(char *in)
{
    char *line=malloc(sizeof(char)*10);
    if(line==NULL)
    {
        error_malloc();
        return NULL;
    }
    char *line_begin=line;
    size_t lenmax=10, len=lenmax;
    bool is_string=0;
    while(*in!='\0')
    {
        _define *element=root;
        while(*in!='\0' && (!is_letter(*in) || is_string))
        {
            if(*in=='"')
                is_string=!is_string;
            line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *in++);
            if(line_begin==NULL)
            {
                error_malloc();
                return NULL;
            }
        }
        char *word=subword(&in);
        if(word==NULL)
        {
            error_malloc();
            free(line_begin);
            return NULL;
        }
        char *word_begin=word;
        element=go_to_define(word_begin);
        if(element!=NULL && element->exist)
        {
            if(element->variables_exist)
            {
                while(*in!='\0' && *in<=' ')
                    in++;
                if(*in!='(')
                {
                    *line='\0';
                    char *new_line=concat(line_begin, element->value);
                    free(line_begin);
                    if(new_line==NULL)
                    {
                        error_malloc();
                        free(word_begin);
                        return NULL;
                    }
                    lenmax=string_length(new_line)+1;
                    len=1;
                    line=new_line+lenmax-1;
                    line_begin=new_line;
                    continue;
                }
                char **variables_begin=read_variables_values(&in, element->variables_number, word_begin, element->VA_ARGS);
                if(variables_begin==NULL)
                {
                    free(word_begin);
                    free(line_begin);
                    return NULL;
                }

                char *processed_define=process_define_with_variables(element, variables_begin);
                for(char **variables=variables_begin; variables<variables_begin+element->variables_number; variables++)
                    free(*variables);
                free(variables_begin);
                if(processed_define==NULL)
                {
                    free(word_begin);
                    free(line_begin);
                    return NULL;
                }
                *line='\0';
                char *new_line=process_define(processed_define);
                if(new_line==NULL)
                {
                    free(processed_define);
                    free(word_begin);
                    free(line_begin);
                    return NULL;
                }
                processed_define=new_line;
                new_line=concat(line_begin, processed_define);
                free(processed_define);
                if(new_line==NULL)
                {
                    error_malloc();
                    free(word_begin);
                    free(line_begin);
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
                    error_malloc();
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
                error_malloc();
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

//Funkcja rozwija podana definicje przyjmujaca dane wartosci jako zmienne
char* process_define_with_variables(const _define *element, char **variables_names)
{
    char *line_begin=(char*)malloc(10*sizeof(char));
    if(line_begin==NULL)
    {
        return NULL;
    }
    char *line=line_begin;
    size_t len=10, len_max=len;
    int variables_number=0;
    char *in=element->value;
    while(*in!='\0')
    {
        if(variables_number<element->variables_occur_number &&
            element->value==in-*(element->variebles_position+variables_number))
        {
            char *variable=*(variables_names+*(element->variables_occur+variables_number));
            *line='\0';
            char *new_line=concat(line_begin, variable);
            free(line_begin);
            if(new_line==NULL)
            {
                return NULL;
            }
            line=new_line+string_length(new_line);
            len_max=string_length(new_line)+1;
            len=1;
            line_begin=new_line;
            variables_number++;
        }
        line_begin=add_char_to_string(&line, line_begin, &len, &len_max, *in);
        if(line_begin==NULL)
        {
            return NULL;
        }
        in++;
    }
    *line='\0';
    return line_begin;
}

//Funkcja idzie najdalej jak to mozliwe po gotowym drzewie.
//Brakujace galezie zostaja stworzone po drodze.
static _define *create_way_to_define(const char *in)
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

//Funkcja wczytuje wartosci, ktore przyjmuje dana definicja.
//Nastepnie zwraca je w posctaci tablicy.
char** read_variables_values(char **in, const int number, const char *define_name, bool VA_ARGS)
{
    char **variables_begin=malloc(number*sizeof(char*));
    if(variables_begin==NULL)
    {
        error_malloc();
        return NULL;
    }
    char **variables=variables_begin;
    int variables_number=0;
    while(**in!='\0' && **in<=' ')
        (*in)++;
    if(**in!='(')
    {
        error_define_not_enough_variables(define_name);
        free(variables_begin);
        return NULL;
    }
    (*in)++;
    while(**in!='\0' && variables_number<number && **in!=')')
    {
        while(**in!='\0' && **in<=' ')
            (*in)++;
        *variables=(char*)malloc(10*sizeof(char));
        if(*variables==NULL)
        {
            error_malloc();
            for(variables=variables_begin; variables<variables_begin+variables_number-1; variables++)
                free(*variables);
            free(variables_begin);
            return NULL;
        }
        int brackets=0;
        char *name=*(variables);
        size_t len=10, len_max=10;
        while(**in!='\0' && !(brackets==0 && (**in==')' || **in==',' )))
        {
            if(**in=='(')
                brackets++;
            else if(**in==')')
                brackets--;
            *variables=add_char_to_string(&name, *variables, &len, &len_max, **in);
            if(*variables==NULL)
            {
                error_malloc();
                for(variables=variables_begin; variables<variables_begin+variables_number-1; variables++)
                    free(*variables);
                free(variables_begin);
                return NULL;
            }
            (*in)++;
        }
        *name='\0';
        variables++;
        variables_number++;
        if(**in==',')
            (*in)++;
    }
    if(variables_number==number-1 && VA_ARGS)
    {
        *variables=malloc(sizeof(char));
        **variables='\0';
        variables_number++;
    }
    if(**in!=')' && VA_ARGS)
    {
        variables--;
        while(**in!='\0' && **in<=' ')
            (*in)++;
        int brackets=0;
        size_t len=1, len_max=string_length(*variables)+1;
        char *name=*(variables)+len_max-1;
        *variables=add_char_to_string(&name, *variables, &len, &len_max, ',');
        if(*variables==NULL)
        {
            error_malloc();
            for(variables=variables_begin; variables<variables_begin+variables_number-1; variables++)
                free(*variables);
            free(variables_begin);
            return NULL;
        }
        while(**in!='\0' && !(brackets==0 && (**in==')')))
        {
            if(**in=='(')
                brackets++;
            else if(**in==')')
                brackets--;
            *variables=add_char_to_string(&name, *variables, &len, &len_max, **in);
            if(*variables==NULL)
            {
                error_malloc();
                for(variables=variables_begin; variables<variables_begin+variables_number-1; variables++)
                    free(*variables);
                free(variables_begin);
                return NULL;
            }
            (*in)++;
        }
        *name='\0';
        name=(*variables);
        if(name==NULL)
        {
            error_malloc();
            for(variables=variables_begin; variables<variables_begin+variables_number; variables++)
                free(*variables);
            free(variables_begin);
            return NULL;
        }
        *variables=name;
    }
    if(**in=='\0')
    {
        error_define_not_enough_variables(define_name);
        for(variables=variables_begin; variables<variables_begin+variables_number; variables++)
            free(*variables);
        free(variables_begin);
        return NULL;
    }
    if(**in==')' && variables_number!=number)
    {
        if(variables_number<number)
            error_define_not_enough_variables(define_name);
        else
            error_define_too_much_variables(define_name);
        for(variables=variables_begin; variables<variables_begin+variables_number; variables++)
            free(*variables);
        free(variables_begin);
        return NULL;
    }

    (*in)++;

    return variables_begin;
}

