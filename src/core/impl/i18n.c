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
 *   Internationalization                                                     *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/i18n.h"
#include "core/comp.h"
#include "core/core.h"
#include "core/string.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    String orig;        /*original string*/
    String trans;       /*translated string*/
} pv_I18n_elem;

typedef pv_I18n_elem* I18n_elem;

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Bool inited = FALSE;
static PtrArray elems;

static const char* _language = "en";

#ifdef DEBUG_I18N
static PtrArray debug_missing;
static PtrArray debug_notused;
#endif

static CoreID MOD_ID = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static I18n_elem
I18n_elem_new(String orig, String trans)
{
    I18n_elem ret;

    ret = MALLOC(sizeof(pv_I18n_elem));
    ret->orig = String_newByCopy(orig);
    ret->trans = String_newByCopy(trans);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
static void
I18n_elem_del(I18n_elem elem)
{
    String_del(elem->orig);
    String_del(elem->trans);
    FREE(elem);
}

/*----------------------------------------------------------------------------*/
static int
I18n_elem_cmp(I18n_elem* e1, I18n_elem* e2)
{
    return String_cmp(&((*e1)->orig), &((*e2)->orig));
}

/*----------------------------------------------------------------------------*/
static void
i18nAdd(Var v)
{
    VarValidator valid;
    I18n_elem elem;
    
    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "o", "");
    VarValidator_declareStringVar(valid, "t", "");
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    elem = I18n_elem_new(Var_getValueString(Var_getArrayElemByCName(v, "o")), Var_getValueString(Var_getArrayElemByCName(v, "t")));
    if (PtrArray_findSorted(elems, elem) != NULL)
    {
        shellPrintf(LEVEL_ERROR, "Translation already defined: %s", String_get(elem->orig));
        I18n_elem_del(elem);
    }
    else
    {
#ifdef DEBUG_I18N
        PtrArray_insertSorted(debug_notused, elem->orig);
#endif
        PtrArray_insertSorted(elems, elem);
    }
}

/*----------------------------------------------------------------------------*/
#ifdef DEBUG_I18N
static void
processDebug()
{
    PtrArrayIterator it;
    
    if (PtrArray_SIZE(elems) == 0)
    {
        shellPrint(LEVEL_DEBUG, "Internationalization had no element set.");
    }
    else
    {
        for (it = PtrArray_START(debug_missing); it != PtrArray_STOP(debug_missing); it++)
        {
            shellPrintf(LEVEL_DEBUG, "Missing translation: %s", String_get(*(String*)it));
        }
        for (it = PtrArray_START(debug_notused); it != PtrArray_STOP(debug_notused); it++)
        {
            shellPrintf(LEVEL_DEBUG, "Unused translation: %s", String_get(*(String*)it));
        }
    }
}
#endif

/*----------------------------------------------------------------------------*/
static void
datasCallback(Var v)
{
    PtrArrayPos i;
    String s;
    Var vl;
    
    if (inited)
    {
#ifdef DEBUG_I18N
        processDebug();
        PtrArray_clear(debug_missing);
        PtrArray_clear(debug_notused);
#endif
        
        PtrArray_clear(elems);

        if (v == NULL)
        {
            return;
        }
    
        Var_resolveLink(v);
        if (Var_getType(v) != VAR_ARRAY)
        {
            shellPrintf(LEVEL_ERROR, "Internationalization reference isn't an array: %s", Var_gets(v));
            Var_setArray(v);
        }
        
        s = String_newBySizedCopy(_language, 2);
        vl = Var_getArrayElemByName(v, s);
        if (vl == NULL)
        {
            if (_language[0] != 'e' || _language[1] != 'n')
            {
                shellPrint(LEVEL_ERROR, "Local language translations not found, using default.");
            }
            String_del(s);
            return;
        }
        
        Var_resolveLink(vl);
        if (Var_getType(vl) != VAR_ARRAY)
        {
            shellPrintf(LEVEL_ERROR, "Internationalization variable isn't an array: %s", Var_gets(vl));
            Var_setArray(vl);
        }
        
        for (i = 0; i < Var_getArraySize(vl); i++)
        {
            i18nAdd(Var_getArrayElemByPos(vl, i));
        }
        String_del(s);
    }
}

/******************************************************************************
 *############################################################################*
 *#                              Public functions                            #*
 *############################################################################*
 ******************************************************************************/
void
i18nInit()
{
    if (!inited)
    {
        elems = PtrArray_newFull(50, 10, (PtrFunc)I18n_elem_del, (PtrCmpFunc)I18n_elem_cmp);

#ifdef DEBUG_I18N
    debug_missing = PtrArray_newFull(50, 10, (PtrFunc)String_del, (PtrCmpFunc)String_cmp);
    debug_notused = PtrArray_newFull(50, 10, NULL, (PtrCmpFunc)String_cmp);
#endif
        
        MOD_ID = coreDeclareModule("i18n", NULL, datasCallback, NULL, NULL, NULL, NULL);
        
        _language = compGuessLocalLanguage();
        shellPrintf(LEVEL_INFO, " -> Current language code is: '%c%c'", _language[0], _language[1]);
        
        inited = TRUE;
    }
}

/*----------------------------------------------------------------------------*/
void
i18nUninit()
{
    if (inited)
    {
        inited = FALSE;
        
#ifdef DEBUG_I18N
        processDebug();
        PtrArray_del(debug_missing);
        PtrArray_del(debug_notused);
#endif
        
        PtrArray_del(elems);

        shellPrint(LEVEL_INFO, "Internationalization module unloaded.");
    }
}

/*----------------------------------------------------------------------------*/
char*
pv_i18nTranslate(char* st)
{
    PtrArrayIterator it;
    pv_I18n_elem elem;
    
    if (inited)
    {
        elem.orig = String_new(st);
        it = PtrArray_findSorted(elems, &elem);
#ifdef DEBUG_I18N
        PtrArray_removeIt(debug_notused, PtrArray_findSorted(debug_notused, elem.orig));
#endif
        String_del(elem.orig);
        
        if (it == NULL)
        {
#ifdef DEBUG_I18N
            String s;
            s = String_new(st);
            if (PtrArray_findSorted(debug_missing, s) == NULL)
            {
                PtrArray_insertSorted(debug_missing, s);
            }
            else
            {
                String_del(s);
            }
#endif
            return st;
        }
        else
        {
            return String_get(((I18n_elem)(*it))->trans);
        }
    }
    else
    {
        return st;
    }
}

/*----------------------------------------------------------------------------*/
String
pv_i18nTranslateString(String st)
{
    PtrArrayIterator s;
    pv_I18n_elem elem;
    
    if (inited)
    {
#ifdef DEBUG_I18N
        PtrArray_removeIt(debug_notused, PtrArray_findSorted(debug_notused, st));
#endif
        elem.orig = st;
        s = PtrArray_findSorted(elems, &elem);
        if (s == NULL)
        {
#ifdef DEBUG_I18N
            if (PtrArray_findSorted(debug_missing, st) == NULL)
            {
                PtrArray_insertSorted(debug_missing, String_newByCopy(st));
            }
#endif
            return st;
        }
        else
        {
            return ((I18n_elem)(*s))->trans;
        }
    }
    else
    {
        return st;
    }
}
