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
 *   Graphical cursor                                                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/impl/cursor.h"

#include "graphics/input.h"
#include "graphics/gl2dobject.h"
#include "graphics/glsurface.h"
#include "graphics/glrect.h"
#include "graphics/color.h"
#include "graphics/impl/impl.h"

#include "tools/fonct.h"
#include "tools/varvalidator.h"

#include "core/core.h"
#include "core/ptrarray.h"
#include "core/string.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_Cursor
{
    String      name;
    GlSurface   surf;
    Sint16      hotspotx;
    Sint16      hotspoty;
};

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static PtrArray _cursors;
static pv_Cursor _defaultcursor;
static Cursor _currentcursor;
static Cursor _defcursor;
static Cursor _nextcursor;
static Gl2DObject _globj;

/*positions*/
static Gl2DCoord _cursorx;      /*virtual cursor X position*/
static Gl2DCoord _cursory;      /*virtual cursor Y position*/
static Gl2DCoord _clipcursorx;  /*in-screen cursor X position*/
static Gl2DCoord _clipcursory;  /*in-screen cursor Y position*/
static Gl2DCoord _xrel;         /*cumul of relative X events*/
static Gl2DCoord _yrel;         /*cumul of relative Y events*/

/*buttons*/
#define BUTTONS_NB 3
static Bool _holdbutton[BUTTONS_NB];
static Uint16 _nbclics[BUTTONS_NB];
static Bool _firstpress[BUTTONS_NB];    /*FALSE if the first clic was begun before (holdbutton was TRUE)*/
static Uint16 _wheeldown;
static Uint16 _wheelup;
static MouseButton _buttonhold;

/*special*/
static Bool _ignorenextrel;    /*ignore next relative event*/

/*parameters*/
static Bool _grabbed;
static Bool _controlmode;
static volatile Bool _nextcontrolmode;
static Uint16 _mouseacceleration;
static Uint16 _mousedeadzone;

/*limits*/
static GlRect _screen_rect;
static GlRect _active_in_rect;
static GlRect _active_out_rect;
static Gl2DSize _active_in;
static Gl2DSize _active_out;
static Gl2DSize _active_width;
static Bool _active_reset;

/*core things*/
static CoreID MOD_ID = CORE_INVALID_ID;
static Var _prefsvar = NULL;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static void
Cursor_del(Cursor cursor)
{
    String_del(cursor->name);
    GlSurface_del(cursor->surf);
    FREE(cursor);
}

/*----------------------------------------------------------------------------*/
static int
Cursor_cmp(Cursor* c1, Cursor* c2)
{
    return String_cmp(&(*c1)->name, &(*c2)->name);
}

/*----------------------------------------------------------------------------*/
static void
setCursor(Cursor cursor)
{
    ASSERT_MAINTHREAD;
    
    _nextcursor = _currentcursor = cursor;
    Gl2DObject_relink(_globj, _currentcursor->surf);
    Gl2DObject_setPos(_globj, _clipcursorx - _currentcursor->hotspotx, _clipcursory - _currentcursor->hotspoty, 0);
}

/*----------------------------------------------------------------------------*/
static void
correctActiveRects(void)
{
    _active_in_rect.x = _screen_rect.x + _active_in;
    _active_in_rect.y = _screen_rect.y + _active_in;
    _active_in_rect.w = _screen_rect.w - 2 * _active_in;
    _active_in_rect.h = _screen_rect.h - 2 * _active_in;
    _active_out_rect.x = _screen_rect.x - _active_out;
    _active_out_rect.y = _screen_rect.y - _active_out;
    _active_out_rect.w = _screen_rect.w + 2 * _active_out;
    _active_out_rect.h = _screen_rect.h + 2 * _active_out;
}

/*----------------------------------------------------------------------------*/
static void
cursorAdd(Var v)
{
    Cursor cursor;
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "pict", "");
    VarValidator_declareIntVar(valid, "hotspot_x", 0);
    VarValidator_declareIntVar(valid, "hotspot_y", 0);
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    cursor = MALLOC(sizeof(pv_Cursor));
    cursor->name = String_newByCopy(Var_getName(v));
    cursor->surf = GlSurface_newFromFile(Var_getValueString(Var_getArrayElemByCName(v, "pict")));
    cursor->hotspotx = Var_getValueInt(Var_getArrayElemByCName(v, "hotspot_x"));
    cursor->hotspoty = Var_getValueInt(Var_getArrayElemByCName(v, "hotspot_y"));
    PtrArray_insertSorted(_cursors, cursor);
}

/*----------------------------------------------------------------------------*/
static Bool
gleventCallback(GlExtID data, GlEvent* event)
{
    (void)data;
    
    switch (event->type)
    {
        case GLEVENT_RESIZE:
            _screen_rect.w = event->event.resizeevent.width;
            _screen_rect.h = event->event.resizeevent.height;
            correctActiveRects();
            _clipcursorx = _cursorx = _screen_rect.w / 2;
            _clipcursory = _cursory = _screen_rect.h / 2;
            Gl2DObject_setPos(_globj, _clipcursorx - _currentcursor->hotspotx, _clipcursory - _currentcursor->hotspoty, 0);
            break;
        default:
            break;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
prefsCallback(Var prefs)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "activein", 0);
    VarValidator_declareIntVar(valid, "activeout", 100);
    VarValidator_declareIntVar(valid, "activereset", 1);
#ifdef __W32
    VarValidator_declareIntVar(valid, "acceleration", 1);
#else
    VarValidator_declareIntVar(valid, "acceleration", 2);
#endif
    VarValidator_declareIntVar(valid, "deadzone", 4);
    VarValidator_validate(valid, prefs);
    VarValidator_del(valid);
    
    inputSetMouseActiveEdges((Gl2DSize)Var_getValueInt(Var_getArrayElemByCName(prefs, "activein"))
                           , (Gl2DSize)Var_getValueInt(Var_getArrayElemByCName(prefs, "activeout"))
                           , (Var_getValueInt(Var_getArrayElemByCName(prefs, "activereset")) != 0) ? TRUE : FALSE);
    inputSetMouseAcceleration((Uint16)Var_getValueInt(Var_getArrayElemByCName(prefs, "acceleration")), (Uint16)Var_getValueInt(Var_getArrayElemByCName(prefs, "deadzone")));

    _prefsvar = prefs;
}

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
void
inputSetFullscreen(Bool fs)
{
    inputSetMouseControl(FALSE);
    
    _grabbed = fs ? TRUE : FALSE;
    
    /*apply*/
    if (_grabbed)
    {
        SDL_WM_GrabInput(SDL_GRAB_ON);
        _ignorenextrel = TRUE;
    }
    else
    {
        SDL_WM_GrabInput(SDL_GRAB_OFF);
    }
}


/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
cursorInit(void)
{
    int i;
    
    _cursors = PtrArray_newFull(10, 5, (PtrFunc)Cursor_del, (PtrCmpFunc)Cursor_cmp);
    _defaultcursor.name = NULL;
    _defaultcursor.surf = GlSurface_new(1, 1, FALSE);
    GlSurface_drawPixel(_defaultcursor.surf, 0, 0, GlColor_MAKE(0xFF, 0xFF, 0xFF, 0xFF));
    _defaultcursor.hotspotx = 0;
    _defaultcursor.hotspoty = 0;
    _nextcursor = _currentcursor = _defcursor = &_defaultcursor;
    _globj = Gl2DObject_new(_defaultcursor.surf, NULL, gleventCallback);
    Gl2DObject_setAlt(_globj, GL2D_HIGHEST);
    
    _cursorx = 0;
    _cursory = 0;
    _clipcursorx = 0;
    _clipcursory = 0;
    _xrel = 0;
    _yrel = 0;
    
    for (i = 0; i < BUTTONS_NB; i++)
    {
        _holdbutton[i] = FALSE;
        _nbclics[i] = 0;
        _firstpress[i] = FALSE;
    }
    _wheeldown = 0;
    _wheelup = 0;
    _buttonhold = MOUSEBUTTON_NONE;
    
    _ignorenextrel = FALSE;
    
    _grabbed = FALSE;
    _controlmode = FALSE;
    _nextcontrolmode = FALSE;
    _mouseacceleration = 1;
    _mousedeadzone = 0;
    
    GlRect_MAKE(_screen_rect, 0, 0, 10, 10);
    _active_in_rect = _active_out_rect = _screen_rect;
    _active_in = 0;
    _active_out = 0;
    _active_width = 0;
    _active_reset = FALSE;
    
    SDL_ShowCursor(SDL_DISABLE);
    
    MOD_ID = coreDeclareModule("cursor", NULL, NULL, NULL, prefsCallback, NULL, NULL);
    _prefsvar = NULL;
}

/*----------------------------------------------------------------------------*/
void
cursorUninit(void)
{
    Gl2DObject_del(_globj);
    PtrArray_del(_cursors);
    GlSurface_del(_defaultcursor.surf);
    
    SDL_ShowCursor(SDL_ENABLE);
    SDL_WM_GrabInput(SDL_DISABLE);
    
    shellPrintf(LEVEL_INFO, "Cursor module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
cursorSetCursors(Var v)
{
    VarArrayPos i;
    
    setCursor(&_defaultcursor);
    PtrArray_clear(_cursors);
    
    ASSERT(Var_getType(v) == VAR_ARRAY, return);
    
    for (i = 0; i < Var_getArraySize(v); i++)
    {
        cursorAdd(Var_getArrayElemByPos(v, i));
    }
}

/*----------------------------------------------------------------------------*/
void
cursorProcessEvent(SDL_Event* event)
{
    switch (event->type)
    {
        case SDL_MOUSEMOTION:
            _xrel += event->motion.xrel;
            _yrel += event->motion.yrel;
            break;
        
        case SDL_MOUSEBUTTONDOWN:
            switch (event->button.button)
            {
                case SDL_BUTTON_LEFT:
                    _holdbutton[0] = TRUE;
                    _buttonhold = MOUSEBUTTON_LEFT;
                    if (_nbclics[0] == 0)
                    {
                        _firstpress[0] = TRUE;
                    }
                    break;
                case SDL_BUTTON_RIGHT:
                    _holdbutton[1] = TRUE;
                    _buttonhold = MOUSEBUTTON_RIGHT;
                    if (_nbclics[1] == 0)
                    {
                        _firstpress[1] = TRUE;
                    }
                    break;
                case SDL_BUTTON_MIDDLE:
                    _holdbutton[2] = TRUE;
                    _buttonhold = MOUSEBUTTON_MIDDLE;
                    if (_nbclics[2] == 0)
                    {
                        _firstpress[2] = TRUE;
                    }
                    break;
#ifdef SDL_BUTTON_WHEELDOWN
                case SDL_BUTTON_WHEELDOWN:
                    _wheeldown++;
                    break;
#endif
#ifdef SDL_BUTTON_WHEELUP
                case SDL_BUTTON_WHEELUP:
                    _wheelup++;
                    break;
#endif
                default:
                    break;
            }
            break;
            
        case SDL_MOUSEBUTTONUP:
            switch (event->button.button)
            {
                case SDL_BUTTON_LEFT:
                    _holdbutton[0] = FALSE;
                    _buttonhold = MOUSEBUTTON_NONE;
                    _nbclics[0]++;
                    break;
                case SDL_BUTTON_RIGHT:
                    _holdbutton[1] = FALSE;
                    _buttonhold = MOUSEBUTTON_NONE;
                    _nbclics[1]++;
                    break;
                case SDL_BUTTON_MIDDLE:
                    _holdbutton[2] = FALSE;
                    _buttonhold = MOUSEBUTTON_NONE;
                    _nbclics[2]++;
                    break;
                default:
                    break;
            }
            break;
    }
}

/*----------------------------------------------------------------------------*/
void
cursorCollectEvents(void)
{
    GlEvent event;
    int i;
    
    ASSERT_MAINTHREAD;
    
    event.type = GLEVENT_MOUSE;
    
    /*change the cursor if needed*/
    if (_nextcursor != _currentcursor)
    {
        setCursor(_nextcursor);
    }
    _nextcursor = _defcursor;
    
    if (_controlmode)
    {
        if (!_ignorenextrel)
        {
            /*produce a control event*/
            event.event.mouseevent.type = MOUSEEVENT_CONTROL;
            event.event.mouseevent.x = _xrel;
            event.event.mouseevent.y = _yrel;
            event.event.mouseevent.button = MOUSEBUTTON_NONE;
            graphicsProcessEvent(&event);
        }
    }
    else
    {
        /*update the position*/
        if (!_ignorenextrel)
        {
            if (!_grabbed)
            {
                /*correct released motion*/
                int newx, newy;
                
                SDL_GetMouseState(&newx, &newy);
                _xrel = newx - _cursorx;
                _yrel = newy - _cursory;
                _cursorx = (Gl2DSize)newx;
                _cursory = (Gl2DSize)newy;
            }
            else
            {
                if (ABS(_xrel) + ABS(_yrel) > _mousedeadzone)
                {
                    _cursorx += _xrel * _mouseacceleration;
                    _cursory += _yrel * _mouseacceleration;
                }
                else
                {
                    _cursorx += _xrel;
                    _cursory += _yrel;
                }
            }
        
            returnInsideRect(&_active_out_rect, &_cursorx, &_cursory);
            _clipcursorx = _cursorx;
            _clipcursory = _cursory;
            returnInsideRect(&_screen_rect, &_clipcursorx, &_clipcursory);
            Gl2DObject_setPos(_globj, _clipcursorx - _currentcursor->hotspotx, _clipcursory - _currentcursor->hotspoty, 0);

            if (_active_width > 0)
            {
                Gl2DCoord x, y;
                
                x = _cursorx;
                y = _cursory;
                
                if (x < _active_in_rect.x)
                {
                    x -= _active_in_rect.x;
                }
                else if (x >= _active_in_rect.x + _active_in_rect.w)
                {
                    x -= _active_in_rect.x + _active_in_rect.w;
                }
                else
                {
                    x = 0;
                }
                if (y < _active_in_rect.y)
                {
                    y -= _active_in_rect.y;
                }
                else if (y >= _active_in_rect.y + _active_in_rect.h)
                {
                    y -= _active_in_rect.y + _active_in_rect.h;
                }
                else
                {
                    y = 0;
                }
                
                /*produce an edge event*/
                if ((x != 0) || (y != 0))
                {
                    event.event.mouseevent.type = MOUSEEVENT_EDGES;
                    event.event.mouseevent.x = (x * 1000) / _active_width;
                    event.event.mouseevent.y = (y * 1000) / _active_width;
                    event.event.mouseevent.button = MOUSEBUTTON_NONE;
                    graphicsProcessEvent(&event);
                }
                
                /*reset the cursor inside _active_in_rect if needed*/
                if (_active_reset)
                {
                    returnInsideRect(&_active_in_rect, &_cursorx, &_cursory);
                }
            }
        }
    }

    /*common event fields*/
    event.event.mouseevent.x = _clipcursorx;
    event.event.mouseevent.y = _clipcursory;
    
    /*produce wheel events*/
    event.event.mouseevent.type = MOUSEEVENT_WHEELDOWN;
    while (_wheeldown != 0)
    {
        graphicsProcessEvent(&event);
        _wheeldown--;
    }
    event.event.mouseevent.type = MOUSEEVENT_WHEELUP;
    while (_wheelup != 0)
    {
        graphicsProcessEvent(&event);
        _wheelup--;
    }
    
    /*produce buttons events*/
    for (i = 0; i < BUTTONS_NB; i++)
    {
        if (i == 0)
        {
            event.event.mouseevent.button = MOUSEBUTTON_LEFT;
        }
        else if (i == 1)
        {
            event.event.mouseevent.button = MOUSEBUTTON_RIGHT;
        }
        else
        {
            event.event.mouseevent.button = MOUSEBUTTON_MIDDLE;
        }
        if ((_nbclics[i] > 0) && (!_firstpress[i]))
        {
            event.event.mouseevent.type = MOUSEEVENT_BUTTONRELEASED;
            graphicsProcessEvent(&event);
            _nbclics[i]--;
        }
        _firstpress[i] = FALSE;
        while (_nbclics[i] > 0)
        {
            event.event.mouseevent.type = MOUSEEVENT_BUTTONPRESSED;
            graphicsProcessEvent(&event);
            event.event.mouseevent.type = MOUSEEVENT_BUTTONRELEASED;
            graphicsProcessEvent(&event);
            _nbclics[i]--;
        }
        if (_holdbutton[i])
        {
            event.event.mouseevent.type = MOUSEEVENT_BUTTONPRESSED;
            graphicsProcessEvent(&event);
            _holdbutton[i] = 0;     /*to avoid MOUSEEVENT_BUTTONPRESSED repeating*/
        }
    }

    /*produce motion event*/
    event.event.mouseevent.type = MOUSEEVENT_MOVE;
    event.event.mouseevent.button = _buttonhold;
    graphicsProcessEvent(&event);
    
    if ((_xrel | _yrel) != 0)
    {
        _ignorenextrel = FALSE;
    }
    if (_controlmode)
    {
        if (!_nextcontrolmode)
        {
            /*quit control mode*/
            _controlmode = FALSE;
            if (!_grabbed)
            {
                SDL_WM_GrabInput(SDL_DISABLE);
                _ignorenextrel = TRUE;
            }
        }
    }
    else
    {
        if (_nextcontrolmode)
        {
            /*enter control mode*/
            _controlmode = TRUE;
            if (!_grabbed)
            {
                SDL_WM_GrabInput(SDL_ENABLE);
                _ignorenextrel = TRUE;
            }
        }
    }
    _xrel = _yrel = 0;
}

/*----------------------------------------------------------------------------*/
void
cursorGetPos(Gl2DCoord* posx, Gl2DCoord* posy)
{
    *posx = _clipcursorx;
    *posy = _clipcursory;
}

/******************************************************************************
 *############################################################################*
 *#                            input.h functions                             #*
 *############################################################################*
 ******************************************************************************/
void
inputSetMouseControl(Bool control)
{
    _nextcontrolmode = control;
}

/*----------------------------------------------------------------------------*/
void
inputSetMouseActiveEdges(Gl2DSize insize, Gl2DSize outsize, Bool reset)
{
    _active_in = insize;
    _active_out = outsize;
    _active_width = insize + outsize;
    _active_reset = reset;
    correctActiveRects();
    
    /*tell preferences about this*/
    if (_prefsvar != NULL)
    {
        Var_setInt(Var_getArrayElemByCName(_prefsvar, "activein"), _active_in);
        Var_setInt(Var_getArrayElemByCName(_prefsvar, "activeout"), _active_out);
        Var_setInt(Var_getArrayElemByCName(_prefsvar, "activereset"), _active_reset);
    }
}

/*----------------------------------------------------------------------------*/
void
inputSetMouseAcceleration(Uint16 factor, Uint16 deadzone)
{
    _mouseacceleration = MAX(factor, 1);
    _mousedeadzone = MAX(deadzone, 0);
    
    /*tell preferences about this*/
    if (_prefsvar != NULL)
    {
        Var_setInt(Var_getArrayElemByCName(_prefsvar, "acceleration"), _mouseacceleration);
        Var_setInt(Var_getArrayElemByCName(_prefsvar, "deadzone"), _mousedeadzone);
    }
}

/*----------------------------------------------------------------------------*/
Cursor
inputGetCursor(String name)
{
    pv_Cursor c;
    PtrArrayIterator it;
    
    c.name = name;
    it = PtrArray_findSorted(_cursors, &c);
    return ((it == NULL) ? &_defaultcursor : ((Cursor)*it));
}

/*----------------------------------------------------------------------------*/
void
inputSetCurrentCursor(Cursor cursor)
{
    _defcursor = cursor;
}

/*----------------------------------------------------------------------------*/
void
inputSetTempCursor(Cursor cursor)
{
    _nextcursor = cursor;
}
