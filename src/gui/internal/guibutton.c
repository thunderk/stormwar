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
 *   Buttons for the GUI                                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guibutton.h"

#include "core/string.h"
#include "gui/internal/guitheme.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GuiButton
{
    GuiWidget widget;           /* Widget. */
    GlSurface drawn_normal;     /* Rendered picture for normal state. */
    GlSurface drawn_pushed;     /* Rendered picture for pushed state. */
    String caption;             /* Text content. */
    Uint16 repeatdelay;         /* Delay for repeating. */
    Uint32 repeattime;          /* Base time for repeating. */
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
GuiButton_draw(GuiButton button)
{
    GuiTheme* theme;
    GlSurface surf;
    GlRect rct;
    
    /*render normal state*/
    theme = guithemeGetStd(GUITHEME_BUTTON);
    surf = button->drawn_normal;
    GlSurface_getRect(surf, &rct);
    GuiTexture_draw(theme->bgtex, surf, rct);
    GlTextRender_directRender(theme->textrender, button->caption, surf, rct, theme->borderleft, theme->bordertop);
    
    /*render pushed state*/
    theme = guithemeGetStd(GUITHEME_BUTTONPUSHED);
    surf = button->drawn_pushed;
    GlSurface_getRect(surf, &rct);
    GuiTexture_draw(theme->bgtex, surf, rct);
    GlTextRender_directRender(theme->textrender, button->caption, surf, rct, theme->borderleft, theme->bordertop);

    /*draw the widget*/
    /*TODO: check that we can assume normal state*/
    surf = GuiWidget_getDrawingSurface(button->widget);
    GlSurface_copyRect(button->drawn_normal, surf, NULL, NULL);
    GuiWidget_redraw(button->widget);
}

/*----------------------------------------------------------------------------*/
static Bool
downEventCallback(GuiWidget widget, GlEvent* event)
{
    GuiButton button;
    GlRect rct;
    
    button = (GuiButton)GuiWidget_getHolder(widget);
    ASSERT(button != NULL, return FALSE);
    ASSERT(button->widget == widget, return FALSE);
    
    if (event->type == GLEVENT_RESIZE)
    {
        /*resize rendered picture*/
        GlSurface_resize(button->drawn_normal, event->event.resizeevent.width, event->event.resizeevent.height, SURFACE_UNDEFINED);
        GlSurface_resize(button->drawn_pushed, event->event.resizeevent.width, event->event.resizeevent.height, SURFACE_UNDEFINED);
        
        /*redraw all*/
        GuiButton_draw(button);
    }
    else if (event->type == GLEVENT_KEYBOARD)
    {
        if ((event->event.keyevent.keychar == '\n') | (event->event.keyevent.keychar == ' '))
        {
            GuiWidget_throwEvent(widget, GUIEVENT_CLICKED);
        }
    }
    else if (event->type == GLEVENT_MOUSE)
    {
        if (event->event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
        {
            GlSurface_copyRect(button->drawn_pushed, GuiWidget_getDrawingSurface(widget), NULL, NULL);
            GuiWidget_redraw(widget);
            
            if (button->repeatdelay != 0)
            {
                button->repeattime = getTicks();
                GuiWidget_throwEvent(widget, GUIEVENT_CLICKED);
            }
        }
        else if (event->event.mouseevent.type == MOUSEEVENT_MOVE)
        {
            Uint32 newtime = getTicks();
            
            GlSurface_getRect(GuiWidget_getDrawingSurface(widget), &rct);
            if ((button->repeatdelay != 0)
             && (button->repeattime != 0)
             && (newtime - button->repeattime > button->repeatdelay)
             && isInRect(&rct, event->event.mouseevent.x, event->event.mouseevent.y))
            {
                button->repeattime = newtime;
                GuiWidget_throwEvent(widget, GUIEVENT_CLICKED);
            }
        }
        else if (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
        {
            GlSurface_copyRect(button->drawn_normal, GuiWidget_getDrawingSurface(widget), NULL, NULL);
            GuiWidget_redraw(widget);

            GlSurface_getRect(GuiWidget_getDrawingSurface(widget), &rct);
            if (isInRect(&rct, event->event.mouseevent.x, event->event.mouseevent.y))
            {
                button->repeattime = 0;
                if (button->repeatdelay == 0)
                {
                    /*only when it was not repeating*/
                    /*FIXME: still, it must happen at least once, because fast clicking could be ignored.*/
                    GuiWidget_throwEvent(widget, GUIEVENT_CLICKED);
                }
            }
        }
    }
    
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GuiButton
GuiButton_new(GuiWidget parent, Uint16 clickrepeat, const char* tooltip)
{
    GuiButton ret;
    
    ret = (GuiButton)MALLOC(sizeof(pv_GuiButton));
    ret->widget = GuiWidget_new(parent, ret, tooltip, FALSE, NULL, (GlEventCallback)downEventCallback);
    ret->drawn_normal = GlSurface_newByCopy(GuiWidget_getDrawingSurface(ret->widget));
    ret->drawn_pushed = GlSurface_newByCopy(GuiWidget_getDrawingSurface(ret->widget));
    ret->caption = String_new("");
    ret->repeatdelay = clickrepeat;
    ret->repeattime = 0;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiButton_del(GuiButton button)
{
    String_del(button->caption);
    GuiWidget_del(button->widget);
    GlSurface_del(button->drawn_normal);
    GlSurface_del(button->drawn_pushed);
    FREE(button);
}

/*----------------------------------------------------------------------------*/
GuiWidget
GuiButton_widget(GuiButton button)
{
    return button->widget;
}

/*----------------------------------------------------------------------------*/
void
GuiButton_requiredSize(GuiButton button, GlRect* rct)
{
    GuiTheme* theme;
    
    theme = guithemeGetStd(GUITHEME_BUTTON);
    GlTextRender_guessSize(theme->textrender, button->caption, &rct->w, &rct->h);
    rct->w += theme->borderleft + theme->borderright;
    rct->h += theme->bordertop + theme->borderbottom;
}

/*----------------------------------------------------------------------------*/
void
GuiButton_setCaption(GuiButton button, String caption)
{
    String_replace(button->caption, String_get(caption));
    GuiButton_draw(button);
}
