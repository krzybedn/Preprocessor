#include "preprocesor.h"

//fukcja otwierajaca plik wejsciowy i wyjsciowy
bool open(FILE **in, FILE **out)///---
{
    ///printf("Podaj adres pliku wejsciowego: ");

    char *name=malloc(sizeof(char)*10), *name_begin=name;
    size_t lenmax=10, len=lenmax;
    int c=99;
    while((c=getc(stdin))>' ')
    {
        name_begin=add_char_to_string(&name, name_begin, &len, &lenmax, c);
        if(name_begin==NULL)
        {
            error_malloc();
            return NULL;
        }
    }
    *name='\0';
    *in = fopen(name_begin, "r");
    if(*in == NULL)
    {
        error_file_open(name_begin);
        return 1;
    }
    free(name_begin);

    ///printf("Podaj adres pliku docelowego: ");

    name=malloc(sizeof(char)*10);
    name_begin=name;
    lenmax=10;
    len=lenmax;
    c=99;
    while((c=getc(stdin))>' ')
    {
        name_begin=add_char_to_string(&name, name_begin, &len, &lenmax, c);
        if(name_begin==NULL)
        {
            error_malloc();
            return NULL;
        }
    }
    *name='\0';
    *out = fopen(name_begin, "w");
    if(*out == NULL)
    {
        error_file_open(name_begin);
        return 1;
    }
    free(name_begin);
    return 0;
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
bool cosik()///---
{
    FILE *in, *out;
    if(open(&in, &out))
    {
        close(in, out);
        return 1;
    }
    _define *root=alloc_define();
    if(root==NULL)
    {
        close(in, out);
        return 1;
    }
    _include *includes_list=alloc_include();
    if(includes_list==NULL)
    {
        destroy_define(root);
        close(in, out);
        return 1;
    }
    bool res=rewrite(in, out, root, includes_list);
    //niezaleznie od wyniku, wszystkie zmienne zdefiniowane w tej funckji byly poprawne, wiec nalezy je normalnie zwolnic przed wyjsciem z niej

    destroy_define(root);
    destroy_include(includes_list);
    close(in, out);
    return res;
}

//konstruktor struktury _define
_define* alloc_define()
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
    return new_define;
}

//destruktor struktury _define
void destroy_define(_define *root)
{
    if(root==NULL)
        return;
    for(int i=0; i<63; i++)
        destroy_define(root->childs[i]);
    free(root);
}

//konstruktor struktury _include
_include* alloc_include()
{
    _include *new_include=malloc(sizeof(_include));
    if(new_include==NULL)
    {
        error_malloc();
        return NULL;
    }
    for(int i=0; i<128; i++)
        new_include->childs[i]=NULL;
    new_include->exist=0;
    return new_include;
}

//destruktor struktury _include
void destroy_include(_include *root)
{
    if(root==NULL)
        return;
    for(int i=0; i<128; i++)
        destroy_include(root->childs[i]);
    free(root);
}

//glowna funkcja oslugujaca przepisanie calego pliku i wywolujaca pozostale funkcje
bool rewrite(FILE *in, FILE *out, _define *root, _include *includes_list)///---
{
    char *line;
    bool multiline_comment=0;
    while((line=getline(in))!=NULL && *line!='\0')
    {
        char *line_begin=line;
        printf("%s\n", line);
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
            if(compare(type, "include"))///
            {
                ///fprintf(out, "#%s %s\n", type, line);
                if(rewrite_include(line, out, root, includes_list))
                {
                    free(type);
                    free(line_begin);
                    return 1;
                }///*/
            }
            else if(compare(type, "define"))///
            {
                ///fprintf(out, "#%s %s\n", type, line);
                if(add_define(line, root))
                {
                    free(type);
                    free(line_begin);
                    return 1;
                }///*/
            }
            else
            {
                line=rewrite_define(line, root, &multiline_comment);
                free(line_begin);
                if(line==NULL)
                {
                    free(type);
                    return 1;
                }
                line_begin=line;
                fprintf(out, "#%s %s\n",type, line);
            }
            free(type);
        }
        else
        {
            line=rewrite_define(line, root, &multiline_comment);
            free(line_begin);
            if(line==NULL)
            {
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

//fukcja wywolywana po znalezieniu dyrektywy #include i pozwalajaca otworzyc odpowiedni plik naglowkowy i go przepisac
bool rewrite_include(char *in, FILE *out, _define *define_list, _include *includes_list)
{
    ///sprawdzic powtorke lub petle
    while(*in<=' ')
        in++;
    if(*in=='\0')
    {
        error_empty_include_name();
        return 1;
    }

    char *adress_begin;
    FILE *header;
    char end=*in;
    if(end=='<')
        end='>';
    in++;
    char *adress=malloc(sizeof(char)*10);
    if(adress==NULL)
    {
        error_malloc();
        return 1;
    }
    adress_begin=adress;
    size_t len=10, lenmax=len;
    while(*in!='\0' && *in!=end)
    {
        adress_begin=add_char_to_string(&adress, adress_begin, &len, &lenmax, *in++);
        if(adress_begin==NULL)
        {
            error_malloc();
            free(adress_begin);
            return NULL;
        }
    }
    if(*in=='\0')
    {
        error_endless_name(adress_begin);
        return 1;
    }
    *adress='\0';
    if(exist_incude(includes_list, adress_begin))
    {
        free(adress_begin);
        return 0;
    }
    add_include(includes_list, adress_begin);

    if(end=='"')
    {
        header=fopen(adress_begin, "r");
        if(header == NULL)
        {
            warning_no_include(adress_begin);
            return 0;
        }
        free(adress_begin);
    }
    else if(*in=='>')
    {
        bool tmp=1;
        FILE *includes=fopen("include.txt", "r");
        if(includes == NULL)
        {
            error_file_open("include.txt");
            return 1;
        }

        do
        {
            char *include_adress=getline(includes);
            if(*include_adress=='\0')
            {
                warning_no_include(adress_begin);
                return 0;
            }
            char *adress=concat(include_adress, adress_begin);
            free(include_adress);

            if(adress!=NULL)
            {
                header=fopen(adress, "r");
                if(header!=NULL)
                {
                    tmp=0;
                }
            }
        }while(tmp);
        free(adress_begin);
    }

    bool res=rewrite(header, out, define_list, includes_list);
    fclose(header);
    return res;
}

//fukcja pomocnicza sluzaca do przejscia gotowego drzewa strukury _define i zwrocenie danego defina
_define *go_to_define(char *in, _define *root)
{
    while(root!=NULL && *in!='\0')
    {
        if(*in>='a' && *in<='z')
            root=root->childs[*in-'a'];
        else if(*in>='A' && *in<='Z')
            root=root->childs[26+*in-'A'];
        else if(*in>='0' && *in<='9')
            root=root->childs[52+*in-'0'];
        else
            root=root->childs[62];
        in++;
    }
    return root;
}

//fukcja wywolywana w celu sprawdzenia, czy w danej linii wystepuja uzycia juz zdefiniowanych makrodyrektyw #define
char* rewrite_define(char *in, _define *root, bool *multiline_comment)///---
{
    char *line=malloc(sizeof(char)*10);
    if(line==NULL)
    {
        error_malloc();
        return NULL;
    }
    char *line_begin=line;
    size_t lenmax=10, len=lenmax;
    _define *root_copy=root;
    while(*in!='\0')
    {
        while(*in!='\0' && !is_letter(*in))
        {
            if(!*multiline_comment && *in=='/')
            {
                if(*(in+1)=='/')
                {
                    *line='\0';
                    return line;
                }
                else if(*(in+1)=='*')
                {
                    *multiline_comment=1;
                    in+=2;
                }
                else
                {
                    line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *in++);
                    if(line_begin==NULL)
                    {
                        error_malloc();
                        return NULL;
                    }
                }
            }
            else if(*multiline_comment)
            {
                if(*in=='*' && *(in+1)=='/')
                {
                    *multiline_comment=0;
                    in++;
                }
                in++;
            }
            else
            {
                line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *in++);
                if(line_begin==NULL)
                {
                    error_malloc();
                    return NULL;
                }
            }
        }
        if(*in=='\0')
        {
            *line='\0';
            return line_begin;
        }
        char *word=subword(&in);
        if(!*multiline_comment)
        {
            root=go_to_define(word, root);
            if(root!=NULL && root->exist)
            {
                if(root->exist_variebles)
                {
                    ///podmiana zmiennych
                }
                else
                {
                    for(int i=0; i<root->value_length; i++)
                    {
                        line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *(root->value+i));
                        if(line_begin==NULL)
                        {
                            error_malloc();
                            return NULL;
                        }
                    }
                }
            }
            else
            {
                char *word_begin=word;
                while(*word!='\0')
                {
                    line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *word++);
                    if(line_begin==NULL)
                    {
                        error_malloc();
                        free(word_begin);
                        return NULL;
                    }
                }
                free(word_begin);
            }
            root=root_copy;
        }
    }
    *line='\0';
    return line_begin;
}

//funkcja wywolywana po znaleznieniu dyrektywy #define w celu dodania nowej definicji do listy
bool add_define(char *line, _define *root)
{
    while(*line<=' ')
        line++;
    char *name=subword(&line), *name_copy=name;
    ///moze wydzielic do innej funkcji
    while(root!=NULL && is_letter(*name))
    {
        if(*name>='a' && *name<='z')
        {
            if(root->childs[*name-'a']==NULL)
            {
                if((root->childs[*name-'a']=alloc_define())==NULL)
                {
                    free(name_copy);
                    error_malloc();
                    return 1;
                }
            }
            root=root->childs[*name-'a'];
        }
        else if(*name>='A' && *name<='Z')
        {
            if(root->childs[26+*name-'A']==NULL)
            {
                if((root->childs[26+*name-'A']=alloc_define())==NULL)
                {
                    free(name_copy);
                    error_malloc();
                    return 1;
                }
            }
            root=root->childs[26+*name-'A'];
        }
        else if(*name>='0' && *name<='9')
        {
            if(root->childs[52+*name-'0']==NULL)
            {
                if((root->childs[52+*name-'0']=alloc_define())==NULL)
                {
                    free(name_copy);
                    error_malloc();
                    return 1;
                }
            }
            root=root->childs[52+*name-'0'];
        }
        else if(*name=='_')
        {
            if(root->childs[62]==NULL)
            {
                if((root->childs[62]=alloc_define())==NULL)
                {
                    free(name_copy);
                    error_malloc();
                    return 1;
                }
            }
            root=root->childs[62];
        }
        name++;
    }

    if(root->exist)
        waring_define_exist(name_copy);

    if(*line=='(')
    {
        ///parametr
        root->exist_variebles=1;
    }
    else
    {
        root->exist_variebles=0;
    }
    while(*line!='\0' && *line<=' ')
        line++;
    root->exist=1;
    root->value=malloc(sizeof(char)*20);
    if(root->value==NULL)
    {
        free(name_copy);
        error_malloc();
        return 1;
    }
    char *word=root->value;
    size_t lenmax=20, len=lenmax;
    while(*line!='\0')
    {
        root->value=add_char_to_string(&word, root->value, &len, &lenmax, *line++);
        if(root->value==NULL)
        {
            free(name_copy);
            error_malloc();
            return 1;
        }
    }
    root->value_length=lenmax-len;
    return 0;
}

//funkcja sprawdza czy dany plik naglowkowy zostal juz dodany
bool exist_incude(_include *include_root, char *name)
{
    while(include_root!=NULL && *name!='\0')
    {
        include_root=include_root->childs[(int)*name];
        name++;
    }

    return (*name=='\0' && include_root!=NULL && include_root->exist);
}

//funkcja dodaje nowy plik naglowkowy do listy plikow, ktore juz wystapily
bool add_include(_include *include_root, char *name)
{
    if(include_root==NULL)
        return 1;
    while(*name!='\0')
    {
        if(include_root->childs[(int)*name]==NULL)
        {
            include_root->childs[(int)*name]=alloc_include();
            if(include_root->childs[(int)*name]==NULL)
            {
                error_malloc();
                return 1;
            }
        }
        include_root=include_root->childs[(int)*name];
        name++;
    }
    include_root->exist=1;
    return 0;
}

/*//funkcja tworzaca kopie danego defina pod danym adresem
bool copy_define(_define *d1, _define *d2)
{
    if(d1->exist)
    {
        free(d1->value);
        if(d1->exist_variebles)
            ;///free(d1->variebles);
    }
    d1->exist=1;
    d1->value_length=d2->value_length;
    if((d1->value=malloc(sizeof(char)*d1->value_length))==NULL)
    {
        error_malloc();
        return 1;
    }
    for(int i=0; i<d1->value_length; i++)
        *(d1->value+i)=*(d2->value+i);
    ///przepisz variebles;

    return 0;
}



//funkcja laczaca dwie listy _define w jedna
bool merge_define(_define *d1, _define *d2)
{
    if(d1==NULL)
        return 0;
    if(d2->exist)
        copy_define(d1, d2);
    for(int i=0; i<63; i++)
    {
        if(d2->childs[i]!=NULL)
        {
            if(d1->childs[i]==NULL && d2->childs[i]!=NULL)
            {
                if((d1->childs[i]=alloc_define())==NULL)
                {
                    error_malloc();
                    return 1;
                }
            }
            if(merge_define(d1->childs[i], d2->childs[i]))
                return 1;
        }
    }
    return 0;
}
*/
