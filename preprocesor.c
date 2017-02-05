#include "preprocesor.h"

static FILE *output_file;

static char *open(FILE **input_file);
static void close(FILE *input_file);

//#define cos(a,b) (a##b)

/*int nic(int cos)
{
    printf("%d", cos);
}*/
//glowna funkcja
int main()
{
    //nic(1);
    //printf("cos(1,2) = %d", cos(1,2));
    FILE *input_file;
    char *address;
    if((address=open(&input_file))==NULL)
    {
        close(input_file);
        return 1;
    }
    char *name=address;
    if((address=split_by_last_slash(&name))==NULL)
    {
        error_malloc();
        free(name);
        close(input_file);
        return 1;
    }
    if(init_define())
    {
        free(name);
        free(address);
        close(input_file);
        return 1;
    }
    if(init_include(address))
    {
        free(name);
        free(address);
        destroy_define();
        close(input_file);
        return 1;
    }
    if(add_include(name))
    {
        free(name);
        free(address);
        destroy_include();
        destroy_define();
        close(input_file);
        return 1;
    }
    free(name);
    bool res=process(input_file);
    //niezaleznie od wyniku, wszystkie zmienne zdefiniowane w tej funckji byly poprawne, wiec nalezy je normalnie zwolnic przed wyjsciem z niej

    free(address);
    destroy_define();
    destroy_include();
    close(input_file);
    return res;
}

//Fukcja otwierajaca plik wejsciowy i wyjsciowy
static char *open(FILE **input_file)
{
    printf("What file you want to process: ");
    *input_file=output_file=NULL;
    char *name=malloc(sizeof(char)*10);
    if(name==NULL)
    {
        error_malloc();
        return NULL;
    }
    size_t lenmax=10, len=lenmax;
    int c,e;
    char *name_begin=name;
    while((c=getc(stdin))!=EOF && c<=' ');  //wczytywanie w ten sposob umozliwia nam obsluge sciezek ujetych w " lub '
    if(c=='"' || c=='\'')
    {
        e=c;
    }
    else
    {
        ungetc(c,stdin);
        e=0;
    }
    while(((c=getc(stdin))>' ' && e==0) || (c!=e && e!=0))
    {
        name_begin=add_char_to_string(&name, name_begin, &len, &lenmax, c);
        if(name_begin==NULL)
        {
            error_malloc();
            return NULL;
        }
    }
    *name='\0';
    *input_file=fopen(name_begin, "r");
    if(*input_file==NULL)
    {
        error_file_open(name_begin);
        return NULL;
    }
    char *address=name_begin;
    printf("Where you want to store the result: ");
    name=malloc(sizeof(char)*10);
    if(name==NULL)
    {
        error_malloc();
        return NULL;
    }
    name_begin=name;
    lenmax=len=10;
    while((c=getc(stdin))!=EOF && c<=' ');
    if(c=='"' || c=='\'')
    {
        e=c;
    }
    else
    {
        ungetc(c,stdin);
        e=0;
    }
    while(((c=getc(stdin))>' ' && e==0) || (c!=e && e!=0))
    {
        name_begin=add_char_to_string(&name, name_begin, &len, &lenmax, c);
        if(name_begin==NULL)
        {
            error_malloc();
            return NULL;
        }
    }
    *name='\0';
    output_file=fopen(name_begin, "w");
    if(output_file==NULL)
    {
        error_file_open(name_begin);
        return NULL;
    }
    free(name_begin);
    return address;
}

//fukcja zamykajaca plik wejsciowy i wyjsciowy
static void close(FILE *input_file)
{
    if(input_file!=NULL)
        fclose(input_file);
    if(output_file!=NULL)
        fclose(output_file);
}
char *concat_multiline(char *begin, FILE *input_file, bool *multiline_comment, char **next_line);
//glowna funkcja oslugujaca przepisanie calego pliku i wywolujaca pozostale funkcje
bool process(FILE *input_file)
{
    char *line;
    bool multiline_comment=0;
    char *next_line=NULL;
    while((line=(next_line==NULL?get_line(input_file):next_line))!=NULL && *line!='\0')
    {
        next_line=NULL;
        char *line_begin=concat_multiline(line, input_file, &multiline_comment, &next_line);
        free(line);
        if(line_begin==NULL)
        {
            error_malloc();
            return 1;
        }
        line=line_begin;
        if(*line=='#')
        {
            line++;
            while(*line<=' ')
                line++;
            char *type=subword(&line);
            if(type==NULL)
            {
                error_malloc();
                free(line_begin);
                return 1;

            }
            if(compare(type, "include"))
            {
                if(process_include(line))
                {
                    free(type);
                    free(line_begin);
                    return 1;
                }
            }
            else
            {
                if(compare(type, "define"))
                {
                    if(add_define(line))
                    {
                        free(type);
                        free(line_begin);
                        return 1;
                    }
                }
                else
                {
                    fprintf(output_file, "#%s %s\n",type, line);
                }
            }
            free(type);
        }
        else
        {
            line=process_define(line);
            free(line_begin);
            if(line==NULL)
            {
                return 1;
            }
            line_begin=line;
            fprintf(output_file, "%s\n",line);
        }
        free(line_begin);
    }
    if(line==NULL)
    {
        error_malloc();
        return 1;
    }
    free(line);
    return 0;
}

char *clearing(char *in, bool *multiline_comment)
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

char *concat_multiline(char *begin, FILE *input_file, bool *multiline_comment, char **next_line)///DO DOKONCZENIA< NAJLEPIEJ NA LINUKSIE
{
    char *line_begin=clearing(begin, multiline_comment);
    if(line_begin==NULL)
    {
        return NULL;
    }
    char *line=line_begin;
    if(*line_begin=='#')
    {
        while(*(line_begin+string_length(line_begin)-1)=='\\')
        {
            //usuwamy znak '\\' z konca linii
            *(line_begin+string_length(line_begin)-1)='\0';
            line=get_line(input_file);
            if(line==NULL)
            {
                free(line_begin);
                return NULL;
            }
            if(*line=='\0')
            {
                free(line);
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

            new_line=concat(line_begin, "\n");
            free(line_begin);
            if(new_line==NULL)
            {
                return NULL;
            }
            line_begin=new_line;
            new_line=concat(line_begin, line);
            free(line_begin);
            free(line);
            if(new_line==NULL)
            {
                return NULL;
            }
            line_begin=line=new_line;
        }
    }
    else
    {
        while(*(line_begin+string_length(line_begin)-1)!=';' && *(line_begin+string_length(line_begin)-1)!='{')
        {
            line=get_line(input_file);
            if(line==NULL)
            {
                free(line_begin);
                return NULL;
            }
            if(*line=='\0')
            {
                free(line);
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

            new_line=concat(line_begin, "\n");
            free(line_begin);
            if(new_line==NULL)
            {
                return NULL;
            }
            line_begin=new_line;
            new_line=concat(line_begin, line);
            free(line_begin);
            free(line);
            if(new_line==NULL)
            {
                return NULL;
            }
            line_begin=new_line;
        }
    }
    return line_begin;
}
