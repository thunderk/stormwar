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
 *   Dynamic array of pointers                                                *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/ptrarray.h"
#include "tools/fonct.h"

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static int
defaultCmp(Ptr* p1, Ptr* p2)
{
    /*TODO: maybe the pointer size can be different than int size on some systems*/
    return ((int)(*p1)) - ((int)(*p2));
}

/*----------------------------------------------------------------------------*/
static void
searchSorted(PtrArray array, Ptr ptr, Bool* found, PtrArrayPos* pos)
{
    /*dichotomic search for an element*/
    /*if not found, the position where the element should be inserted will be returned*/
    PtrArrayPos inf, sup, m;
    
    if (array->len == 0)
    {
        *found = FALSE;
        *pos = 0;
    }
    else if (array->cmpfunc(&ptr, PtrArray_STOP(array) - 1) > 0)
    {
        *found = FALSE;
        *pos = array->len;
    }
    else
    {
        inf = 0;
        sup = array->len - 1;
        while (inf < sup)
        {
            m = (inf + sup) / 2;
            if (array->cmpfunc(&ptr, array->array + m) <= 0)
            {
                sup = m;
            }
            else
            {
                inf = m + 1;
            }
        }
        *found = (array->cmpfunc(&ptr, array->array + sup) == 0);
        *pos = sup;
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
PtrArray
PtrArray_new()
{
    PtrArray ret;
    
    ret = (PtrArray)MALLOC(sizeof(pv_PtrArray));
    ret->array = MALLOC(5 * sizeof(Ptr));
    ret->len = 0;
    ret->alloclen = 5;
    ret->allocrange = 5;
    ret->delfunc = NULL;
    ret->cmpfunc = defaultCmp;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
PtrArray
PtrArray_newFakeCopy(PtrArray base)
{
    PtrArray ret;
    
    ret = (PtrArray)MALLOC(sizeof(pv_PtrArray));
    ret->len = base->len;
    ret->alloclen = base->alloclen;
    ret->allocrange = base->allocrange;
    ret->array = MALLOC(ret->alloclen * sizeof(Ptr));
    ret->delfunc = NULL;
    ret->cmpfunc = base->cmpfunc;
    
    memCOPY(ret->array, base->array, sizeof(Ptr) * ret->len);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
PtrArray
PtrArray_newFull(PtrArrayPos startsize, PtrArrayPos sizebloc, PtrFunc delfunc, PtrCmpFunc cmpfunc)
{
    PtrArray ret;
    
    ret = (PtrArray)MALLOC(sizeof(pv_PtrArray));
    ret->alloclen = MAX(startsize, 1);
    ret->array = MALLOC(ret->alloclen * sizeof(Ptr));
    ret->len = 0;
    ret->allocrange = MAX(sizebloc, 1);
    ret->delfunc = delfunc;
    if (cmpfunc == NULL)
    {
        ret->cmpfunc = defaultCmp;
    }
    else
    {
        ret->cmpfunc = cmpfunc;
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
PtrArray_del(PtrArray array)
{
    PtrArrayPos i;
    if (array->delfunc != NULL)
    {
        for (i = 0; i < array->len; i++)
        {
            array->delfunc(array->array[i]);
        }
    }
    FREE(array->array);
    FREE(array);
}

/*----------------------------------------------------------------------------*/
void
PtrArray_clear(PtrArray array)
{
    PtrArrayPos i;
    if (array->delfunc != NULL)
    {
        for (i = 0; i < array->len; i++)
        {
            array->delfunc(array->array[i]);
        }
    }
    array->alloclen = array->allocrange;
    FREE(array->array);
    array->array = MALLOC(array->alloclen * sizeof(Ptr));
    array->len = 0;
}

/*----------------------------------------------------------------------------*/
void
PtrArray_append(PtrArray array, Ptr ptr)
{
    if (array->len == array->alloclen)
    {
        /*need growing*/
        array->alloclen += array->allocrange;
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
    /*add the pointer*/
    array->array[array->len++] = ptr;
}

/*----------------------------------------------------------------------------*/
void
PtrArray_prepend(PtrArray array, Ptr ptr)
{
    Ptr* newarray;
    PtrArrayPos i;
    
    if (array->len == array->alloclen)
    {
        /*need growing*/
        array->alloclen += array->allocrange;
        newarray = MALLOC(array->alloclen * sizeof(Ptr));
        for (i = array->len; i > 0; i--)
        {
            newarray[i] = array->array[i - 1];
        }
        newarray[0] = ptr;
        FREE(array->array);
        array->array = newarray;
        array->len++;
    }
    else
    {
        for (i = array->len; i > 0; i--)
        {
            array->array[i] = array->array[i - 1];
        }
        array->array[0] = ptr;
        array->len++;
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_prependFast(PtrArray array, Ptr ptr)
{
    if (array->len == array->alloclen)
    {
        /*need growing*/
        /*TODO: reallocation copies array->array[0] although it will be moved again later*/
        array->alloclen += array->allocrange;
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
    array->array[array->len++] = array->array[0];
    array->array[0] = ptr;
}

/*----------------------------------------------------------------------------*/
void
PtrArray_insert(PtrArray array, Ptr ptr, PtrArrayPos pos)
{
    Ptr* newarray;
    PtrArrayPos i;
    
    if (pos == 0)
    {
        PtrArray_prepend(array, ptr);
    }
    else if (pos >= array->len)
    {
        PtrArray_append(array, ptr);
    }
    else
    {
        if (array->len == array->alloclen)
        {
            /*need growing*/
            array->alloclen += array->allocrange;
            newarray = MALLOC(array->alloclen * sizeof(Ptr));
            for (i = 0; i < pos; i++)
            {
                newarray[i] = array->array[i];
            }
            newarray[pos] = ptr;
            for (i = pos; i < array->len; i++)
            {
                newarray[i + 1] = array->array[i];
            }
            FREE(array->array);
            array->array = newarray;
            array->len++;
        }
        else
        {
            for (i = array->len; i > pos; i--)
            {
                array->array[i] = array->array[i - 1];
            }
            array->array[pos] = ptr;
            array->len++;
        }
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_insertFast(PtrArray array, Ptr ptr, PtrArrayPos pos)
{
    if (pos == 0)
    {
        PtrArray_prepend(array, ptr);
    }
    else if (pos >= array->len)
    {
        PtrArray_append(array, ptr);
    }
    else
    {
        if (array->len == array->alloclen)
        {
            /*need growing*/
            /*TODO: reallocation copies array->array[pos] although it will be moved again later*/
            array->alloclen += array->allocrange;
            array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
        }
        array->array[array->len++] = array->array[pos];
        array->array[pos] = ptr;
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_insertSorted(PtrArray array, Ptr ptr)
{
    PtrArrayPos i;
    Bool b;
    
    ASSERT(array->cmpfunc != NULL, return);
    
    /*i = 0;
    while ((i < array->len) && (array->cmpfunc(&(ptr), &(array->array[i])) > 0))
    {
        i++;
    }
    PtrArray_insert(array, ptr, i);*/
    searchSorted(array, ptr, &b, &i);
    PtrArray_insert(array, ptr, i);
}

/*----------------------------------------------------------------------------*/
void
PtrArray_remove(PtrArray array, Ptr ptr)
{
    PtrArrayPos i, pos;
    Ptr p;
    
    /*search*/
    pos = 0;
    while ((pos < array->len) && (array->array[pos] != ptr))
    {
        pos++;
    }
    if (pos == array->len)
    {
        return;
    }

    /*delete*/
    if (array->delfunc != NULL)
    {
        array->delfunc(array->array[pos]);
    }
    p = array->array[pos];
    for (i = pos + 1; i < array->len; i++)
    {
        array->array[i - 1] = array->array[i];
    }
    array->len--;
    if ((array->alloclen - array->len == array->allocrange) && (array->len != 0))
    {
        /*need shrinking*/
        array->alloclen -= array->allocrange;
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_removeFast(PtrArray array, Ptr ptr)
{
    PtrArrayPos pos;
    Ptr p;
    
    /*search*/
    pos = 0;
    while ((pos < array->len) && (array->array[pos] != ptr))
    {
        pos++;
    }
    if (pos == array->len)
    {
        return;
    }

    /*delete*/
    if (array->delfunc != NULL)
    {
        array->delfunc(array->array[pos]);
    }
    p = array->array[pos];
    array->array[pos] = array->array[--array->len];
    if ((array->alloclen - array->len == array->allocrange) && (array->len != 0))
    {
        /*need shrinking*/
        array->alloclen -= array->allocrange;
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_removeIt(PtrArray array, PtrArrayIterator it)
{
    PtrArrayPos i, pos;
    Ptr p;
    
    if (it == NULL)
    {
        return;
    }
    
    pos = it - array->array;
    ASSERT(pos < array->len, return);

    /*delete*/
    if (array->delfunc != NULL)
    {
        array->delfunc(array->array[pos]);
    }
    p = array->array[pos];
    for (i = pos + 1; i < array->len; i++)
    {
        array->array[i - 1] = array->array[i];
    }
    array->len--;
    if ((array->alloclen - array->len == array->allocrange) && (array->len != 0))
    {
        /*need shrinking*/
        array->alloclen -= array->allocrange;
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_removeItFast(PtrArray array, PtrArrayIterator it)
{
    PtrArrayPos pos;
    Ptr p;
    
    pos = it - array->array;
    ASSERT(pos < array->len, return);

    /*delete*/
    if (array->delfunc != NULL)
    {
        array->delfunc(array->array[pos]);
    }
    p = array->array[pos];
    array->array[pos] = array->array[--array->len];
    if ((array->alloclen - array->len == array->allocrange) && (array->len != 0))
    {
        /*need shrinking*/
        array->alloclen -= array->allocrange;
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_removeRange(PtrArray array, PtrArrayPos start, PtrArrayPos end)
{
    PtrArrayPos i1, i2;
    
    ASSERT(start < array->len, return)
    ASSERT(end < array->len, return)
    ASSERT(start <= end, return)

    i1 = start;
    i2 = end + 1;
    while ((i1 <= end) && (i2 < array->len))
    {
        if (array->delfunc != NULL)
        {
            array->delfunc(array->array[i1]);
        }
        array->array[i1++] = array->array[i2++];
    }
    if (array->delfunc != NULL)
    {
        /*some may not have been deleted*/
        while (i1 <= end)
        {
            array->delfunc(array->array[i1++]);
        }
    }
        
    /*some may not have been moved down*/
    while (i2 < array->len)
    {
        array->array[i1++] = array->array[i2++];
    }
    
    array->len -= (end - start + 1);
    
    if (array->alloclen - array->len >= array->allocrange)
    {
        if (array->len < array->allocrange)
        {
            array->alloclen = array->allocrange;
        }
        else
        {
            array->alloclen = array->len;
        }
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_removeRangeFast(PtrArray array, PtrArrayPos start, PtrArrayPos end)
{
    PtrArrayPos i1, i2;
    
    ASSERT(start < array->len, return)
    ASSERT(end < array->len, return)
    ASSERT(start <= end, return)

    i1 = start;
    i2 = array->len - 1;
    while ((i1 <= end) && (i1 < i2))
    {
        if (array->delfunc != NULL)
        {
            array->delfunc(array->array[i1]);
        }
        array->array[i1++] = array->array[i2--];
    }
    if (array->delfunc != NULL)
    {
        /*some haven't been deleted*/
        while (i1 <= end)
        {
            array->delfunc(array->array[i1++]);
        }
    }
    array->len -= (end - start + 1);
    
    if (array->alloclen - array->len >= array->allocrange)
    {
        if (array->len < array->alloclen)
        {
            array->alloclen = array->allocrange;
        }
        else
        {
            array->alloclen = array->len;
        }
        array->array = REALLOC(array->array, array->alloclen * sizeof(Ptr));
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_sort(PtrArray array)
{
    ASSERT(array->cmpfunc != NULL, return);
    
    qsort(array->array, array->len, sizeof(Ptr), (int(*)(const void*,const void*))array->cmpfunc);
}

/*----------------------------------------------------------------------------*/
void
PtrArray_sortRange(PtrArray array, PtrArrayPos start, PtrArrayPos end)
{
    ASSERT(array->cmpfunc != NULL, return);
    ASSERT(start < array->len, return);
    ASSERT(end < array->len, return);
    ASSERT(start <= end, return);
    
    qsort(array->array + start, end - start + 1, sizeof(Ptr), (int(*)(const void*,const void*))array->cmpfunc);
}

/*----------------------------------------------------------------------------*/
PtrArrayIterator
PtrArray_find(PtrArray array, Ptr base)
{
    PtrArrayIterator i;

    ASSERT(array->cmpfunc != NULL, return NULL);
    
    i = PtrArray_START(array);
    while ((i != PtrArray_STOP(array)) && (array->cmpfunc(i, &base) != 0))
    {
        i++;
    }
    return ((i == PtrArray_STOP(array)) ? NULL : i);
}

/*----------------------------------------------------------------------------*/
PtrArrayIterator
PtrArray_findSorted(PtrArray array, Ptr base)
{
    PtrArrayPos i;
    Bool b;
    
    ASSERT(array->cmpfunc != NULL, return NULL);
    
    /*i = PtrArray_START(array);
    while ((i != PtrArray_STOP(array)) && (array->cmpfunc(i, &base) < 0))
    {
        i++;
    }
    return (((i != PtrArray_STOP(array)) && (array->cmpfunc(i, &base) == 0)) ? i : NULL);*/
    
    searchSorted(array, base, &b, &i);
    if (b)
    {
        return array->array + i;
    }
    else
    {
        return NULL;
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_foreach(PtrArray array, PtrFunc func)
{
    PtrArrayIterator i;

    for (i = PtrArray_START(array); i != PtrArray_STOP(array); i++)
    {
        func(*i);
    }
}

/*----------------------------------------------------------------------------*/
void
PtrArray_foreachWithData(PtrArray array, PtrFuncWithData func, void* data)
{
    PtrArrayIterator i;

    for (i = PtrArray_START(array); i != PtrArray_STOP(array); i++)
    {
        func(*i, data);
    }
}
