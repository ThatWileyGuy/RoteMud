/***********************************************************************************
 *                                                                                  *
 *          _______.____    __    ____       _______                  _______       *
 *         /       |\   \  /  \  /   /  _   |   ____|          __    |   ____|      *
 *        |   (----` \   \/    \/   /  (_)  |  |__    ____   _/  |_  |  |__         *
 *         \   \      \            /    _   |   __|  /  _ \  \   __\ |   __|        *
 *     .----)   |      \    /\    /    (_)  |  |    (  <_> )  |  |   |  |____       *
 *     |_______/        \__/  \__/          |__|     \____/   |__|   |_______|      *
 *                                                                                  *
 * SWFotE v2.0 (FotE v1.1 cleaned up and considerably modded)  by:                  *
 * Greg (Keberus) Mosley                                                            *
 * Roman (Trelar) Arnold                                                            *
 *                                                                                  *
 * SWFotE v1 & v1.1 copyright (c) 2002 was created by                               *
 * Chris 'Tawnos' Dary (cadary@uwm.edu),                                            *
 * Korey 'Eleven' King (no email),                                                  *
 * Matt 'Trillen' White (mwhite17@ureach.com),                                      *
 * Daniel 'Danimal' Berrill (danimal924@yahoo.com),                                 *
 * Richard 'Bambua' Berrill (email unknown),                                        *
 * Stuart 'Ackbar' Unknown (email unknown)                                          *
 *                                                                                  *
 * SWR 1.0 copyright (c) 1997, 1998 was created by Sean Cooper                      *
 * based on a concept and ideas from the original SWR immortals:                    *
 * Himself (Durga), Mark Matt (Merth), Jp Coldarone (Exar), Greg Baily (Thrawn),    *
 * Ackbar, Satin, Streen and Bib as well as much input from our other builders      *
 * and players.                                                                     *
 *                                                                                  *
 * Original SMAUG 1.4a written by Thoric (Derek Snider) with Altrag,                *
 * Blodkai, Haus, Narn, Scryn, Swordbearer, Tricops, Gorog, Rennard,                *
 * Grishnakh, Fireblade, and Nivek.                                                 *
 *                                                                                  *
 * Original MERC 2.1 code by Hatchet, Furey, and Kahn.                              *
 *                                                                                  *
 * Original DikuMUD code by: Hans Staerfeldt, Katja Nyboe, Tom Madsen,              *
 * Michael Seifert, and Sebastian Hammer.                                           *
 *                                                                                  *
 ***********************************************************************************/

module;

#include "mud.hxx"

export module hashstr;

import mud;

#define STR_HASH_SIZE 1024

struct HASHSTR_DATA
{
    HASHSTR_DATA* next;        /* next hash element */
    unsigned short int links;  /* number of links to this string */
    unsigned short int length; /* length of string */
};

char* str_alloc(char const* str);
char* quick_link(char* str);
int str_free(char* str);
void show_hash(int count);
char* hash_stats(void);
char* check_hash(char* str);
void hash_dump(int hash);
void show_high_hash(int top);
bool in_hash_table(char* str);

HASHSTR_DATA* string_hash[STR_HASH_SIZE];

/*
 * Check hash table for existing occurance of string.
 * If found, increase link count, and return pointer,
 * otherwise add new string to hash table, and return pointer.
 */
export char* str_alloc(char const* str)
{
    int len, hash, psize;
    HASHSTR_DATA* ptr;

    len = strlen(str);
    psize = sizeof(HASHSTR_DATA);
    hash = len % STR_HASH_SIZE;
    for (ptr = string_hash[hash]; ptr; ptr = ptr->next)
        if (len == ptr->length && !strcmp(str, (char*)ptr + psize))
        {
            if (ptr->links < 65535)
                ++ptr->links;
            return (char*)ptr + psize;
        }
    ptr = (HASHSTR_DATA*)malloc(len + psize + 1);
    ptr->links = 1;
    ptr->length = len;
    if (len)
        strcpy((char*)ptr + psize, str);
    /*     memcpy( (char *) ptr+psize, str, len+1 ); */
    else
        strcpy((char*)ptr + psize, "");
    ptr->next = string_hash[hash];
    string_hash[hash] = ptr;
    return (char*)ptr + psize;
}

/*
 * Used to make a quick copy of a string pointer that is known to be already
 * in the hash table.  Function increments the link count and returns the
 * same pointer passed.
 */
export char* quick_link(char* str)
{
    HASHSTR_DATA* ptr;

    ptr = (HASHSTR_DATA*)(str - sizeof(HASHSTR_DATA));
    if (ptr->links == 0)
    {
        fprintf(stderr, "quick_link: bad pointer\n");
        return NULL;
    }
    if (ptr->links < 65535)
        ++ptr->links;
    return str;
}

/*
 * Used to remove a link to a string in the hash table.
 * If all existing links are removed, the string is removed from the
 * hash table and disposed of.
 * returns how many links are left, or -1 if an error occurred.
 */
export int str_free(char* str)
{
    int len, hash;
    HASHSTR_DATA *ptr, *ptr2, *ptr2_next;
    len = strlen(str);
    hash = len % STR_HASH_SIZE;
    ptr = (HASHSTR_DATA*)(str - sizeof(HASHSTR_DATA));
    if (ptr->links == 65535) /* permanent */
        return ptr->links;
    if (ptr->links == 0)
    {
        fprintf(stderr, "str_free: bad pointer\n");
        return -1;
    }
    if (--ptr->links == 0)
    {
        if (string_hash[hash] == ptr)
        {
            string_hash[hash] = ptr->next;
            free(ptr);
            return 0;
        }
        for (ptr2 = string_hash[hash]; ptr2; ptr2 = ptr2_next)
        {
            ptr2_next = ptr2->next;
            if (ptr2_next == ptr)
            {
                ptr2->next = ptr->next;
                free(ptr);
                return 0;
            }
        }
        fprintf(stderr, "str_free: pointer not found for string: %s\n", str);
        return -1;
    }
    return ptr->links;
}

export void show_hash(int count)
{
    HASHSTR_DATA* ptr;
    int x, c;

    for (x = 0; x < count; x++)
    {
        for (c = 0, ptr = string_hash[x]; ptr; ptr = ptr->next, c++)
            ;
        fprintf(stderr, " %d", c);
    }
    fprintf(stderr, "\n");
}

export void hash_dump(int hash)
{
    HASHSTR_DATA* ptr;
    char* str;
    int c, psize;

    if (hash > STR_HASH_SIZE || hash < 0)
    {
        fprintf(stderr, "hash_dump: invalid hash size\n\r");
        return;
    }
    psize = sizeof(HASHSTR_DATA);
    for (c = 0, ptr = string_hash[hash]; ptr; ptr = ptr->next, c++)
    {
        str = ((char*)ptr) + psize;
        fprintf(stderr, "Len:%4d Lnks:%5d Str: %s\n\r", ptr->length, ptr->links, str);
    }
    fprintf(stderr, "Total strings in hash %d: %d\n\r", hash, c);
}

export char* check_hash(char* str)
{
    static char buf[1024];
    int len, hash, psize, p, c;
    HASHSTR_DATA *ptr, *fnd;

    buf[0] = '\0';
    len = strlen(str);
    psize = sizeof(HASHSTR_DATA);
    hash = len % STR_HASH_SIZE;
    for (fnd = NULL, ptr = string_hash[hash], c = 0; ptr; ptr = ptr->next, c++)
        if (len == ptr->length && !strcmp(str, (char*)ptr + psize))
        {
            fnd = ptr;
            p = c + 1;
        }
    if (fnd)
        sprintf_s(buf, "Hash info on string: %s\n\rLinks: %d  Position: %d/%d  Hash: %d  Length: %d\n\r", str,
                  fnd->links, p, c, hash, fnd->length);
    else
        sprintf_s(buf, "%s not found.\n\r", str);
    return buf;
}

export char* hash_stats(void)
{
    static char buf[1024];
    HASHSTR_DATA* ptr;
    int x, c, total, totlinks, unique, bytesused, wouldhave, hilink;

    totlinks = unique = total = bytesused = wouldhave = hilink = 0;
    for (x = 0; x < STR_HASH_SIZE; x++)
    {
        for (c = 0, ptr = string_hash[x]; ptr; ptr = ptr->next, c++)
        {
            total++;
            if (ptr->links == 1)
                unique++;
            if (ptr->links > hilink)
                hilink = ptr->links;
            totlinks += ptr->links;
            bytesused += (ptr->length + 1 + sizeof(HASHSTR_DATA));
            wouldhave += (sizeof(HASHSTR_DATA) + (ptr->links * (ptr->length + 1)));
        }
    }
    sprintf_s(buf,
              "Hash strings allocated:%8d  Total links  : %d\n\rString bytes allocated:%8d  Bytes saved  : "
              "%d\n\rUnique (wasted) links :%8d  Hi-Link count: %d\n\r",
              total, totlinks, bytesused, wouldhave - bytesused, unique, hilink);
    return buf;
}

export void show_high_hash(int top)
{
    HASHSTR_DATA* ptr;
    int x, psize;
    char* str;

    psize = sizeof(HASHSTR_DATA);
    for (x = 0; x < STR_HASH_SIZE; x++)
        for (ptr = string_hash[x]; ptr; ptr = ptr->next)
            if (ptr->links >= top)
            {
                str = ((char*)ptr) + psize;
                fprintf(stderr, "Links: %5d  String: >%s<\n\r", ptr->links, str);
            }
}

export bool in_hash_table(char* str)
{
    int len, hash, psize;
    HASHSTR_DATA* ptr;

    len = strlen(str);
    psize = sizeof(HASHSTR_DATA);
    hash = len % STR_HASH_SIZE;
    for (ptr = string_hash[hash]; ptr; ptr = ptr->next)
        if (len == ptr->length && str == ((char*)ptr + psize))
            return true;
    return false;
}
