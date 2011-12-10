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
 *   Keyboard manager                                                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/impl/keyboard.h"

#include "graphics/impl/impl.h"
#include "core/core.h"
#include "kernel.h"

/******************************************************************************
 *                                   Types                                    *
 ******************************************************************************/
typedef Uint8 KeyboardMask;
#define KEYMASK_NONE 0x00
#define KEYMASK_SHIFT 0x01
#define KEYMASK_CTRL 0x02
#define KEYMASK_ALT 0x04
#define KEYMASK_MENU 0x08
#define KEYMASK_SYS 0x10

#define EMERGENCY_NONE      0x00
#define EMERGENCY_RESIZE    0x01
#define EMERGENCY_RESTART   0x02

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
#define MAX_EVENTS 10
static GlEvent _eventqueue[MAX_EVENTS];
static volatile int _eventnb;
static volatile KeyboardMask _keymask;
static volatile Uint8 _emergency;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
/*----------------------------------------------------------------------------*/


/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
keyboardInit(void)
{
    _eventnb = 0;
    _keymask = KEYMASK_NONE;
    _emergency = EMERGENCY_NONE;
    SDL_EnableUNICODE(1);
    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
}

/*----------------------------------------------------------------------------*/
void
keyboardUninit(void)
{
}

/*----------------------------------------------------------------------------*/
void
keyboardProcessEvent(SDL_Event* event)
{
    GlEvent* ev;

    /*TODO: release pressed keys when the queue is full*/
    /*TODO: catch emergency keys even when the queue is full*/
    
    if (_eventnb == MAX_EVENTS)
    {
        /*too much events, discarding all events*/
        return;
    }
    
    ev = _eventqueue + _eventnb;
    
    ev->type = GLEVENT_KEYBOARD;
    ev->event.keyevent.keyfunc = KEY_NONE;
    ev->event.keyevent.keychar = '\0';
    if ((event->key.keysym.unicode < 255 && event->key.keysym.unicode >= 32)
     || ((char)event->key.keysym.unicode == '\r')
     || ((char)event->key.keysym.unicode == '\t')
     || ((char)event->key.keysym.unicode == '\n')
     || ((char)event->key.keysym.unicode == '\b'))
    {
        ev->event.keyevent.keychar = (char)event->key.keysym.unicode;
    }
    else
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_LALT:
            case SDLK_RALT:
                if (event->type == SDL_KEYDOWN)
                {
                    _keymask |= KEYMASK_ALT;
                }
                else
                {
                    _keymask &= (~KEYMASK_ALT);
                }
                ev->event.keyevent.keyfunc = KEY_ALT;
                break;
            case SDLK_MENU:
                if (event->type == SDL_KEYDOWN)
                {
                    _keymask |= KEYMASK_MENU;
                }
                else
                {
                    _keymask &= (~KEYMASK_MENU);
                }
                ev->event.keyevent.keyfunc = KEY_MENU;
                break;
            case SDLK_LCTRL:
            case SDLK_RCTRL:
                if (event->type == SDL_KEYDOWN)
                {
                    _keymask |= KEYMASK_CTRL;
                }
                else
                {
                    _keymask &= (~KEYMASK_CTRL);
                }
                ev->event.keyevent.keyfunc = KEY_CTRL;
                break;
            case SDLK_LSUPER:
            case SDLK_RSUPER:
                if (event->type == SDL_KEYDOWN)
                {
                    _keymask |= KEYMASK_SYS;
                }
                else
                {
                    _keymask &= (~KEYMASK_SYS);
                }
                ev->event.keyevent.keyfunc = KEY_SYS;
                break;
            case SDLK_LSHIFT:
            case SDLK_RSHIFT:
                if (event->type == SDL_KEYDOWN)
                {
                    _keymask |= KEYMASK_SHIFT;
                }
                else
                {
                    _keymask &= (~KEYMASK_SHIFT);
                }
                ev->event.keyevent.keyfunc = KEY_SHIFT;
                break;
            case SDLK_LEFT:
                ev->event.keyevent.keyfunc = KEY_LEFT;
                break;
            case SDLK_RIGHT:
                ev->event.keyevent.keyfunc = KEY_RIGHT;
                break;
            case SDLK_UP:
                ev->event.keyevent.keyfunc = KEY_UP;
                break;
            case SDLK_DOWN:
                ev->event.keyevent.keyfunc = KEY_DOWN;
                break;
            case SDLK_PAGEUP:
                ev->event.keyevent.keyfunc = KEY_PAGEUP;
                break;
            case SDLK_PAGEDOWN:
                ev->event.keyevent.keyfunc = KEY_PAGEDOWN;
                break;
            case SDLK_ESCAPE:
                ev->event.keyevent.keyfunc = KEY_ESCAPE;
                break;
            case SDLK_F1:
                ev->event.keyevent.keyfunc = KEY_F1;
                break;
            case SDLK_F2:
                ev->event.keyevent.keyfunc = KEY_F2;
                break;
            case SDLK_F3:
                ev->event.keyevent.keyfunc = KEY_F3;
                break;
            case SDLK_F4:
                ev->event.keyevent.keyfunc = KEY_F4;
                break;
            case SDLK_F5:
                ev->event.keyevent.keyfunc = KEY_F5;
                break;
            case SDLK_F6:
                ev->event.keyevent.keyfunc = KEY_F6;
                break;
            case SDLK_F7:
                ev->event.keyevent.keyfunc = KEY_F7;
                break;
            case SDLK_F8:
                ev->event.keyevent.keyfunc = KEY_F8;
                break;
            case SDLK_F9:
                ev->event.keyevent.keyfunc = KEY_F9;
                break;
            case SDLK_F10:
                ev->event.keyevent.keyfunc = KEY_F10;
                break;
            case SDLK_F11:
                ev->event.keyevent.keyfunc = KEY_F11;
                break;
            case SDLK_F12:
                ev->event.keyevent.keyfunc = KEY_F12;
                break;
            default:
                ;
        }
    }
    
    /*emergency keys*/
    if ((_keymask == KEYMASK_CTRL) && (event->type == SDL_KEYUP))
    {
        if (ev->event.keyevent.keyfunc == KEY_F12)
        {
            /*emergency exit*/
            coreStop();
        }
        else if (ev->event.keyevent.keyfunc == KEY_F1)
        {
            _emergency |= EMERGENCY_RESIZE;
        }
        else if (ev->event.keyevent.keyfunc == KEY_F2)
        {
            _emergency |= EMERGENCY_RESTART;
        }
    }
    
    if ((ev->event.keyevent.keyfunc != KEY_NONE) || (ev->event.keyevent.keychar != '\0'))
    {
        /*event validated*/
        if (event->type == SDL_KEYDOWN)
        {
            ev->event.keyevent.type = KEYEVENT_PRESSED;
        }
        else
        {
            ev->event.keyevent.type = KEYEVENT_RELEASED;
        }
        _eventnb++;
    }
}

/*----------------------------------------------------------------------------*/
void
keyboardCollectEvents(void)
{
    int i;
    
    /*treat emergency cases*/
    if (_emergency & EMERGENCY_RESIZE)
    {
        graphicsSetVideoMode(800, 600, FALSE);
    }
    if (_emergency & EMERGENCY_RESTART)
    {
        kernelTitle();
    }
    _emergency = EMERGENCY_NONE;
    
    for (i = 0; i < _eventnb; i++)
    {
        graphicsProcessEvent(_eventqueue + i);
    }
    _eventnb = 0;
}
