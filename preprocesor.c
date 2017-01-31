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
    int c;
    char *name_begin=name;
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
bool cosik()///---
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
    _define *root=alloc_define();
    if(root==NULL)
    {
        free(name);
        free(address);
        close(in, out);
        return 1;
    }
    _include *includes_list=alloc_include();
    if(includes_list==NULL)
    {
        free(name);
        free(address);
        destroy_define(root);
        close(in, out);
        return 1;
    }
    if(add_include(includes_list, name))
    {
        free(name);
        free(address);
        destroy_include(includes_list);
        destroy_define(root);
        close(in, out);
        return 1;
    }
    bool res=rewrite(in, out, root, includes_list, address);
    //niezaleznie od wyniku, wszystkie zmienne zdefiniowane w tej funckji byly poprawne, wiec nalezy je normalnie zwolnic przed wyjsciem z niej

    free(name);
    free(address);
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
    new_define->variables_positions=NULL;
    new_define->variables_occur=NULL;
    return new_define;
}

//desktruktor pojedynczego elementu _define
void destroy_single_define(_define *root)
{
    if(root->exist)
    {
        if(root->value!=NULL)
            free(root->value);
        if(root->variables_positions!=NULL)
            free(root->variables_positions);
        if(root->variables_occur!=NULL)
            free(root->variables_occur);
    }
    free(root);
}

//destruktor struktury _define
void destroy_define(_define *root)
{
    if(root==NULL)
        return;
    for(int i=0; i<63; i++)
        destroy_define(root->childs[i]);
    destroy_single_define(root);
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

/*******************/
//glowna funkcja oslugujaca przepisanie calego pliku i wywolujaca pozostale funkcje
bool rewrite(FILE *in, FILE *out, _define *root, _include *includes_list, char *source)///---
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
                if(rewrite_include(line, out, root, includes_list, source))
                {
                    free(type);
                    free(line_begin);
                    return 1;
                }
            }
            else
            {
                line=rewrite_define(line, root);
                free(line_begin);
                if(line==NULL)
                {
                    free(type);
                    return 1;
                }
                line_begin=line;
                if(compare(type, "define"))
                {
                    if(add_define(line, root))
                    {
                        free(type);
                        free(line_begin);
                        return 1;
                    }
                }
                else
                {
                    fprintf(out, "#%s %s\n",type, line);
                }
            }
            free(type);
        }
        else
        {
            line=rewrite_define(line, root);///----?
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
bool rewrite_include(char *in, FILE *out, _define *define_list, _include *includes_list, char *source)
{
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
        char *full_address=concat(source, adress_begin);
        if(full_address==NULL)
        {
            error_malloc();
            return 1;
        }
        header=fopen(full_address, "r");
        if(header == NULL)
        {
            free(full_address);
            warning_no_include(adress_begin);
            return 0;
        }
        free(full_address);
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
            char *full_address=concat(include_adress, adress_begin);
            free(include_adress);

            if(full_address!=NULL)
            {
                header=fopen(full_address, "r");
                if(header!=NULL)
                {
                    tmp=0;
                }
            }
            free(full_address);
        }while(tmp);
        free(adress_begin);
    }

    bool res=rewrite(header, out, define_list, includes_list, source);
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
char* rewrite_define(char *in, _define *root)///---
{
    printf("\n----------\n%s\n-----------\n", in);
            /**/printf("0");
    char *line=malloc(sizeof(char)*10);
            /**/printf("1");
    if(line==NULL)
    {
        error_malloc();
        return NULL;
    }
    char *line_begin=line;
    size_t lenmax=10, len=lenmax;
    _define *root_copy=root;
    //char *new_in=NULL;
            /**/printf("2");
    while(*in!='\0')
    {
            /**/printf("A");
        while(*in!='\0' && !is_letter(*in))
        {
            /**/printf("a");
            line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *in++);
            /**/printf("b");
            if(line_begin==NULL)
            {
                error_malloc();
                /*if(new_in!=NULL)
                    free(new_in);*/
                return NULL;
            }
            /**/printf("c");
        }
            /**/printf("B");
        if(*in=='\0')
        {
            *line='\0';
            /*if(new_in!=NULL)
                free(new_in);*/
            return line_begin;
        }
            /**/printf("C");
        char *word=subword(&in);
            /**/printf("D");
        if(word==NULL)
        {
            error_malloc();
            /*if(new_in!=NULL)
                free(new_in);*/
            free(line_begin);
            return NULL;
        }
            /**/printf("E");
        char *word_begin=word;
            /**/printf("F");
        root=go_to_define(word, root);
            /**/printf("G");
        if(root!=NULL && root->exist)
        {
            /**/printf("H");
            if(root->variables_exist)
            {
            /**/printf("I\n");
                    ;/***if(*in!='(')
                    {
                        error_define_not_enough_variables(word);
                        /*if(new_in!=NULL)
                            free(new_in);
                        free(line_begin);
                        free(word);
                        return NULL;
                    }
                    in++;
                    //char *copy=new_in;
                    char *new_in=rewrite_define(in, root_copy);
                    /*if(copy!=NULL)
                        free(copy);
                    if(new_in==NULL)
                    {
                        /*if(new_in!=NULL)
                            free(new_in);
                        free(line_begin);
                        free(word);
                        return NULL;
                    }
                    //in=new_in;
                    char **variables_begin=read_variables_values(&in, root->variables_number, word);
                    if(variables_begin==NULL)
                    {
                        /*if(new_in!=NULL)
                            free(new_in);
                        free(word);
                        free(line_begin);
                        return NULL;
                    }

                    char *expanded_define=rewrite_define_with_variables(root, variables_begin);
                    for(char **variables=variables_begin; variables-variables_begin<root->variables_number; variables++)
                        free(*variables);
                    free(variables_begin);
                    if(expanded_define==NULL)
                    {
                        /*if(new_in!=NULL)
                            free(new_in);
                        free(word_begin);
                        free(line_begin);
                        return NULL;
                    }
                    *line='\0';
                    char *new_line=concat(line_begin, expanded_define);
                    if(new_line==NULL)
                    {
                        error_malloc();
                        free(word);
                        /*if(new_in!=NULL)
                            free(new_in);
                        free(line_begin);
                        free(expanded_define);
                        return NULL;
                    }
                    free(line_begin);
                    line_begin=new_line;*/
                }
            else
            {
            /**/printf("\nJ");
                for(int i=0; i<root->value_length; i++)
                {
            /**/printf("a");
                    line_begin=add_char_to_string(&line, line_begin, &len, &lenmax, *(root->value+i));

            /**/printf("b");
                    if(line_begin==NULL)
                    {
                        error_malloc();
                        free(word);
                        /*if(new_in!=NULL)
                            free(new_in);*/
                        return NULL;
                    }
            /**/printf("c");
                }
            }
        }
        else
        {
            /**/printf("\nK");
            *line='\0';

            /**/printf("a");
            char *new_line=concat(line_begin, word_begin);
            /**/printf("b");
            if(new_line==NULL)
            {
                error_malloc();
                free(word_begin);
                return NULL;
            }
            /**/printf("c");
            line=new_line+(line-line_begin);
            line_begin=new_line;
            /**/printf("d");
            /**/printf("e");
        }
            /**/printf("L");
        root=root_copy;
            /**/printf("M--%s||", word_begin);
        free(word_begin);
            /**/printf("N");
    }
            /**/printf("3");
    *line='\0';
            /**/printf("4\n--------------------------------\n");
    /*if(new_in!=NULL)
        free(new_in);*/
    return line_begin;
}

//funkcja wywolywana po znaleznieniu dyrektywy #define w celu dodania nowej definicji do listy
bool add_define(char *line, _define *root)
{
    while(*line<=' ')
        line++;
    char *name=subword(&line), *name_begin=name;
    ///moze wydzielic do innej funkcji
    while(root!=NULL && is_letter(*name))
    {
        if(*name>='a' && *name<='z')
        {
            if(root->childs[*name-'a']==NULL)
            {
                if((root->childs[*name-'a']=alloc_define())==NULL)
                {
                    free(name_begin);
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
                    free(name_begin);
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
                    free(name_begin);
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
                    free(name_begin);
                    error_malloc();
                    return 1;
                }
            }
            root=root->childs[62];
        }
        name++;
    }

    if(root->exist)
        waring_define_exist(name_begin);
    root->exist=1;
    if(*line=='(')
    {
        char **variables_begin;
        variables_begin=read_variables_names(&line, &root->variables_number);
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
        root->variables_exist=1;
        if(add_variables_to_define(root, line, variables_begin))
        {
            free(name_begin);
            error_malloc();
            return 1;
        }
    }
    else
    {
        root->variables_exist=0;
        while(*line!='\0' && *line<=' ')
            line++;
        root->value=malloc(sizeof(char)*20);
        if(root->value==NULL)
        {
            free(name_begin);
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
                free(name_begin);
                error_malloc();
                return 1;
            }
        }
        root->value_length=lenmax-len;
    }

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
bool add_variables_to_define(_define *root, char *line, char **variables_names)
{
    root->variables_positions=(int*)malloc(10*sizeof(int*));
    if(root->variables_positions==NULL)
    {
        return 1;
    }
    root->variables_occur=(int*)malloc(10*sizeof(int));
    if(root->variables_occur==NULL)
    {
        return 1;
    }
    root->value=(char*)malloc(10*sizeof(char));
    if(root->value==NULL)
    {
        return 1;
    }
    size_t len=10, lenmax=10;
    int variables_max=10;
    root->variables_occur_number=0;
    char *word=root->value;
    while(*line!='\0')
    {
        while(!is_letter(*line))
        {
            root->value=add_char_to_string(&word, root->value, &len, &lenmax, *line++);
            if(root->value==NULL)
            {
                return 1;
            }
        }
        char *name=subword(&line);
        if(name==NULL)
        {
            return 1;
        }
        for(int i=0; i<root->variables_number; i++)
        {
            if(compare(name, *(variables_names+i)))
            {
                if(root->variables_occur_number==variables_max)
                {
                    int *new_pos=(int*)realloc(root->variables_positions, variables_max*sizeof(int));
                    if(new_pos==NULL)
                    {
                        free(root->variables_positions);
                        return 1;
                    }
                    root->variables_positions=new_pos;

                    int *new_occ=(int*)realloc(root->variables_occur, variables_max*sizeof(int));
                    if(new_occ==NULL)
                    {
                        free(root->variables_occur);
                        return 1;
                    }
                    root->variables_occur=new_occ;
                    variables_max*=2;
                }
                *(root->variables_occur+root->variables_occur_number)=i;
                *(root->variables_positions+root->variables_occur_number)=len;
                root->value=add_char_to_string(&word, root->value, &len, &lenmax, '\n');
                root->variables_occur_number++;
                //wiemy ze cala zawartosc defina ma jedna linijke, mozemy wiec wykorzystac zank '\n' jako znak zmiennej
                if(root->value==NULL)
                {
                    return 1;
                }
            }
        }
    }
    root->value_length=len;
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
        variables++;
        if(**line==',')
            (*line)++;
        variables_number++;
    }
    if(**line==EOF)
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

char* rewrite_define_with_variables(_define *root, char **variables_names)
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
    char *in=root->value;
    while(*in!='\0')
    {
        if(*in=='\n')
        {
            char *variable=*(variables_names+*(root->variables_occur+variables_number));
            int var_len=string_length(variable);
            *line='\0';
            char *new_line=concat(line_begin, variable);
            printf("%s\n", new_line);
            if(new_line==NULL)
            {
                error_malloc();
                free(line_begin);
                return NULL;
            }
            line=new_line+(line-line_begin)+var_len;
            *line='s';
            len=len_max=string_length(new_line);
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
/*//funkcja tworzaca kopie danego defina pod danym adresem
bool copy_define(_define *d1, _define *d2)
{
    if(d1->exist)
    {
        free(d1->value);
        if(d1->exist_variebles)
            ;///free(d1->variables);
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
    ///przepisz variables;

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
