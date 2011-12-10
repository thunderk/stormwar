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

/*!
 * \file
 * \brief This is the memory manager.
 *
 * The memory manager can be used to debug memory allocations and freeings.
 * It can keep track of not freed allocated stuff and not allocated freeings.
 * If DEBUG_MEMPRINT is defined in a file, all memory management performed in this
 * file will be printed to stdout.
 */

#ifndef _SW_MEM_H_
#define _SW_MEM_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include <stdlib.h>

#ifdef HAVE_STRING_H
    #include <string.h>
#else
    #ifdef HAVE_STRINGS_H
        #include <strings.h>
    #endif
#endif

/******************************************************************************
 *                                   Macros                                   *
 ******************************************************************************/
/*! \brief Common pointer type used for memory allocations and freeings. */
typedef void* MemPointer;

#undef MALLOC
#undef FREE
#undef REALLOC
#undef PV_DEBUG_MEMPRINT
#undef debugALLOC
#undef debugFREE

#ifdef DEBUG_MEMPRINT
    #define PV_DEBUG_MEMPRINT 1
#else
    #define PV_DEBUG_MEMPRINT 0
#endif

#ifdef DEBUG_MEM
    #define MALLOC(_size_) pv_debugAlloc(malloc(_size_),__FILE__,__LINE__,_size_,PV_DEBUG_MEMPRINT)
    #define FREE(_p_) free(pv_debugFree(_p_,__FILE__,__LINE__,PV_DEBUG_MEMPRINT))
    #define REALLOC(_p_,_size_) pv_debugAlloc(realloc(pv_debugFree(_p_,__FILE__,__LINE__,PV_DEBUG_MEMPRINT),_size_),__FILE__,__LINE__,_size_,PV_DEBUG_MEMPRINT)
    #define debugALLOC(_p_,_size_) pv_debugAlloc((MemPointer)_p_,__FILE__,__LINE__,_size_,PV_DEBUG_MEMPRINT)
    #define debugFREE(_p_) pv_debugFree((MemPointer)_p_,__FILE__,__LINE__,PV_DEBUG_MEMPRINT)
#else
    /*! \brief Use this macro instead of the standard malloc function */
    #define MALLOC malloc
    /*! \brief Use this macro instead of the standard free function */
    #define FREE free
    /*! \brief Use this macro instead of the standard realloc function */
    #define REALLOC realloc
    /*! \brief Debug function to trace memory allocation, already called by sw_malloc */
    #define debugALLOC(_p_,_size_) _p_
    /*! \brief Debug function to trace memory deallocation, already called by sw_free */
    #define debugFREE(_p_) _p_
#endif

#undef memCOPY
#ifdef HAVE_MEMCPY
    #define memCOPY memcpy
#else
    #define memCOPY pv_memcpy
#endif


/******************************************************************************
 *############################################################################*
 *#                              Mem functions                               #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize memory manager.
 */
void memInit(void);

/*!
 * \brief Stop memory manager.
 */
void memUninit(void);

/*!
 * \brief Return the current number of allocated chunks.
 *
 * \return Number of allocated things.
 */
Uint32 memNbAlloc(void);


/*----------------------------------------------------------------------------*/
#ifdef DEBUG_MEM
/*!
 * \brief Support function for debugALLOC macro.
 *
 * Don't use this function directly, only use the macro debugALLOC.
 */
MemPointer pv_debugAlloc(MemPointer p, const char* file, unsigned int line, size_t size, Bool print);

/*!
 * \brief Support function for debugFREE macro.
 *
 * Don't use this function directly, only use the macro debugFREE.
 */
MemPointer pv_debugFree(MemPointer p, const char* file, unsigned int line, Bool print);
#endif /*DEBUG_MEM*/


/*----------------------------------------------------------------------------*/
#ifndef HAVE_MEMCPY
/*!
 * \brief Builtin replacement for memcpy function.
 *
 * Don't use this function directly, only use the macro memCOPY.
 * \param dest - Destination position.
 * \param src - Source position.
 * \param n - Number of bytes.
 * \return dest.
 */
void* pv_memcpy(void* dest, const void* src, size_t n);
#endif

#endif
