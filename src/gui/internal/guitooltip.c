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
 *   Module to handle tooltip display                                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guitooltip.h"

#include "core/core.h"
#include "gui/internal/guitheme.h"
#include "graphics/gl2dobject.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
/*tooltip's top-level widget*/
static GuiWidget _widget;

/*last caller, NULL if there is no tooltip displayed*/
static GuiWidget _caller;

/*screen size*/
static Uint16 _screen_w;
static Uint16 _screen_h;

/*time*/
static CoreTime _time;
static Bool _delay;     /*appearing delay*/

/*core things*/
static CoreID MOD_ID = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static Bool
downEventCallback(GlExtID wid, GlEvent* event)
{
    GuiWidget widget = (GuiWidget)wid;
    ASSERT(_widget == widget, return FALSE);
    (void)widget;

    if (event->type == GLEVENT_RESIZE)
    {
        _screen_w = event->event.resizeevent.width;
        _screen_h = event->event.resizeevent.height;
    }
    else if ((event->type == GLEVENT_MOUSE) && (event->event.mouseevent.type == MOUSEEVENT_MOVE))
    {
        GuiWidget_show(_widget, FALSE);
        _caller = NULL;
        _time = 0;
        _delay = FALSE;
        return TRUE;
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime duration)
{
    (void)thread;
    
    if (_delay)
    {
        _time += duration;
        if (_time > 1000)
        {
            _delay = FALSE;
            _time = 0;
            GuiWidget_show(_widget, TRUE);
        }
        else if (_time > 500)
        {
            /* user didn't insist enough, cancelling tooltip */
            _caller = NULL;
            _time = 0;
            _delay = FALSE;
        }
    }
    else if (_caller != NULL)
    {
        _time += duration;
        if (_time > 1000)
        {
            GuiWidget_show(_widget, FALSE);
            _caller = NULL;
            _time = 0;
            _delay = FALSE;
        }
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
guitooltipInit()
{
    _screen_w = 10;
    _screen_h = 10;
    _caller = NULL;
    _widget = GuiWidget_new(NULL, NULL, NULL, FALSE, NULL, (GlEventCallback)downEventCallback);
    _time = 0;
    _delay = FALSE;
    GuiWidget_show(_widget, FALSE);
    
    MOD_ID = coreDeclareModule("tooltip", NULL, NULL, NULL, NULL, NULL, threadCallback);
    coreRequireThreadSlot(MOD_ID, coreGetThreadID("main"));
}

/*----------------------------------------------------------------------------*/
void
guitooltipUninit()
{
    GuiWidget_del(_widget);
    shellPrint(LEVEL_INFO, "Tooltip module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
guitooltipSet(String text, GlRect* rect, GuiWidget caller)
{
    GlRect rct;
    GuiTheme* theme;
    GlSurface surf;
    
    if (_caller != caller)
    {
        _caller = caller;
        
        theme = guithemeGetStd(GUITHEME_TOOLTIP);
        GlTextRender_guessSize(theme->textrender, text, &rct.w, &rct.h);
        rct.w += theme->borderleft + theme->borderright;
        rct.h += theme->bordertop + theme->borderbottom;

        /*best horizontal position*/
        rct.x = rect->x + (rect->w - rct.w) / 2;
        if (rct.x < 0)
        {
            rct.x = 0;
        }
        else if (rct.x + rct.w > _screen_w)
        {
            rct.x = _screen_w - rct.w;
        }
        
        /*best vertical position*/
        if (rect->y + rect->h + rct.h > _screen_h)
        {
            rct.y = rect->y - rct.h;
        }
        else
        {
            rct.y = rect->y + rect->h;
        }
        
        /*TODO: replace the GL2D_HIGHEST use by a real GUI altitude management*/
        GuiWidget_topLevelSet(_widget, GL2D_HIGHEST - 1, rct);
        surf = GuiWidget_getDrawingSurface(_widget);
        
        GlSurface_getRect(surf, &rct);
        GuiTexture_draw(theme->bgtex, surf, rct);
        GlTextRender_setWidthLimit(theme->textrender, 0);
        GlTextRender_directRender(theme->textrender, text, surf, rct, theme->borderleft, theme->bordertop);
        /*GuiWidget_show(_widget, TRUE);*/
        _time = 0;
        _delay = TRUE;
    }
    else
    {
        if (_delay)
        {
            if (_time > 250)
            {
                /* user insisted, validating tooltip */
                _time += 1000;
            }
        }
        else
        {
            _time = 0;
        }
    }
}
