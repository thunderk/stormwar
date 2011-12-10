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
 *   Kernel loader and external events handler                                *
 *                                                                            *
  ***************************************************************************/

/*!
 * \mainpage StormWar Documentation
 *
 * StormWar is a 3D realtime strategy game, released under GPL license.
 *
 * This is the StormWar's source headers documentation for developers.<br>
 * Only module top interfaces are reported here, even if more documentation can be found
 * in these module's implementations.
 *
 * Website of StormWar can be found here: <a href="http://stormwar.free.fr">http://stormwar.free.fr</a>
 */

#ifndef _SW_MAIN_H_
#define _SW_MAIN_H_ 1

#include "SDL_types.h"

/******************************************************************************
 *                                  Defines                                   *
 ******************************************************************************/
#ifndef Bool
#define Bool Uint8
#endif

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#ifdef __W32
    #include "config_w32.h"
#endif
#ifdef __OSX__
    #include "config_osx.h"
#endif
#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "system/mem.h"
#include <stdlib.h>
#include <stdio.h>

#include "core/shell.h"
#include "core/types.h"
#include "core/constants.h"

/******************************************************************************
 *                                    i18n                                    *
 ******************************************************************************/
#include "core/i18n.h"

/******************************************************************************
 *                               Shared variables                             *
 ******************************************************************************/
/*! \brief Main thread ID */
extern Uint32 global_mainthread;

/*! \brief Counter used for debugging. */
extern Uint32 debugcount;

/******************************************************************************
 *                               Debugging tools                              *
 ******************************************************************************/
#undef ASSERT
#undef ASSERT_CRITICAL
#undef ASSERT_WARN
#ifdef ASSERTS
    #define ASSERT(_val_,_act_) if (!(_val_)) {shellPrintf(LEVEL_ERROR,_("Assertion failed in %s line %d:\n   %s"), __FILE__, __LINE__, #_val_);_act_;}
    #define ASSERT_WARN(_val_) if (!(_val_)) {shellPrintf(LEVEL_ERROR,_("Assertion failed in %s line %d:\n   %s"), __FILE__, __LINE__, #_val_);}
    #define ASSERT_CRITICAL(_val_) if (!(_val_)) {shellPrintf(LEVEL_ERROR,_("Assertion failed in %s line %d:\n   %s"), __FILE__, __LINE__, #_val_);error("main","ASSERT_CRITICAL",_("Critical assertion failed."));}
    #include "SDL_thread.h"
    #define ASSERT_MAINTHREAD if (SDL_ThreadID() != global_mainthread) {shellPrintf(LEVEL_ERROR,_("Assertion of main thread failed in %s line %d:\n   main=%X,current=%X"),__FILE__,__LINE__,global_mainthread,SDL_ThreadID());}
#else
    #define ASSERT(_val_,_act_)
    #define ASSERT_WARN(_val_)
    #define ASSERT_CRITICAL(_val_)
    #define ASSERT_MAINTHREAD
#endif

#undef DEBUG_COUNT
#define DEBUG_COUNT printf("%u\n", debugcount++)

/******************************************************************************
 *############################################################################*
 *#                              Main functions                              #*
 *############################################################################*
 ******************************************************************************/
/*! \brief Display an error message and exit the program
 *
 *  \param obj   - File in which error occured
 *  \param funct - Function's name where error occured
 *  \param desc  - An explicit description of the error type
 */
void
error(char * obj, char * funct, char * desc);

/*!
 * \brief Critical exit after an error.
 */
void
criticalExit(void);

#endif
