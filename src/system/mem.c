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
 *   Memory management                                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "main.h"
#include "system/mem.h"
#include "tools/fonct.h"

#include <stdlib.h>
#include <stdio.h>

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
#ifdef DEBUG_MEM
typedef struct
{
    MemPointer p;
    const char* file;
    unsigned int line;
    size_t size;
} MemAllocated;

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static MemAllocated* debug_mem;
static unsigned int debug_mem_len;
static unsigned int debug_mem_alloclen;
static SDL_mutex* debug_mutex;
#endif

/******************************************************************************
 *############################################################################*
 *#                              Main functions                              #*
 *############################################################################*
 ******************************************************************************/
void
memInit()
{
#ifdef DEBUG_MEM
    debug_mem = malloc(sizeof(MemAllocated) * 50000);
    debug_mem_len = 0;
    debug_mem_alloclen = 50000;
    debug_mutex = SDL_CreateMutex();
#endif
}

/*----------------------------------------------------------------------------*/
void
memUninit()
{
#ifdef DEBUG_MEM
    unsigned int i;
    MemAllocated m;
    
    SDL_mutexP(debug_mutex);
    for (i = 0; i < debug_mem_len; i++)
    {
        m = debug_mem[i];
        printf("*** Allocated ressource at line %d of file %s not freed: %p, %d bytes.\n", m.line, m.file, m.p, m.size);
    }
    free(debug_mem);
    SDL_mutexV(debug_mutex);
    
    SDL_DestroyMutex(debug_mutex);
#endif
}

/*----------------------------------------------------------------------------*/
Uint32
memNbAlloc()
{
#ifdef DEBUG_MEM
    return debug_mem_len;
#else
    return 0;
#endif
}

/*----------------------------------------------------------------------------*/
#ifdef DEBUG_MEM
MemPointer
pv_debugAlloc(MemPointer p, const char* file, unsigned int line, size_t size, Bool print)
{
    if (p == NULL)
    {
        return NULL;
    }

    SDL_mutexP(debug_mutex);
    
    if (debug_mem_len == debug_mem_alloclen)
    {
        debug_mem_alloclen += 10000;
        debug_mem = realloc(debug_mem, sizeof(MemAllocated) * debug_mem_alloclen);
    }
    debug_mem[debug_mem_len].p = p;
    debug_mem[debug_mem_len].file = file;
    debug_mem[debug_mem_len].line = line;
    debug_mem[debug_mem_len].size = size;
    debug_mem_len++;
    if (print)
    {
        printf("MEM ALLOC: %p => %d bytes in '%s' line %d\n", p, size, file, line);
    }
    
    SDL_mutexV(debug_mutex);
    return p;
}

/*----------------------------------------------------------------------------*/
MemPointer
pv_debugFree(MemPointer p, const char* file, unsigned int line, Bool print)
{
    unsigned int i;
    
    SDL_mutexP(debug_mutex);
    i = debug_mem_len;
    /*TODO: maybe switch between forward and backward searching when needed*/
    while ((i > 0) && (debug_mem[i - 1].p != p))
    {
        i--;
    }
    if (i == 0)
    {
        printf("Trying to free a ressource not allocated at line %d of file %s: %p\n", line, file, p);
    }
    else
    {
        i--;
        if (print)
        {
            printf("MEM FREE : %p => %d bytes in '%s' line %d\n", p, debug_mem[i].size, file, line);
            printf("    was allocated in '%s' line %d\n", debug_mem[i].file, debug_mem[i].line);
        }

        debug_mem[i] = debug_mem[--debug_mem_len];
        if ((debug_mem_alloclen - debug_mem_len > 10000) & (debug_mem_alloclen > 50000))
        {
            debug_mem_alloclen -= 10000;
            debug_mem = realloc(debug_mem, sizeof(MemAllocated) * debug_mem_alloclen);
        }
    }
    SDL_mutexV(debug_mutex);
    return p;
}
#endif

/*----------------------------------------------------------------------------*/
#ifndef HAVE_MEMCPY
void*
pv_memcpy(void* dest, const void* src, size_t n)
{
    void* ret = dest;
    Uint8* d;
    Uint8* s;
    while (n--)
    {
        d = (Uint8*)dest;
        s = (Uint8*)src;
        *d = *s;
        dest = (void*)(++d);
        src = (void*)(++s);
    }
    return ret;
}
#endif
