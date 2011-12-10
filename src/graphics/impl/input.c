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
 *   Functions to control input behavior                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/core.h"
#include "graphics/input.h"

#include "tools/fonct.h"
#include "graphics/graphics.h"
#include "graphics/impl/keyboard.h"
#include "graphics/impl/cursor.h"
#include "graphics/impl/impl.h"

#include "SDL.h"
#include "SDL_mutex.h"

#include <math.h>

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
#ifndef NO_EVENT_QUEUE
static SDL_mutex*       _mutex;
#endif

/* Current time for the input loop (not for each SDL event) */
static Uint32           _curtime;

/* Resize event */
static Uint16           _resizew;
static Uint16           _resizeh;
static Bool             _ignorenextresize;

/* While datas are loading */
static volatile Bool    _freeze;

/* Core things */
static CoreID           MOD_ID;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static Bool
processOneEvent()
{
    /* Pay attention this can be run by a thread different from main ! */
    
    SDL_Event event;
    
    /*collect one event from SDL queue*/
    if (! SDL_PollEvent(&event))
    {
        /*no event left*/
        return FALSE;
    }
    
    switch (event.type)
    {
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONUP:
            /*pass the event to the cursor module*/
#ifndef NO_EVENT_QUEUE
            SDL_mutexP(_mutex);
#endif
            if (!_freeze)
            {
                cursorProcessEvent(&event);
            }
#ifndef NO_EVENT_QUEUE
            SDL_mutexV(_mutex);
#endif
            break;
        
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            /*pass the event to the keyboard module*/
            /*FIXME: emergency events won't be passed*/
#ifndef NO_EVENT_QUEUE
            SDL_mutexP(_mutex);
#endif
            if (!_freeze)
            {
                keyboardProcessEvent(&event);
            }
#ifndef NO_EVENT_QUEUE
            SDL_mutexV(_mutex);
#endif
            break;
            
        case SDL_QUIT:
            coreStop();
            break;

        
        case SDL_VIDEORESIZE:
            if (!_ignorenextresize)
            {
                _resizew = event.resize.w;
                _resizeh = event.resize.h;
            }
            _ignorenextresize = FALSE;
            break;
        
        default:
            ;
    }

    return TRUE;
}

/*----------------------------------------------------------------------------*/
static void
coreCallback(CoreEvent event)
{
    if (event == CORE_DATALOADING)
    {
        _freeze = TRUE;
    }
    if (event == CORE_DATALOADED)
    {
        _freeze = FALSE;
    }
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(volatile CoreID thread, CoreTime duration)
{
    (void)thread;
    (void)duration;
    
    /*process pending events into the queue*/
    while (processOneEvent())
    {
        ;
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
inputInit()
{
#ifndef NO_EVENT_QUEUE
    _mutex = SDL_CreateMutex();
#endif
    _curtime = getTicks();
    
    _resizew = 0;
    _resizeh = 0;
    _ignorenextresize = TRUE;       /*to ignore the window creation event*/
    _freeze = TRUE;
    
    MOD_ID = coreDeclareModule("input", coreCallback, NULL, NULL, NULL, NULL, threadCallback);
#ifndef NO_EVENT_QUEUE
    {
        volatile CoreID threadid;
        
        coreCreateThread(MOD_ID, "input", FALSE, &threadid);
        coreRequireThreadSlot(MOD_ID, threadid);
        coreSetThreadTimer(MOD_ID, threadid, 20);
    }
#endif
}

/*----------------------------------------------------------------------------*/
void
inputUninit()
{
#ifndef NO_EVENT_QUEUE
    SDL_DestroyMutex(_mutex);
#endif
    
    shellPrint(LEVEL_INFO, "Input module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
inputCollectEvents()
{
    ASSERT_MAINTHREAD;
    
#ifdef NO_EVENT_QUEUE
    while (processOneEvent())
    {
        ;
    }
#endif
    
    /*treat resize events for windowed mode*/
    if (_resizew != 0)
    {
        graphicsSetVideoMode(_resizew, _resizeh, FALSE);
        _resizew = 0;
        _resizeh = 0;
    }
    
    /*collect events*/
#ifndef NO_EVENT_QUEUE
    SDL_mutexP(_mutex);
#endif
    keyboardCollectEvents();
    cursorCollectEvents();
#ifndef NO_EVENT_QUEUE
    SDL_mutexV(_mutex);
#endif
}
