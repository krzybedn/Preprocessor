#include "preprocesor.h"

static FILE *output_file;

static char *open(FILE **input_file);
static void close(FILE *input_file);

//glowna funkcja
bool start()
{
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
    bool res=expand(input_file);
    //niezaleznie od wyniku, wszystkie zmienne zdefiniowane w tej funckji byly poprawne, wiec nalezy je normalnie zwolnic przed wyjsciem z niej

    free(address);
    destroy_define();
    destroy_include();
    close(input_file);
    return res;
}

//fukcja otwierajaca plik wejsciowy i wyjsciowy
static char *open(FILE **input_file)///---
{
    ///printf("Podaj adres pliku wejsciowego: ");
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
    //wczytywanie w ten sposob umozliwia nam obsluge sciezek ujetych w " lub '
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
    *input_file=fopen(name_begin, "r");
    if(*input_file==NULL)
    {
        error_file_open(name_begin);
        return NULL;
    }
    char *address=name_begin;
    ///printf("Podaj adres pliku docelowego: ");
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

//glowna funkcja oslugujaca przepisanie calego pliku i wywolujaca pozostale funkcje
bool expand(FILE *input_file)
{
    char *line;
    bool multiline_comment=0;
    while((line=get_line(input_file))!=NULL && *line!='\0')
    {
        if(compare(line, "_CRTIMP int __mingw_stdio_redirect__(fprintf)(FILE*, const char*, ...);"))
            printf("___________\n");
        char *line_begin=delete_comments(line, &multiline_comment);
        if(line_begin==NULL)
        {
            error_malloc();
            free(line);
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
                ///printf("include\n");
                if(expand_include(line))
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
                ///printf("define %s\n", line);
                    if(add_define(line))
                    {
                        free(type);
                        free(line_begin);
                        return 1;
                    }
                }
                else if(compare(type, "undef"))
                {
                ///printf("undef %s\n", line);
                    fprintf(output_file, "#%s %s\n",type, line);
                }
                else
                {
               /// printf("inne %s\n", line);
                    line=expand_define(line);
                    free(line_begin);
                    if(line==NULL)
                    {
                        error_malloc();
                        free(type);
                        return 1;
                    }
                    line_begin=line;
                    fprintf(output_file, "#%s %s\n",type, line);
                }
            }
            free(type);
        }
        else
        {
               /// printf("ogolne %s\n", line);
            line=expand_define(line);
            free(line_begin);
            if(line==NULL)
            {
                error_malloc();
                return 1;
            }
            line_begin=line;
            fprintf(output_file, "%s",line);
            if(*line!='\n' && *line!='\0')//Dzieki takim zabiegom nie pomijamy linii pustych
                fprintf(output_file, "\n");
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
