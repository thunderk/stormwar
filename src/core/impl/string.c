/******************************************************************************
 *                   StormWar, a Real Time Strategy game                      *
 *                   Copyright (C) 2005  LEMAIRE Michael                      *
 *----------------------------------------------------------------------------*
 *  This program is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                            *
 *  Read the full terms of this license in the "COPYING" file.                *
  ****************************************************************************
 *                                                                            *
 *   String type                                                              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/string.h"

#include "tools/fonct.h"

#include <stdio.h>

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_String
{
    char* str;              /* \0-terminated string. */
    unsigned int len;       /* String len. */
    unsigned int alloclen;  /* Really allocated len. */
};

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
String
String_new(const char* st)
{
    unsigned int i;
    char* stp;
    String ret;
    
    if (st == NULL)
    {
        st =  "";
    }
    
    /*get size*/
    stp = (char*)st;
    i = 0;
    while (*(stp++) != '\0')
    {
        i++;
    }
    
    /*alloc*/
    ret = MALLOC(sizeof(pv_String));
    ret->len = i;
    ret->alloclen = i + 6;
    ret->str = MALLOC(sizeof(char) * ret->alloclen);
    
    /*fill*/
    memCOPY(ret->str, st, sizeof(char) * (i + 1));
    
    return ret;
}

/*----------------------------------------------------------------------------*/
String
String_newBySizedCopy(const char* st, unsigned int nbc)
{
    String ret;
    
    /*alloc*/
    ret = MALLOC(sizeof(pv_String));
    ret->len = nbc;
    ret->alloclen = nbc + 6;
    ret->str = MALLOC(sizeof(char) * ret->alloclen);
    
    /*copy*/
    memCOPY(ret->str, st, sizeof(char) * nbc);
    ret->str[nbc] = '\0';
    
    return ret;
}

/*----------------------------------------------------------------------------*/
String
String_newByCopy(String string)
{
    String ret;
    
    /*alloc*/
    ret = MALLOC(sizeof(pv_String));
    ret->len = string->len;
    ret->alloclen = string->alloclen;
    ret->str = MALLOC(sizeof(char) * ret->alloclen);
    
    /*copy*/
    memCOPY(ret->str, string->str, sizeof(char) * (ret->len + 1));
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
String_del(String s)
{
    FREE(s->str);
    FREE(s);
}

/*----------------------------------------------------------------------------*/
char*
String_get(String string)
{
    return string->str;
}

/*----------------------------------------------------------------------------*/
Bool
String_isEmpty(String string)
{
    return (string->len == 0);
}

/*----------------------------------------------------------------------------*/
unsigned int
String_getLength(String string)
{
    return string->len;
}

/*----------------------------------------------------------------------------*/
void
String_replace(String string, const char* cstring)
{
    String fake;
    
    FREE(string->str);
    fake = String_new(cstring);
    *string = *fake;
    FREE(fake);
}

/*----------------------------------------------------------------------------*/
void
String_clear(String string)
{
    string->str = REALLOC(string->str, 6);
    string->len = 0;
    string->alloclen = 6;
    string->str[0] = '\0';
}

/*----------------------------------------------------------------------------*/
void
String_copy(String dest, String src)
{
    /*free the dest*/
    FREE(dest->str);
    
    /*alloc*/
    dest->len = src->len;
    dest->alloclen = src->alloclen;
    dest->str = MALLOC(sizeof(char) * src->alloclen);
    
    /*copy*/
    memCOPY(dest->str, src->str, sizeof(char) * (src->len + 1));
}

/*----------------------------------------------------------------------------*/
void
String_erase(String string, unsigned int pos, unsigned int size)
{
    char* src;
    char* dest;

    /*checks*/
    pos = MIN(pos, string->len - 1);
    size = MIN(size, string->len - pos);
    
    if (size == 0)
    {
        return;
    }
    
    /*move down*/
    src = string->str + pos + size;
    dest = string->str + pos;
    string->len -= size;
    
    while (size-- > 0)
    {
        *(dest++) = *(src++);
    }
    
    if (string->alloclen - string->len > 5)
    {
        /*need shrinking*/
        string->alloclen = string->len + 6;
        string->str = REALLOC(string->str, string->alloclen);
    }
}

/*----------------------------------------------------------------------------*/
void
String_appendString(String string1, String string2)
{
    if (string1->len + string2->len >= string1->alloclen)
    {
        /*need growing*/
        string1->alloclen += string2->len;
        string1->str = REALLOC(string1->str, string1->alloclen);
    }
    
    /*append*/
    memCOPY(string1->str + string1->len, string2->str, sizeof(char) * (string2->len + 1));
    string1->len += string2->len;
}

/*----------------------------------------------------------------------------*/
void
String_append(String string, const char* st)
{
    unsigned int size;
    char* stp;

    /*get size*/
    stp = (char*)st;
    size = 0;
    while (*(stp++) != '\0')
    {
        size++;
    }
    
    if (string->len + size + 1 >= string->alloclen)
    {
        /*need growing*/
        string->alloclen += size + 3;
        string->str = REALLOC(string->str, string->alloclen);
    }
    
    /*append*/
    memCOPY(string->str + string->len, st, sizeof(char) * (size + 1));
    string->len += size;
}

/*----------------------------------------------------------------------------*/
void
String_appendChar(String string, char ch)
{
    if (string->len + 1 == string->alloclen)
    {
        /*need growing*/
        string->alloclen += 5;
        string->str = REALLOC(string->str, string->alloclen);
    }
    
    string->str[string->len++] = ch;
    string->str[string->len] = '\0';
}

/*----------------------------------------------------------------------------*/
Bool
String_equal(String s1, String s2)
{
    char* c1;
    char* c2;
    
    c1 = s1->str;
    c2 = s2->str;
    while ((*c1 != '\0') & (*c1 == *c2))
    {
        c1++;
        c2++;
    }
    return *c1 == *c2;
}

/*----------------------------------------------------------------------------*/
int
String_cmp(String* s1, String* s2)
{
    char* c1;
    char* c2;
    
    c1 = (*s1)->str;
    c2 = (*s2)->str;
    while ((*c1 != '\0') & (*c1 == *c2))
    {
        c1++;
        c2++;
    }
    return *c1 - *c2;
}

/*----------------------------------------------------------------------------*/
void
String_printf(String s, const char* format, ...)
{
    va_list(ap);
    va_start(ap, format);
    
    String_vprintf(s, format, ap);
    
    va_end(ap);
}

/*----------------------------------------------------------------------------*/
void
String_vprintf(String s, const char* format, va_list list)
{
#ifdef HAVE_VASPRINTF
    FREE(s->str);

    s->len = vasprintf(&s->str, format, list);
    s->alloclen = s->len + 1;
    (void)debugALLOC(s->str, sizeof(char) * s->alloclen);
#else
    static char buf[10000];
    int size;
    
    size = vsnprintf(buf, 10000, format, list);
    if (size < 0)
    {
        String_clear(s);
    }
    else
    {
        String fake;
        
        size = MIN(size, 9999);     /*since GlibC 2.1, vsnprintf doesn't return -1*/
        
        fake = String_newBySizedCopy(buf, size);
        FREE(s->str);
        *s = *fake;
        FREE(fake);
    }
#endif
}
