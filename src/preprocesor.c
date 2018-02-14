/*
 *  Krzysztof Bednarek
 */

#include "preprocesor.h"

static FILE *output_file;

static char *open_from_command_line(FILE **input_file);
static char *open_from_program_arguments(FILE **input_file, char *input_address, char *output_address);
static void close(FILE *input_file);

int main(int argc, char * argv[])
{
    FILE *input_file;
    char *address;
    if(argc==1)
    {
        if((address=open_from_command_line(&input_file))==NULL)
        {
            close(input_file);
            return 1;
        }
    }
    else if(argc==3)
    {
        address=open_from_program_arguments(&input_file, argv[1], argv[2]);
        if(address==NULL)
        {
            close(input_file);
            return 1;
        }
    }
    else
    {
        error_main_arguments();
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

//Fukcja otwierajaca plik wejsciowy i wyjsciowy, gdy nie zosta³y one przekazane przy uruchamianiu programu
static char *open_from_command_line(FILE **input_file)
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

//Funkcja otwierajaca plik wejsciowy i wejsciowy przekazane jako argument wejsciowy programu
static char *open_from_program_arguments(FILE **input_file, char *input_address, char *output_address)
{
    output_file=NULL;
    *input_file=fopen(input_address, "r");
    if(*input_file==NULL)
    {
        error_file_open(input_address);
        return NULL;
    }
    output_file=fopen(output_address, "w");
    if(output_file==NULL)
    {
        error_file_open(output_address);
        return NULL;
    }
    char *address=malloc(string_length(input_address)*sizeof(char));
    if(address==NULL)
    {
        error_malloc();
        return NULL;
    }
    string_copy(address, input_address);
    return address;
}

//Fukcja zamykajaca plik wejsciowy i wyjsciowy
static void close(FILE *input_file)
{
    if(input_file!=NULL)
        fclose(input_file);
    if(output_file!=NULL)
        fclose(output_file);
}

//Glowna funkcja obslugujaca przepisanie calego pliku i wywolujaca pozostale funkcje
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

