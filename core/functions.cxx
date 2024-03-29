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

#include "mud.hxx"

/*
   Original Code from SW:FotE 1.1
   Reworked strrep function.
   Fixed a few glaring errors. It also will not overrun the bounds of a string.
   -- Xorith
*/
char* strrep(const char* src, const char* sch, const char* rep)
{
    int lensrc = strlen(src), lensch = strlen(sch), lenrep = strlen(rep), x, y, in_p;
    static char newsrc[MAX_STRING_LENGTH];
    bool searching = false;

    newsrc[0] = '\0';
    for (x = 0, in_p = 0; x < lensrc; x++, in_p++)
    {
        if (src[x] == sch[0])
        {
            searching = true;
            for (y = 0; y < lensch; y++)
                if (src[x + y] != sch[y])
                    searching = false;

            if (searching)
            {
                for (y = 0; y < lenrep; y++, in_p++)
                {
                    if (in_p == (MAX_STRING_LENGTH - 1))
                    {
                        newsrc[in_p] = '\0';
                        return newsrc;
                    }
                    newsrc[in_p] = rep[y];
                }
                x += lensch - 1;
                in_p--;
                searching = false;
                continue;
            }
        }
        if (in_p == (MAX_STRING_LENGTH - 1))
        {
            newsrc[in_p] = '\0';
            return newsrc;
        }
        newsrc[in_p] = src[x];
    }
    newsrc[in_p] = '\0';
    return newsrc;
}

char* strlinwrp(char* src, int length)
{
    int srclen, in_p, x, last_line;
    char newstr[MAX_STRING_LENGTH];
    bool looking;
    if (!length)
        length = 60;
    if (!src)
    {
        newstr[0] = '\0';
        return str_dup(newstr);
    }
    srclen = strlen(src);
    in_p = 0;
    // STRFREE(newstr); Commented out - was just defined!
    looking = false;
    for (x = 0, last_line = 0; x < srclen; x++, last_line++)
    {
        if (src[x] == '\r' || src[x] == '\n')
        {
            last_line = 0;
            looking = false;
        }
        else if (last_line % length == 0 && x != 0)
            looking = true;
        if (looking)
        {
            if (src[x] == ' ')
            {
                newstr[in_p++] = '\r';
                newstr[in_p++] = '\n';
                looking = false;
                last_line = 0;
                if (src[x + 1] == ' ')
                    x++;
                continue;
            }
        }
        newstr[in_p++] = src[x];
    }
    newstr[in_p++] = '\r';
    newstr[in_p++] = '\n';
    newstr[in_p++] = '\0';
    return str_dup(newstr);
}

std::string remand(const std::string_view& arg)
{
    std::string result;
    result.reserve(arg.size());

    for (auto iter = arg.begin(); iter != arg.end(); iter++)
    {
        if (*iter == '&' && (iter + 1) != arg.end())
            iter++;
        else if (*iter == '^' && (iter + 1) != arg.end())
            iter++;
        else
        {
            result.push_back(*iter);
        }
    }

    return result;
}

char* rembg(const char* src)
{
    static char ret[MAX_STRING_LENGTH];
    char* retptr;
    retptr = ret;

    if (src == nullptr)
        return nullptr;

    for (; *src != '\0'; src++)
    {
        if (*src == '^' && *(src + 1) != '\0')
            src++;
        else
        {
            *retptr = *src;
            retptr++;
        }
    }

    *retptr = '\0';
    return ret;
}

/*char *chrmax( char *src, int length )
{

  int srclen,in_p,i,count;
  char newstr[MAX_STRING_LENGTH];
  bool looking;
  srclen = strlen(src);
  in_p=0; STRFREE(newstr); count=0;
  looking=false;
  for(i=0,in_p=0;i<srclen;i++,in_p++)
  {

      if (count >= length){
        newstr[in_p] = '\0';
        return str_dup( newstr );
      }

    if (src[i] == '&'){
        looking = true;

        if (src[i++] == '&')
          count++;
          i--;
    }
    if (looking){
          looking = false;
            newstr[in_p] = src[i];
            in_p++;
            i++;
        newstr[in_p] = src[i];
    }
    else {
        newstr[in_p] = src[i];
        count++;

    }
  }
  if (count < length){
    for(i=0;i<length-count-1;i++,in_p++){
      newstr[in_p] = ' ';
    }
  }
  newstr[in_p] = '\0';
  return str_dup( newstr );
}
*/

char* chrmax(char* src, int length)
{
    int i, len;
    static char ret[MAX_STRING_LENGTH];
    ret[0] = '\0';
    for (len = i = 0; len < length; ++i)
    {
        if ((src[i] != '&') || (src[i] != '^'))
            ++len;
        if ((src[i] == '&') || (src[i] == '^'))
        {
            if ((src[i] == '&') && (src[i + 1] == '&'))
                ++len;
            else if ((src[i] == '^') && (src[i + 1] == '^'))
                ++len;
            else
                len -= 2;
        }
        ret[i] = src[i];
    }
    return ret;
}
int strlen_color(char* argument)
{
    char* str;
    int i, length;

    str = argument;
    if (argument == nullptr)
        return 0;

    for (length = i = 0; i < strlen(argument); ++i)
    {
        if ((str[i] != '&') && (str[i] != '^'))
            ++length;
        if ((str[i] == '&') || (str[i] == '^'))
        {
            if ((str[i] == '&') && (str[i + 1] == '&'))
                length = 2 + length;
            else if ((str[i] == '^') && (str[i + 1] == '^'))
                length = 2 + length;
            else
                --length;
        }
    }

    return length;
}

char* format_str(char* src, int len)
{
    int sp1, sx;
    static char add_len[MAX_STRING_LENGTH];
    add_len[0] = '\0';
    sp1 = strlen_color(src);
    if (sp1 < len)
    {
        for (sx = 14; sx >= sp1; sx--)
            strcat_s(add_len, " ");
        strcat(src, add_len);
        return src;
    }
    else
        return chrmax(src, len);
}
