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
 *   Camera control                                                           *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/internal/game.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"
#include "graphics/input.h"

#include "core/core.h"
#include "core/string.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Statics                                   *
 ******************************************************************************/
#define CONTROL_NONE 0x00
#define CONTROL_ZOOM 0x01
#define CONTROL_ROT 0x02
#define CONTROL_PAN 0x04
#define CONTROL_SCROLL 0x08
static Uint8 _control;

static Cursor _cursors[10][3];

/*core things*/
static CoreID MOD_ID = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
coreCallback(CoreEvent event)
{
    String s;
    const char* st[10] = {"none", "bottomleft", "bottom", "bottomright", "left", "normal", "right", "topleft", "top", "topright"};
    int i, j;
        
    if (event == CORE_DATALOADED)
    {
        s = String_new("");
        for (i = 0; i < 10; i++)
        {
            for (j = 0; j < 3; j++)
            {
                String_replace(s, st[i]);
                String_appendChar(s, '1' + (char)j);
                _cursors[i][j] = inputGetCursor(s);
            }
        }
        String_del(s);
        inputSetCurrentCursor(_cursors[5][0]);
    }
}

/*----------------------------------------------------------------------------*/
static void
chooseCursor(Gl2DCoord dx, Gl2DCoord dy, Gl2DCoord dmax)
{
    int dir, val;
    
    /*clamp values*/
    dx = MIN(dx, dmax - 1);
    dx = MAX(dx, -dmax + 1);
    dy = MIN(dy, dmax - 1);
    dy = MAX(dy, -dmax + 1);
    
    /*new direction*/
    if ((dx == 0) && (dy == 0))
    {
        dir = 5;
    }
    else if (dx >= 2 * ABS(dy))
    {
        dir = 6;
    }
    else if (dx <= -2 * ABS(dy))
    {
        dir = 4;
    }
    else if (dy >= 2 * ABS(dx))
    {
        dir = 2;
    }
    else if (dy <= -2 * ABS(dx))
    {
        dir = 8;
    }
    else
    {
        if (dx > 0)
        {
            if (dy < 0)
            {
                dir = 9;
            }
            else
            {
                dir = 3;
            }
        }
        else    /*x < 0*/
        {
            if (dy < 0)
            {
                dir = 7;
            }
            else
            {
                dir = 1;
            }
        }
    }
    
    /*new value*/
    if (dir != 5)
    {
        dx = ABS(dx);
        dy = ABS(dy);
        val = (MAX(dx, dy) * 3) / dmax;
        ASSERT(val < 3, return);
        inputSetTempCursor(_cursors[dir][val]);
    }
}

/*----------------------------------------------------------------------------*/
static Bool
gleventCallback(GlExtID data, GlEvent* event)
{
    (void)data;
    if (event->type == GLEVENT_MOUSE)
    {
        switch (event->event.mouseevent.type)
        {
            case MOUSEEVENT_CONTROL:
                if (_control & CONTROL_ROT)
                {
                    cameraRotateAround(-(float)event->event.mouseevent.x / 200.0f, -(float)event->event.mouseevent.y / 200.0f);
                    chooseCursor(event->event.mouseevent.x, event->event.mouseevent.y, 50);
                }
                if (_control & CONTROL_SCROLL)
                {
                    cameraScroll(-(float)event->event.mouseevent.x / 20.0f, -(float)event->event.mouseevent.y / 20.0f);
                    chooseCursor(event->event.mouseevent.x, event->event.mouseevent.y, 50);
                }
                if (_control & CONTROL_ZOOM)
                {
                    cameraZoom((float)event->event.mouseevent.y / 50.0f);
                    chooseCursor(0, event->event.mouseevent.y, 50);
                }
                if (_control & CONTROL_PAN)
                {
                    cameraPan(-(float)event->event.mouseevent.x / 300.0f);
                    chooseCursor(event->event.mouseevent.x, 0, 50);
                }
                break;
            case MOUSEEVENT_EDGES:
                cameraScroll(-(float)event->event.mouseevent.x / 300.0f, -(float)event->event.mouseevent.y / 300.0f);
                chooseCursor(event->event.mouseevent.x, event->event.mouseevent.y, 1000);
                break;
            case MOUSEEVENT_WHEELDOWN:
                cameraZoom(0.5);
                break;
            case MOUSEEVENT_WHEELUP:
                cameraZoom(-0.5);
                break;
            case MOUSEEVENT_BUTTONPRESSED:
                if (event->event.mouseevent.button == MOUSEBUTTON_MIDDLE)
                {
                    _control |= CONTROL_ROT;
                    inputSetMouseControl(TRUE);
                }
                break;
            case MOUSEEVENT_BUTTONRELEASED:
                if (event->event.mouseevent.button == MOUSEBUTTON_MIDDLE)
                {
                    _control &= (~CONTROL_ROT);
                    if (_control == CONTROL_NONE)
                    {
                        inputSetMouseControl(FALSE);
                    }
                }
                break;
            default:
                break;
        }
    }
    else if (event->type == GLEVENT_KEYBOARD)
    {
        switch (event->event.keyevent.keyfunc)
        {
            case KEY_PAGEUP:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    cameraZoom(-1.0f);
                }
                break;
            case KEY_PAGEDOWN:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    cameraZoom(1.0f);
                }
                break;
            case KEY_LEFT:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    cameraScroll(1.0f, 0.0f);
                }
                break;
            case KEY_RIGHT:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    cameraScroll(-1.0f, 0.0f);
                }
                break;
            case KEY_UP:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    cameraScroll(0.0f, 1.0f);
                }
                break;
            case KEY_DOWN:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    cameraScroll(0.0f, -1.0f);
                }
                break;
            case KEY_CTRL:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    _control |= CONTROL_ROT;
                    inputSetMouseControl(TRUE);
                }
                else
                {
                    _control &= (~CONTROL_ROT);
                    if (_control == CONTROL_NONE)
                    {
                        inputSetMouseControl(FALSE);
                    }
                }
                break;
            case KEY_SHIFT:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    _control |= CONTROL_ZOOM;
                    inputSetMouseControl(TRUE);
                }
                else
                {
                    _control &= (~CONTROL_ZOOM);
                    if (_control == CONTROL_NONE)
                    {
                        inputSetMouseControl(FALSE);
                    }
                }
                break;
            case KEY_SYS:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    _control |= CONTROL_PAN;
                    inputSetMouseControl(TRUE);
                }
                else
                {
                    _control &= (~CONTROL_PAN);
                    if (_control == CONTROL_NONE)
                    {
                        inputSetMouseControl(FALSE);
                    }
                }
                break;
            case KEY_ALT:
                if (event->event.keyevent.type == KEYEVENT_PRESSED)
                {
                    _control |= CONTROL_SCROLL;
                    inputSetMouseControl(TRUE);
                }
                else
                {
                    _control &= (~CONTROL_SCROLL);
                    if (_control == CONTROL_NONE)
                    {
                        inputSetMouseControl(FALSE);
                    }
                }
                break;
            default:
                break;
        }
    }
    
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
gameCameraInit(void)
{
    int i, j;
    
    MOD_ID = coreDeclareModule("gamecamera", coreCallback, NULL, NULL, NULL, NULL, NULL);
    graphicsAddEventCollector(GLEVENT_MOUSE | GLEVENT_KEYBOARD, gleventCallback);
    _control = CONTROL_NONE;
    
    for (i = 0; i < 10; i++)
    {
        for (j = 0; j < 3; j++)
        {
            _cursors[i][j] = inputGetCursor(STRING_NULL);
        }
    }
}

/*----------------------------------------------------------------------------*/
void
gameCameraUninit(void)
{
    inputSetMouseControl(FALSE);
}

/*----------------------------------------------------------------------------*/
void
gameCameraReset(WorldCoord x, WorldCoord y)
{
    cameraSetPos((Gl3DCoord)x, 0.0, (Gl3DCoord)y);
    cameraSetZoom(20.0);
    cameraSetAngle(1.5708, 1.1);
}
