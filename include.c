#include "include.h"

static _include *root;
static const char *source;


static _include* alloc_include();
static void destroy_all_includes(_include *element);
static bool exist_incude(const char *name);
static FILE *includes;

bool init_include(const char *address)
{
    includes=fopen("include.txt", "r");
    if(includes == NULL)
    {
        error_file_open("include.txt");
        return 1;
    }
    source=address;
    if(root!=NULL)
        destroy_include();
    root=alloc_include();
    if(root==NULL)
        return 1;
    else
        return 0;
}

//konstruktor struktury _include
static _include* alloc_include()
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

void destroy_include()
{
    fclose(includes);
    destroy_all_includes(root);
}

//destruktor struktury _include
static void destroy_all_includes(_include *element)
{
    if(element==NULL)
        return;
    for(int i=0; i<128; i++)
        destroy_all_includes(element->childs[i]);
    free(element);
}

//fukcja wywolywana po znalezieniu dyrektywy #include i pozwalajaca otworzyc odpowiedni plik naglowkowy i go przepisac
bool expand_include(const char *in)
{
    while(*in<=' ')
        in++;
    if(*in=='\0')
    {
        error_empty_include_name();
        return 1;
    }

    char end;
    if(*in=='<')
    {
        end='>';
    }
    else if(*in=='"')
    {
        end='"';
    }
    else
    {
        error_include_type(*in);
        return 1;
    }
    in++;
    char *adress=malloc(sizeof(char)*10);
    if(adress==NULL)
    {
        error_malloc();
        return 1;
    }
    char *address_begin=adress;
    FILE *header;
    size_t len=10, lenmax=len;
    while(*in!='\0' && *in!=end)
    {
        char *new_adress=add_char_to_string(&adress, address_begin, &len, &lenmax, *in++);
        if(address_begin==NULL)
        {
            error_malloc();
            free(address_begin);
            return NULL;
        }
        address_begin=new_adress;
    }
    if(*in=='\0')
    {
        error_endless_name(address_begin);
        return 1;
    }
    *adress='\0';
    if(exist_incude(address_begin))
    {
        free(address_begin);
        return 0;
    }
    add_include(address_begin);

    if(end=='"')
    {
        char *full_address=concat(source, address_begin);
        free(address_begin);
        if(full_address==NULL)
        {
            error_malloc();
            return 1;
        }
        header=fopen(full_address, "r");
        if(header == NULL)
        {
            free(full_address);
            warning_no_include(address_begin);
            return 0;
        }
        free(full_address);
    }
    else if(*in=='>')
    {
        bool tmp=1;
        do
        {
            char *include_adress=get_line(includes);
            if(*include_adress=='\0')
            {
                warning_no_include(address_begin);
                free(address_begin);
                fclose(includes);
                return 0;
            }
            char *full_address=concat(include_adress, address_begin);
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
        rewind(includes);
        free(address_begin);
    }

    bool res=expand(header);
    fclose(header);
    return res;
}


//funkcja sprawdza czy dany plik naglowkowy zostal juz dodany
static bool exist_incude(const char *name)
{
    _include *element=root;
    while(element!=NULL && *name!='\0')
    {
        element=element->childs[(int)*name];
        name++;
    }

    return (*name=='\0' && element!=NULL && element->exist);
}

//funkcja dodaje nowy plik naglowkowy do listy plikow, ktore juz wystapily
bool add_include(const char *name)
{
    _include *element=root;
    while(*name!='\0')
    {
        if(element->childs[(int)(*name)]==NULL)
        {
            if((element->childs[(int)(*name)]=alloc_include())==NULL)
            {
                error_malloc();
                return 1;
            }
        }
        element=element->childs[(int)(*name)];
        name++;
    }
    element->exist=1;
    return 0;
}
