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
 *   Completion tool                                                          *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "tools/completion.h"

#include "core/string.h"

/******************************************************************************
 *                                   Types                                    *
 ******************************************************************************/
struct pv_CompletionResults
{
    PtrArray results;
    PtrArrayPos pos;
};

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
CompletionResults
CompletionResults_new()
{
    CompletionResults ret;
    
    ret = MALLOC(sizeof(pv_CompletionResults));
    ret->results = PtrArray_newFull(3, 2, (PtrFunc)String_del, NULL);
    ret->pos = 0;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
CompletionResults_del(CompletionResults results)
{
    PtrArray_del(results->results);
    FREE(results);
}

/*----------------------------------------------------------------------------*/
String
CompletionResults_getNextResult(CompletionResults results, Bool cycle)
{
    if (PtrArray_SIZE(results->results) == 0)
    {
        return NULL;
    }
    
    if (cycle && (results->pos >= PtrArray_SIZE(results->results)))
    {
        results->pos = 1;
        return PtrArray_TYPEDELEM(results->results, 0, String);
    }
    else if (results->pos >= PtrArray_SIZE(results->results))   /*cycle==FALSE*/
    {
        return NULL;
    }
    else
    {
        return PtrArray_TYPEDELEM(results->results, results->pos++, String);
    }
}

/*----------------------------------------------------------------------------*/
CompletionList
CompletionList_new()
{
    return PtrArray_newFull(10, 5, (PtrFunc)String_del, (PtrCmpFunc)String_cmp);
}

/*----------------------------------------------------------------------------*/
void
CompletionList_del(CompletionList list)
{
    PtrArray_del(list);
}

/*----------------------------------------------------------------------------*/
void
CompletionList_add(CompletionList list, String elem)
{
    PtrArrayIterator s;
    
    s = PtrArray_findSorted(list, elem);
    if (s == NULL)
    {
        PtrArray_insertSorted(list, String_newByCopy(elem));
    }
}

/*----------------------------------------------------------------------------*/
void
CompletionList_query(CompletionList list, const char* prefix, CompletionResults results)
{
    String sprefix;
    String elemprefix;
    PtrArrayIterator it;
    int c;
    
    sprefix = String_new(prefix);
    PtrArray_clear(results->results);
    results->pos = 0;
    
    for (it = PtrArray_START((PtrArray)list); it != PtrArray_STOP((PtrArray)list); it++)
    {
        elemprefix = String_newBySizedCopy(String_get((String)*it), String_getLength(sprefix));
        c = String_cmp(&sprefix, &elemprefix);
        if (c == 0)
        {
            /*found a possible match, adding it to the results*/
            PtrArray_append(results->results, String_newByCopy((String)*it));
        }
        if (c < 0)
        {
            /*as the array is sorted...*/
            return;
        }
    }
}
