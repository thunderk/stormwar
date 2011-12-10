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
 *   Shell function object                                                    *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/impl/shellfunction.h"

#include "core/string.h"

/******************************************************************************
 *                               Static variables                             *
 ******************************************************************************/
static char* VARTYPE_STRING[5] = {"void", "int", "float", "string", "array"};

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
ShellFunction_new(ShellFunction* func, String name, CoreID id, int nbparams, VarType rettype)
{
    int i;
    
    func->name = String_newByCopy(name);
    func->id = id;
    func->nbparam = nbparams;
    if (nbparams != 0)
    {
        func->params = (Var*)MALLOC(sizeof(Var) * nbparams);
    }
    else
    {
        func->params = NULL;
    }
    for (i = 0; i < nbparams; i++)
    {
        func->params[i] = NULL;
    }
    func->ret = Var_new();
    Var_setType(func->ret, rettype);
}

/*----------------------------------------------------------------------------*/
void
ShellFunction_copy(ShellFunction* src, ShellFunction* dest)
{
    int i;
    
    dest->name = String_newByCopy(src->name);
    dest->id = src->id;
    dest->nbparam = src->nbparam;
    if (dest->nbparam != 0)
    {
        dest->params = (Var*)MALLOC(sizeof(Var) * dest->nbparam);
    }
    else
    {
        dest->params = NULL;
    }
    for (i = 0; i < dest->nbparam; i++)
    {
        dest->params[i] = Var_new();
        Var_setType(dest->params[i], Var_getType(src->params[i]));
    }
    dest->ret = Var_new();
    Var_setType(dest->ret, Var_getType(src->ret));
}

/*----------------------------------------------------------------------------*/
void
ShellFunction_del(ShellFunction* func)
{
    int i;
    
    String_del(func->name);
    if (func->params != NULL)
    {
        for (i = 0; i < func->nbparam; i++)
        {
            if (func->params[i] != NULL)
            {
                Var_del(func->params[i]);
            }
        }
        FREE(func->params);
    }
    Var_del(func->ret);
}

/*----------------------------------------------------------------------------*/
int
ShellFunction_cmp(ShellFunction** func1, ShellFunction** func2)
{
    return String_cmp(&((*func1)->name), &((*func2)->name));
}

/*----------------------------------------------------------------------------*/
void
ShellFunction_setParamType(ShellFunction* func, int pos, VarType type)
{
    ASSERT(pos >= 0, return);
    ASSERT(pos < func->nbparam, return);

    if (func->params[pos] != NULL)
    {
        Var_del(func->params[pos]);
    }
    func->params[pos] = Var_new();
    Var_setType(func->params[pos], type);
}

/*----------------------------------------------------------------------------*/
void ShellFunction_getPrototype(ShellFunction* func, String prototype)
{
    int i;
    
    String_replace(prototype, VARTYPE_STRING[Var_getType(func->ret)]);
    String_appendChar(prototype, ' ');
    String_appendString(prototype, func->name);
    
    String_appendChar(prototype, '(');
    
    for (i = 0; i < func->nbparam; i++)
    {
        String_append(prototype, VARTYPE_STRING[Var_getType(func->params[i])]);
        if (i + 1 < func->nbparam)
        {
            String_appendChar(prototype, ',');
        }
    }
    String_appendChar(prototype, ')');
}
