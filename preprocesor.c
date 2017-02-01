#include "preprocesor.h"

//fukcja otwierajaca plik wejsciowy i wyjsciowy
char *open(FILE **in, FILE **out)///---
{
    ///printf("Podaj adres pliku wejsciowego: ");
    *in=*out=NULL;
    char *name=malloc(sizeof(char)*10);
    if(name==NULL)
    {
        error_malloc();
        return NULL;
    }
    size_t lenmax=10, len=lenmax;
    int c,e;
    char *name_begin=name;
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
    *in=fopen(name_begin, "r");
    if(*in==NULL)
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
    *out=fopen(name_begin, "w");
    if(*out==NULL)
    {
        error_file_open(name_begin);
        return NULL;
    }
    free(name_begin);
    return address;
}

//fukcja zamykajaca plik wejsciowy i wyjsciowy
void close(FILE *in, FILE *out)
{
    if(in!=NULL)
        fclose(in);
    if(out!=NULL)
        fclose(out);
}

//glowna funkcja
bool start()///---
{
    FILE *in, *out;
    char *address;
    if((address=open(&in, &out))==NULL)
    {
        close(in, out);
        return 1;
    }
    char *name=address;
    if((address=split_by_last_slash(&name))==NULL)
    {
        error_malloc();
        free(name);
        close(in, out);
        return 1;
    }
    if(init_define())
    {
        free(name);
        free(address);
        close(in, out);
        return 1;
    }
    if(init_include(address))
    {
        free(name);
        free(address);
        destroy_define();
        close(in, out);
        return 1;
    }
    if(add_include(name))
    {
        free(name);
        free(address);
        destroy_include();
        destroy_define();
        close(in, out);
        return 1;
    }
    bool res=rewrite(in, out);
    //niezaleznie od wyniku, wszystkie zmienne zdefiniowane w tej funckji byly poprawne, wiec nalezy je normalnie zwolnic przed wyjsciem z niej

    free(name);
    free(address);
    destroy_define();
    destroy_include();
    close(in, out);
    return res;
}

//glowna funkcja oslugujaca przepisanie calego pliku i wywolujaca pozostale funkcje
bool rewrite(FILE *in, FILE *out)
{
    char *line;
    bool multiline_comment=0;
    while((line=getline(in))!=NULL && *line!='\0')
    {
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
                if(rewrite_include(line, out))
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
                    line=rewrite_define(line);
                    free(line_begin);
                    if(line==NULL)
                    {
                        error_malloc();
                        free(type);
                        return 1;
                    }
                    line_begin=line;
                    fprintf(out, "#%s %s\n",type, line);
                }
            }
            free(type);
        }
        else
        {
            line=rewrite_define(line);
            free(line_begin);
            if(line==NULL)
            {
                error_malloc();
                return 1;
            }
            line_begin=line;
            fprintf(out, "%s",line);
            if(*line!='\n' && *line!='\0')//Dzieki takim zabiegom nie pomijamy linii pustych
                fprintf(out, "\n");
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
