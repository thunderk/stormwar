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
 *   Validator to check the fields of a named array                           *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "tools/varvalidator.h"

#include "core/string.h"

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
/*----------------------------------------------------------------------------*/

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
VarValidator
VarValidator_new()
{
    Var ret;
    
    ret = Var_new();
    Var_setArray(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
VarValidator_del(VarValidator val)
{
    Var_del(val);
}

/*----------------------------------------------------------------------------*/
void
VarValidator_declareIntVar(VarValidator val, const char* varname, Int varvalue)
{
    Var v;
    
    ASSERT(varname[0] != '\0', return);

    v = Var_new();
    Var_setName(v, varname);
    Var_setInt(v, varvalue);
    Var_addToArray(val, v);
    Var_del(v);
}

/*----------------------------------------------------------------------------*/
void
VarValidator_declareFloatVar(VarValidator val, const char* varname, Float varvalue)
{
    Var v;
    
    ASSERT(varname[0] != '\0', return);

    v = Var_new();
    Var_setName(v, varname);
    Var_setFloat(v, varvalue);
    Var_addToArray(val, v);
    Var_del(v);
}

/*----------------------------------------------------------------------------*/
void
VarValidator_declareStringVar(VarValidator val, const char* varname, char* varvalue)
{
    Var v;
    String s;
    
    ASSERT(varname[0] != '\0', return);
    
    v = Var_new();
    Var_setName(v, varname);
    s = String_new(varvalue);
    Var_setString(v, s);
    Var_addToArray(val, v);
    Var_del(v);
    String_del(s);
}

/*----------------------------------------------------------------------------*/
void
VarValidator_declareArrayVar(VarValidator val, const char* varname)
{
    Var v;
    
    if (varname[0] == '\0')
    {
        shellPrint(LEVEL_ERROR, "Can't declare a validator field without a name.");
        return;
    }
    v = Var_new();
    Var_setArray(v);
    Var_setName(v, varname);
    Var_addToArray(val, v);
    Var_del(v);
}

/*----------------------------------------------------------------------------*/
void
VarValidator_validate(VarValidator val, Var v)
{
    VarArrayPos i;
    Var velem, vs;
    
#ifdef DEBUG_VARSET
    shellPrintf(LEVEL_ERRORSTACK, "In VarValidator for: %s", Var_gets(v));
#else
    shellPrintf(LEVEL_ERRORSTACK, "In VarValidator for: %s", String_get(Var_getName(v)));
#endif
    
    /*check type*/
    Var_resolveLink(v);
    if (Var_getType(v) != VAR_ARRAY)
    {
        shellPrint(LEVEL_ERROR, "Variable not of array type. Converted to array.");
        Var_setArray(v);
    }
    
    /*resolve links*/
    for (i = 0; i < Var_getArraySize(v); i++)
    {
        String s;   /*to protect the name*/
        
        velem = Var_getArrayElemByPos(v, i);
        s = String_newByCopy(Var_getName(velem));
        Var_resolveLink(velem);
        Var_setName(velem, String_get(s));
        
        String_del(s);
    }
    
    /*check unexpected variables*/
    for (i = 0; i < Var_getArraySize(v); i++)
    {
        velem = Var_getArrayElemByPos(v, i);
        if (String_isEmpty(Var_getName(velem)))
        {
            shellPrintf(LEVEL_ERROR, "Unnamed variable found: %s", Var_gets(velem));
            Var_setVoid(velem);
            Var_setName(velem, "");
        }
        else
        {
            vs = Var_getArrayElemByName(val, Var_getName(velem));
            if (vs == NULL)
            {
                shellPrintf(LEVEL_ERROR, "Unexpected variable found: %s", Var_gets(velem));
                Var_setVoid(velem);
                Var_setName(velem, "");
            }
        }
    }
    Var_removeUnnamedFields(v);
    
    /*make declaration match*/
    for (i = 0; i < Var_getArraySize(val); i++)
    {
        velem = Var_getArrayElemByPos(val, i);
        vs = Var_getArrayElemByName(v, Var_getName(velem));
        if (vs == NULL)
        {
            shellPrintf(LEVEL_ERROR, "Expected variable not found: %s", Var_gets(velem));
            Var_addToArray(v, velem);
        }
        else if (Var_getType(vs) != Var_getType(velem))
        {
            shellPrintf(LEVEL_ERROR, "Incorrect type for variable in named array: %s", Var_gets(vs));
            shellPrintf(LEVEL_ERROR, " Replaced by: %s", Var_gets(velem));
            Var_setFromVar(vs, velem);
        }
    }
#ifdef DEBUG_VARSET
    shellPrintf(LEVEL_DEBUG, "Varset validated: %s", Var_gets(v));
#endif

    shellPopErrorStack();
}
