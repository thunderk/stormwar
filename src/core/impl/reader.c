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
 *   Buffered reader                                                          *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/reader.h"

#include "core/string.h"

#include <stdio.h>
#include <ctype.h>

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_Reader
{
    char* source;
    char* readpos;
    ReaderToken current;
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static String
readSource(Bool file, const char* s)
{
    String ret;
    FILE* f = NULL;
    char c;
    Bool protected = FALSE;
    Bool escape = FALSE;

    /*This function is used to store the source in a string after having pre-parsed it.*/
    
    ret = String_new("");
    if (file)
    {
        f = fopen(s, "rb");
        if (f == NULL)
        {
            return ret;
        }
    }
    
    do
    {
        /*read one character*/
        if (file)
        {
            int i;
            i = fgetc(f);
            if (i == EOF)
            {
                c = '\0';
            }
            else
            {
                c = (int)i;
            }
        }
        else
        {
            c = *(s++);
        }
        
        /*cases*/
        if (protected)
        {
            if (escape)
            {
                if (c == '\0')
                {
                    shellPrint(LEVEL_ERROR, "Escaping character at end of stream in :");
                    shellPrintf(LEVEL_ERROR, "%s", s);
                    String_appendChar(ret, '\"');
                }
                else
                {
                    String_appendChar(ret, c);
                }
                escape = FALSE;
            }
            else
            {
                if (c == '\\')
                {
                    escape = TRUE;
                    String_appendChar(ret, c);
                }
                else if (c == '\"')
                {
                    protected = FALSE;
                    String_appendChar(ret, c);
                }
                else if (c == '\0')
                {
                    shellPrint(LEVEL_ERROR, "End of stream encountered while expecting a '\"' in :");
                    shellPrintf(LEVEL_ERROR, "%s", s);
                    String_appendChar(ret, '\"');
                }
                else
                {
                    String_appendChar(ret, c);
                }
            }
        }
        else
        {
            if (c == '\"')
            {
                String_appendChar(ret, c);
                protected = TRUE;
            }
            else if ((!isspace((int)c)) && (c != '\0'))
            {
                String_appendChar(ret, c);
            }
        }
    } while (c != '\0');
    
    if (file)
    {
        fclose(f);
    }
    
    return ret;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Reader
Reader_newFromFile(const char* path)
{
    Reader ret;
    String source;
    
    ret = MALLOC(sizeof(pv_Reader));
    source = readSource(TRUE, path);
    ret->source = MALLOC(sizeof(char) * (String_getLength(source) + 1));
    memCOPY(ret->source, String_get(source), sizeof(char) * (String_getLength(source) + 1));
    String_del(source);
    
    ret->readpos = ret->source;
    ret->current.type = READER_CHAR;
    
    Reader_forward(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
Reader
Reader_newFromString(const char* s)
{
    Reader ret;
    String source;
    
    ret = MALLOC(sizeof(pv_Reader));
    source = readSource(FALSE, s);
    ret->source = MALLOC(sizeof(char) * (String_getLength(source) + 1));
    memCOPY(ret->source, String_get(source), sizeof(char) * (String_getLength(source) + 1));
    String_del(source);
    
    ret->readpos = ret->source;
    ret->current.type = READER_CHAR;

    Reader_forward(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Reader_del(Reader reader)
{
    if ((reader->current.type == READER_STRING) | (reader->current.type == READER_NAME))
    {
        String_del(reader->current.value.s);
    }
    FREE(reader->source);
    FREE(reader);
}

/*----------------------------------------------------------------------------*/
ReaderToken*
Reader_getCurrent(Reader reader)
{
    return &reader->current;
}

/*----------------------------------------------------------------------------*/
ReaderToken*
Reader_forward(Reader reader)
{
    char c;
    ReaderToken* ret;
    
    ret = &reader->current;
    
    /*freeing the current token*/
    if ((reader->current.type == READER_STRING) | (reader->current.type == READER_NAME))
    {
        String_del(reader->current.value.s);
    }
    else if (reader->current.type == READER_END)
    {
        return ret;
    }
        
    /*reading the next one*/
    c = *(reader->readpos++);
    if (c == '\0')
    {
        reader->current.type = READER_END;
        return ret;
    }
    else if (c == '\"')
    {
        /*seems to have a double-quoted string*/
        ret->type = READER_STRING;
        ret->value.s = String_new("");
        c = *(reader->readpos++);
        while (c != '\"')   /*should always exit of this, because of the pre-parsing*/
        {
            ASSERT_CRITICAL(c != '\0');
            if (c == '\\')
            {
                /*found an escaping char*/
                c = *(reader->readpos++);
                if (c == 'n')
                {
                    String_appendChar(ret->value.s, '\n');
                }
                else if (c == 't')
                {
                    String_appendChar(ret->value.s, '\t');
                }
                else if (c == 'r')
                {
                    String_appendChar(ret->value.s, '\r');
                }
                else
                {
                    String_appendChar(ret->value.s, c);
                }
            }
            else
            {
                String_appendChar(ret->value.s, c);
            }
            c = *(reader->readpos++);
        }
    }
    else if (isdigit(c) | (c == '-'))
    {
        /*seems to have a number*/
        const char* begin;
        Bool f = FALSE;
        
        begin = reader->readpos - 1;
        c = *reader->readpos;
        while (isdigit(c) | ((c == '.') & (!f)))
        {
            if (c == '.')
            {
                f = TRUE;
            }
            c = *(++reader->readpos);
        }
        if (f)
        {
            ret->type = READER_FLOAT;
            sscanf(begin, "%f", &(ret->value.f));
        }
        else
        {
            ret->type = READER_INT;
            sscanf(begin, "%d", &(ret->value.i));
        }
    }
    else if (isalpha(c))
    {
        /*seems to have a name*/
        ret->type = READER_NAME;
        ret->value.s = String_new("");
        String_appendChar(ret->value.s, c);
        c = *(reader->readpos);
        while (isalnum(c) | (c == '_'))
        {
            String_appendChar(ret->value.s, c);
            c = *(++reader->readpos);
        }
    }
    else
    {
        /*other character*/
        ret->type = READER_CHAR;
        ret->value.c = c;
    }

    return ret;
}
