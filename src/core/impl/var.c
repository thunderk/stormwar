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
 *   Object used to contain any type of variable                              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/var.h"

#include "core/impl/impl.h"

#include "core/string.h"
#include "core/ptrarray.h"

#include "core/reader.h"

#include "core/core.h"

#include <ctype.h>
#include <stdio.h>

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_Var
{
    String name;            /*!< Variable's name */
    VarType type;           /*!< Variable's type */
    String image;           /*!< String representation */
    union
    {
        Int      vint;          /*!< Integer value */
        Float    vfloat;        /*!< Float value   */
        String   vstring;       /*!< String value  */
        PtrArray    varray;     /*!< Array value   */
        String      link;       /*!< Path and name of a file containing the value */
    } value;                /*!< Variable's value */
};

/******************************************************************************
 *                                   Macros                                   *
 ******************************************************************************/
#define Var_CLEARIMAGE(_var_) if ((_var_)->image != NULL) {String_del((_var_)->image); (_var_)->image = NULL;}

/******************************************************************************
 *############################################################################*
 *#                           Internal functions                             #*
 *############################################################################*
 ******************************************************************************/
void
Var_setFromReader(Var var, Reader reader)
{
    ReaderToken* cur;
    
    cur = Reader_getCurrent(reader);
    if ((cur->type == READER_CHAR) && (cur->value.c == '#'))
    {
        /*this looks like a name, read it*/
        cur = Reader_forward(reader);
        if (cur->type != READER_NAME)
        {
            /*TODO: better error message*/
            shellPrint(LEVEL_ERROR, "Didn't find a variable name after '#' given.");
            Var_setName(var, NULL);
            Var_setVoid(var);
            return;
        }
        
        /*set the name*/
        Var_setName(var, String_get(cur->value.s));
        
        /*check if we have a '=' sign*/
        cur = Reader_forward(reader);
        if ((cur->type != READER_CHAR) || (cur->value.c != '='))
        {
            /*TODO: collect the value from shell variables*/
            /*TODO: better error message*/
            shellPrintf(LEVEL_ERROR, "Don't have a '=' symbol for '%s' variable.", String_get(Var_getName(var)));
            Var_setVoid(var);
            return;
        }
        
        cur = Reader_forward(reader);       /*to skip the '='*/
    }
    else
    {
        /*Variable isn't named*/
        Var_setName(var, NULL);
    }
    
    
    /*here should be a value*/
    if (cur->type == READER_STRING)
    {
        /*seems to have a string*/
        if (String_get(cur->value.s)[0] == '&')
        {
            String s;
            
            s = String_newBySizedCopy(String_get(cur->value.s) + 1, String_getLength(cur->value.s) - 1);
            Var_setString(var, _s(s));
            String_del(s);
        }
        else
        {
            Var_setString(var, cur->value.s);
        }
        cur = Reader_forward(reader);
    }
    else if (cur->type == READER_INT)
    {
        Var_setInt(var, cur->value.i);
        cur = Reader_forward(reader);
    }
    else if (cur->type == READER_FLOAT)
    {
        Var_setFloat(var, cur->value.f);
        cur = Reader_forward(reader);
    }
    else if ((cur->type == READER_CHAR) && (cur->value.c == '['))
    {
        /*seems to have an array*/
        Var_setArray(var);
        cur = Reader_forward(reader);     /*to skip the '['*/
        
        while (!((cur->type == READER_CHAR) && (cur->value.c == ']')))
        {
            Var ve;

            /*TODO: maybe check for READER_END*/
            
            ve = Var_new();
            Var_setFromReader(ve, reader);
            Var_insertIntoArray(var, ve);
            
            cur = Reader_getCurrent(reader);
            if ((cur->type == READER_CHAR) && (cur->value.c == ','))
            {
                cur = Reader_forward(reader);     /*to skip the ','*/
            }
            else if (cur->type == READER_END)
            {
                /*TODO: better error message*/
                shellPrintf(LEVEL_ERROR, "End of stream encountered while expecting ']' for variable '%s'.", String_get(Var_getName(var)));
                Var_setVoid(var);
                return;
            }
            else if (!((cur->type == READER_CHAR) && (cur->value.c == ']')))
            {
                /*TODO: better error message*/
                shellPrintf(LEVEL_ERROR, "Unexpected token encountered while expecting ']' for variable '%s'.", String_get(Var_getName(var)));
                Var_setVoid(var);
                return;
            }
        }
        cur = Reader_forward(reader); /*to skip the ']'*/
    }
    else if ((cur->type == READER_CHAR) && (cur->value.c == '@'))
    {
        /*seems to have a link*/
        cur = Reader_forward(reader);
        if (cur->type != READER_STRING)
        {
            /*TODO: better error message*/
            shellPrintf(LEVEL_ERROR, "Wrong link format for variable '%s'.", String_get(Var_getName(var)));
            Var_setVoid(var);
            return;
        }
        
        var->type = VAR_LINK;
        var->value.link = String_newByCopy(cur->value.s);
        cur = Reader_forward(reader);
    }
    else if (cur->type == READER_NAME)
    {
        /*seems to have a shell call*/
        shellExecFromReader(var, reader);
    }
    else
    {
        /*we're running short of options*/
        /*TODO: better error message*/
        shellPrintf(LEVEL_ERROR, "Wrong value format for variable '%s'.", String_get(Var_getName(var)));
        Var_setVoid(var);
        return;
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Var
Var_new()
{
    Var ret;
    
    ret = (Var)MALLOC(sizeof(pv_Var));
    ret->name = String_new("");
    ret->type = VAR_VOID;
    ret->image = NULL;
    return ret;
}

/*----------------------------------------------------------------------------*/
Var
Var_newByCopy(Var v)
{
    Var ret;
    
    ret = (Var)MALLOC(sizeof(pv_Var));
    ret->type = VAR_VOID;
    ret->name = String_newByCopy(v->name);
    ret->image = NULL;
    Var_setFromVar(ret, v);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Var_del(Var var)
{
    Var_CLEARIMAGE(var);
    Var_setVoid(var);
    String_del(var->name);
    FREE(var);
}

/*----------------------------------------------------------------------------*/
int
Var_nameCmp(Var* v1, Var* v2)
{
    ASSERT(v1 != NULL, return 0);
    ASSERT(v2 != NULL, return 0);
    
    return String_cmp(&((*v1)->name), &((*v2)->name));
}

/*----------------------------------------------------------------------------*/
void
Var_setName(Var var, const char* name)
{
    String_replace(var->name, (name == NULL) ? "" : name);
    Var_CLEARIMAGE(var);
}

/*----------------------------------------------------------------------------*/
void
Var_setType(Var var, VarType type)
{
    Var_setVoid(var);
    switch (type)
    {
        case VAR_VOID:
            break;
        case VAR_INT:
            var->value.vint = 0;
            break;
        case VAR_FLOAT:
            var->value.vfloat = 0.0;
            break;
        case VAR_STRING:
            var->value.vstring = String_new("");
            break;
        case VAR_ARRAY:
            Var_setArray(var);
            break;
        case VAR_LINK:
            var->value.link = String_new("");
            break;
    }
    var->type = type;
    /*Var_CLEARIMAGE(var);*/
}

/*----------------------------------------------------------------------------*/
void
Var_setVoid(Var var)
{
    if (var->type == VAR_STRING)
    {
        String_del(var->value.vstring);
    }
    else if (var->type == VAR_ARRAY)
    {
        PtrArray_del(var->value.varray);
    }
    else if (var->type == VAR_LINK)
    {
        String_del(var->value.link);
    }
    var->type = VAR_VOID;
    Var_CLEARIMAGE(var);
}

/*----------------------------------------------------------------------------*/
void
Var_setInt(Var var, Int val)
{
    Var_setVoid(var);
    var->type = VAR_INT;
    var->value.vint = val;
    /*Var_CLEARIMAGE(var);*/
}

/*----------------------------------------------------------------------------*/
void
Var_setFloat(Var var, Float val)
{
    Var_setVoid(var);
    var->type = VAR_FLOAT;
    var->value.vfloat = val;
    /*Var_CLEARIMAGE(var);*/
}

/*----------------------------------------------------------------------------*/
void
Var_setString(Var var, String val)
{
    Var_setVoid(var);
    var->type = VAR_STRING;
    var->value.vstring = String_newByCopy(val);
    /*Var_CLEARIMAGE(var);*/
}

/*----------------------------------------------------------------------------*/
void
Var_setArray(Var var)
{
    Var_setVoid(var);
    var->type = VAR_ARRAY;
    var->value.varray = PtrArray_newFull(1, 4, (PtrFunc)Var_del, (PtrCmpFunc)Var_nameCmp);
    /*Var_CLEARIMAGE(var);*/
}

/*----------------------------------------------------------------------------*/
void
Var_addToArray(Var var, Var elem)
{
    Var_insertIntoArray(var, Var_newByCopy(elem));
}

/*----------------------------------------------------------------------------*/
void
Var_insertIntoArray(Var var, Var elem)
{
    if (var->type != VAR_ARRAY)
    {
        Var_setArray(var);
    }
    
    if (String_isEmpty(elem->name))
    {
        PtrArray_append(var->value.varray, (Ptr)elem);
    }
    else
    {
        PtrArray_insertSorted(var->value.varray, (Ptr)elem);
    }
    Var_CLEARIMAGE(var);
}

/*----------------------------------------------------------------------------*/
void
Var_setFromVar(Var dest, Var src)
{
    PtrArrayIterator i;
    Var v;
    
    switch (src->type)
    {
        case VAR_VOID:
            Var_setVoid(dest);
            break;
        case VAR_INT:
            Var_setInt(dest, src->value.vint);
            break;
        case VAR_FLOAT:
            Var_setFloat(dest, src->value.vfloat);
            break;
        case VAR_STRING:
            Var_setString(dest, src->value.vstring);
            break;
        case VAR_ARRAY:
            Var_setArray(dest);
            /*array is empty, we fill it by copy*/
            for (i = PtrArray_START(src->value.varray); i != PtrArray_STOP(src->value.varray); i++)
            {
                v = Var_new();
                Var_setName(v, String_get(((Var)*i)->name));
                Var_setFromVar(v, (Var)*i);
                PtrArray_append(dest->value.varray, v);
            }
            break;
        case VAR_LINK:
            Var_setVoid(dest);
            dest->type = VAR_LINK;
            dest->value.link = String_newByCopy(src->value.link);
            break;
    }
    /*Var_CLEARIMAGE(dest);*/
}

/*----------------------------------------------------------------------------*/
Bool
Var_setFromString(Var var, String s)
{
    Reader reader;
    
    reader = Reader_newFromString(String_get(s));
    if (reader == NULL)
    {
        return TRUE;
    }
    
    Var_setFromReader(var, reader);
    Reader_del(reader);
    return FALSE;
}

/*----------------------------------------------------------------------------*/
Bool
Var_readFromFile(Var var, String file)
{
    Reader reader;
    String name;
    
    if (String_cmp(&file, &STRING_NULL) == 0)
    {
        shellPrintf(LEVEL_ERROR, _("Try to read a file with void path."));
        return TRUE;
    }
    
    name = String_newByCopy(Var_getName(var));
    
    coreFindData(file);
    reader = Reader_newFromFile(String_get(file));
    /*if (reader == NULL)
    {
        String_del(name);
        return TRUE;
    }*/
    
    Var_setFromReader(var, reader);
    
    Reader_del(reader);
    
    Var_setName(var, String_get(name));
    String_del(name);
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
Bool
Var_saveToFile(Var var, String file)
{
    FILE* outfile;
    
    ASSERT(Var_getType((Var)var) == VAR_ARRAY, return TRUE);

    /*we open the file for output*/
    outfile = fopen(String_get(file), "w");
    if (outfile == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Unable to save variable to file '%s' :", String_get(file));
        shellPrintf(LEVEL_ERROR, "   %s", Var_gets(var));
        return TRUE;
    }

    /*we save the var*/
    fprintf(outfile, "%s\n", Var_gets(var));

    fclose(outfile);
    return FALSE;
}

/*----------------------------------------------------------------------------*/
VarType
Var_getType(Var var)
{
    return var->type;
}

/*----------------------------------------------------------------------------*/
String
Var_getName(Var var)
{
    return var->name;
}

/*----------------------------------------------------------------------------*/
const char*
Var_gets(Var var)
{
    String s;
    unsigned int i;
    
    if (var->image == NULL)
    {
        var->image = String_new("");
        if (! String_isEmpty(var->name))
        {
            String_appendChar(var->image, '#');
            String_appendString(var->image, var->name);
            String_appendChar(var->image, '=');
        }
        
        switch (var->type)
        {
            case VAR_VOID:
                break;
            case VAR_INT:
                s = String_new("");
                String_printf(s, "%d", var->value.vint);
                String_appendString(var->image, s);
                String_del(s);
                break;
            case VAR_FLOAT:
                s = String_new("");
                String_printf(s, "%f", var->value.vfloat);
                String_appendString(var->image, s);
                String_del(s);
                break;
            case VAR_STRING:
                String_appendChar(var->image, '\"');
                String_appendString(var->image, var->value.vstring);
                String_appendChar(var->image, '\"');
                break;
            case VAR_ARRAY:
                String_appendChar(var->image, '[');
                for (i = 0; i < Var_getArraySize(var); i++)
                {
                    String_append(var->image, Var_gets(Var_getArrayElemByPos(var, i)));
                    if ((int)i < PtrArray_SIZE(var->value.varray) - 1)
                    {
                        String_appendChar(var->image, ',');
                    }
                }
                String_appendChar(var->image, ']');
                break;
            case VAR_LINK:
                String_appendChar(var->image, '@');
                String_appendChar(var->image, '\"');
                String_appendString(var->image, var->value.link);
                String_appendChar(var->image, '\"');
                break;
        }
    }
    
    return String_get(var->image);
}

/*----------------------------------------------------------------------------*/
Int
Var_getValueInt(Var var)
{
    ASSERT(var != NULL, return 0);
    ASSERT(var->type == VAR_INT, return 0);
    
    return var->value.vint;
}

/*----------------------------------------------------------------------------*/
Float
Var_getValueFloat(Var var)
{
    ASSERT(var != NULL, return 0);
    ASSERT(var->type == VAR_FLOAT, return 0);
    
    return var->value.vfloat;
}

/*----------------------------------------------------------------------------*/
String
Var_getValueString(Var var)
{
    ASSERT(var != NULL, return 0);
    ASSERT(var->type == VAR_STRING, return 0);
    
    return var->value.vstring;
}

/*----------------------------------------------------------------------------*/
void
Var_resolveLink(Var var)
{
    String s;
    if (var->type == VAR_LINK)
    {
        /*resolve link*/
        /*we copy the path because resolving will destroy the current variable's value that contains it*/
        s = String_newByCopy(var->value.link);
        Var_readFromFile(var, s);
        String_del(s);
        Var_CLEARIMAGE(var);
    }
}

/*----------------------------------------------------------------------------*/
void
Var_removeUnnamedFields(Var var)
{
    int i;
    
    ASSERT(var->type == VAR_ARRAY, return);
    
    i = 0;
    while (i < PtrArray_SIZE(var->value.varray))
    {
        if (String_isEmpty(PtrArray_TYPEDELEM(var->value.varray, i, Var)->name))
        {
            PtrArray_removePos(var->value.varray, i);
        }
        else
        {
            i++;
        }
    }
    Var_CLEARIMAGE(var);
}

/*----------------------------------------------------------------------------*/
Var
Var_getArrayElemByName(Var var, String name)
{
    PtrArrayIterator i;
    Var v;
    
    ASSERT(var->type == VAR_ARRAY, return NULL);
    
    v = Var_new();
    Var_setName(v, String_get(name));
    i = PtrArray_findSorted(var->value.varray, v);
    Var_del(v);
    
    if (i == NULL)
    {
        return NULL;
    }
    else
    {
        return (Var)*i;
    }
}

/*----------------------------------------------------------------------------*/
Var
Var_getArrayElemByCName(Var var, char* name)
{
    String s;
    Var ret;
    
    s = String_new(name);
    ret = Var_getArrayElemByName(var, s);
    String_del(s);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
VarArrayPos
Var_getArraySize(Var var)
{
    ASSERT(var->type == VAR_ARRAY, return 0);
    
    return PtrArray_SIZE(var->value.varray);
}

/*----------------------------------------------------------------------------*/
Var
Var_getArrayElemByPos(Var var, VarArrayPos pos)
{
    ASSERT(var->type == VAR_ARRAY, return NULL);
    ASSERT(pos < PtrArray_SIZE(var->value.varray), return NULL);
    
    return PtrArray_TYPEDELEM(var->value.varray, pos, Var);
}
