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
 *   Graphical scrollbars                                                     *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiscrollbar.h"

#include "gui/internal/guibutton.h"
#include "gui/internal/guitheme.h"

#include "graphics/color.h"
#include "graphics/glsurface.h"

#include "tools/fonct.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define CHILD_INCBUTTON 0
#define CHILD_DECBUTTON 1
#define CHILD_SLIDER 2
#define CHILD_NB 3

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GuiScrollbar
{
    GuiWidget widget;           /* Widget. */
    GuiScrollbarType type;      /* Orientation. */
    GuiTexture background;      /* Background texture. */
    GuiButton incbutton;        /* Button to increment the value. */
    GuiButton decbutton;        /* Button to decrement the value. */
    GuiButton slider;           /* Slider button. */
    GlRect sliderrect;          /* slider's rect. */
    Sint16 value;               /* Current value. */
    Sint16 min;                 /* Lower value. */
    Sint16 max;                 /* Upper value. */
    Uint16 smallinc;            /* Small increment. */
    Uint16 largeinc;            /* Large increment. */
    Uint16 length;              /* Virtual length. */
    Uint16 width;               /* Virtual width. */
    Sint16 fieldstart;          /* Start (included) of the graphical field of allowed values. */
    Sint16 fieldend;            /* End (included) of the graphical field of allowed values. */
    Uint16 fieldfull;           /* Full size of the field. */
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
GuiScrollbar_drawBackground(GuiScrollbar scrollbar)
{
    GlSurface surf;
    GlRect rct;
    
    surf = GuiWidget_getDrawingSurface(scrollbar->widget);
    GlSurface_getRect(surf, &rct);
    GuiTexture_draw(scrollbar->background, surf, rct);
}

/*----------------------------------------------------------------------------*/
static void
GuiScrollbar_valueChanged(GuiScrollbar scrollbar)
{
    Sint16 pos;

    if (scrollbar->max == scrollbar->min)
    {
        pos = scrollbar->fieldstart;
    }
    else
    {
        pos = scrollbar->fieldstart + (scrollbar->value - scrollbar->min) * (scrollbar->fieldend - scrollbar->fieldstart) / (scrollbar->max - scrollbar->min);
    }
    
    if (scrollbar->type == GUISCROLLBAR_HORIZONTAL)
    {
        scrollbar->sliderrect.x = pos;
        scrollbar->sliderrect.y = 0;
    }
    else if (scrollbar->type == GUISCROLLBAR_VERTICAL)
    {
        scrollbar->sliderrect.x = 0;
        scrollbar->sliderrect.y = pos;
    }
    else
    {
        scrollbar->sliderrect.x = 0;
        scrollbar->sliderrect.y = pos;
    }
    GuiWidget_setChildRect(scrollbar->widget, CHILD_SLIDER, scrollbar->sliderrect);
}

/*----------------------------------------------------------------------------*/
static void
GuiScrollbar_scaleChanged(GuiScrollbar scrollbar)
{
    Sint16 s;
    
    /*compute the new slider size*/
    s = scrollbar->max + 1 - scrollbar->min;    /*number of 'ticks'*/
    s = scrollbar->fieldfull - s;
    if (s < (scrollbar->width / 2) + 1)
    {
        s = (scrollbar->width / 2) + 1;
    }
    s = MIN(s, scrollbar->fieldfull);
    
    if (scrollbar->type == GUISCROLLBAR_HORIZONTAL)
    {
        scrollbar->sliderrect.w = s;
        scrollbar->sliderrect.h = scrollbar->width;
    }
    else
    {
        scrollbar->sliderrect.h = s;
        scrollbar->sliderrect.w = scrollbar->width;
    }
    
    /*adjust field limit*/
    scrollbar->fieldend = scrollbar->fieldstart + scrollbar->fieldfull - s;
    
    /*set the slider*/
    GuiScrollbar_valueChanged(scrollbar);
}

/*----------------------------------------------------------------------------*/
static void
GuiScrollbar_sizeChanged(GuiScrollbar scrollbar)
{
    Uint16 s1, s2;
    GlSurface surf;
    GlRect rect;
    
    surf = GuiWidget_getDrawingSurface(scrollbar->widget);
    
    /*new virtual size*/
    if (scrollbar->type == GUISCROLLBAR_HORIZONTAL)
    {
        scrollbar->length = GlSurface_getWidth(surf);
        scrollbar->width = GlSurface_getHeight(surf);
    }
    else
    {
        scrollbar->width = GlSurface_getWidth(surf);
        scrollbar->length = GlSurface_getHeight(surf);
    }
    
    /*redraw background*/
    GuiScrollbar_drawBackground(scrollbar);
    
    /*adjust buttons*/
    s1 = MIN(scrollbar->width, (scrollbar->length - 1) / 2);        /*we work with 'length-1' to let at least 1 pixel for the slider*/
    s2 = MIN((scrollbar->length - 1) - s1, scrollbar->width);
    if (scrollbar->type == GUISCROLLBAR_HORIZONTAL)
    {
        GlRect_MAKE(rect, 0, 0, s1, scrollbar->width);
        GuiWidget_setChildRect(scrollbar->widget, CHILD_DECBUTTON, rect);
        rect.x = scrollbar->length - s2;
        rect.w = s2;
        GuiWidget_setChildRect(scrollbar->widget, CHILD_INCBUTTON, rect);
    }
    else if (scrollbar->type == GUISCROLLBAR_VERTICAL)
    {
        GlRect_MAKE(rect, 0, 0, scrollbar->width, s1);
        GuiWidget_setChildRect(scrollbar->widget, CHILD_INCBUTTON, rect);
        rect.y = scrollbar->length - s2;
        rect.h = s2;
        GuiWidget_setChildRect(scrollbar->widget, CHILD_DECBUTTON, rect);
    }
    else /*scrollbar->type == GUISCROLLBAR_VERTICALINV*/
    {
        GlRect_MAKE(rect, 0, 0, scrollbar->width, s1);
        GuiWidget_setChildRect(scrollbar->widget, CHILD_DECBUTTON, rect);
        rect.y = scrollbar->length - s2;
        rect.h = s2;
        GuiWidget_setChildRect(scrollbar->widget, CHILD_INCBUTTON, rect);
    }
    
    /*adjust field and slider*/
    scrollbar->fieldstart = s1;
    scrollbar->fieldfull = scrollbar->length - s1 - s2;
    GuiScrollbar_scaleChanged(scrollbar);
}

/*----------------------------------------------------------------------------*/
static Bool
downEventCallback(GlExtID wid, GlEvent* event)
{
    GuiScrollbar scrollbar;
    GlSurface surf;
    GuiWidget widget = (GuiWidget)wid;
    
    scrollbar = (GuiScrollbar)GuiWidget_getHolder(widget);
    ASSERT(scrollbar != NULL, return FALSE);
    ASSERT(scrollbar->widget == widget, return FALSE);
    
    surf = GuiWidget_getDrawingSurface(widget);
    
    if (event->type == GLEVENT_RESIZE)
    {
        GuiScrollbar_sizeChanged(scrollbar);
    }
    else if (event->type == GLEVENT_KEYBOARD)
    {
        if ((scrollbar->type == GUISCROLLBAR_HORIZONTAL) | (scrollbar->type == GUISCROLLBAR_VERTICAL))
        {
            if ((event->event.keyevent.keyfunc == KEY_UP) | (event->event.keyevent.keyfunc == KEY_RIGHT))
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value + scrollbar->smallinc);
            }
            else if ((event->event.keyevent.keyfunc == KEY_DOWN) | (event->event.keyevent.keyfunc == KEY_LEFT))
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value - scrollbar->smallinc);
            }
            else if (event->event.keyevent.keyfunc == KEY_PAGEUP)
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value + scrollbar->largeinc);
            }
            else if (event->event.keyevent.keyfunc == KEY_PAGEDOWN)
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value - scrollbar->largeinc);
            }
        }
        else /*scrollbar->type == GUISCROLLBAR_VERTICALINV*/
        {
            if ((event->event.keyevent.keyfunc == KEY_DOWN) | (event->event.keyevent.keyfunc == KEY_RIGHT))
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value + scrollbar->smallinc);
            }
            else if ((event->event.keyevent.keyfunc == KEY_UP) | (event->event.keyevent.keyfunc == KEY_LEFT))
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value - scrollbar->smallinc);
            }
            else if (event->event.keyevent.keyfunc == KEY_PAGEDOWN)
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value + scrollbar->largeinc);
            }
            else if (event->event.keyevent.keyfunc == KEY_PAGEUP)
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value - scrollbar->largeinc);
            }
        }
    }
    else if ((event->type == GLEVENT_MOUSE) &&
                (((event->event.mouseevent.type == MOUSEEVENT_MOVE) && (event->event.mouseevent.button != MOUSEBUTTON_NONE))
              || (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)))
    {
        Sint16 val;
        Sint16 pos;
        
        if (scrollbar->fieldend == scrollbar->fieldstart)
        {
            /*field is too little to do things inside*/
            return FALSE;
        }
        
        if (scrollbar->type == GUISCROLLBAR_HORIZONTAL)
        {
            pos = event->event.mouseevent.x - scrollbar->fieldstart;
            pos = MAX(pos, 0);
            pos = MIN(pos, scrollbar->fieldend - scrollbar->fieldstart);
            val = pos * (scrollbar->max - scrollbar->min) / (scrollbar->fieldend - scrollbar->fieldstart) + scrollbar->min;
        }
        else if (scrollbar->type == GUISCROLLBAR_VERTICAL)
        {
            /*FIXME: this is the code for VERTICALINV*/
            pos = event->event.mouseevent.y - scrollbar->fieldstart;
            pos = MAX(pos, 0);
            pos = MIN(pos, scrollbar->fieldend - scrollbar->fieldstart);
            val = pos * (scrollbar->max - scrollbar->min) / (scrollbar->fieldend - scrollbar->fieldstart) + scrollbar->min;
        }
        else
        {
            pos = event->event.mouseevent.y - scrollbar->fieldstart;
            pos = MAX(pos, 0);
            pos = MIN(pos, scrollbar->fieldend - scrollbar->fieldstart);
            val = pos * (scrollbar->max - scrollbar->min) / (scrollbar->fieldend - scrollbar->fieldstart) + scrollbar->min;
        }
        
        GuiScrollbar_setValue(scrollbar, val);
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*/
void
upEventCallback(GuiWidget widget, unsigned int childnb, GuiEvent event)
{
    GuiScrollbar scrollbar;
    
    scrollbar = (GuiScrollbar)GuiWidget_getHolder(widget);
    ASSERT(scrollbar != NULL, return);
    ASSERT(scrollbar->widget == widget, return);
    
    switch (childnb)
    {
        case CHILD_INCBUTTON:
            if (event == GUIEVENT_CLICKED)
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value + scrollbar->smallinc);
            }
            break;
        case CHILD_DECBUTTON:
            if (event == GUIEVENT_CLICKED)
            {
                GuiScrollbar_setValue(scrollbar, scrollbar->value - scrollbar->smallinc);
            }
            break;
        default:
            ;
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GuiScrollbar
GuiScrollbar_new(GuiWidget parent, GuiScrollbarType type, const char* tooltip)
{
    GuiScrollbar ret;
    
    ret = (GuiScrollbar)MALLOC(sizeof(pv_GuiScrollbar));
    ret->widget = GuiWidget_new(parent, ret, tooltip, FALSE, upEventCallback, downEventCallback);
    ret->value = 0;
    ret->min = 0;
    ret->max = 1;
    ret->smallinc = 1;
    ret->largeinc = 1;
    ret->type = type;
    ret->background = guithemeGetStd(GUITHEME_SCROLLBAR)->bgtex;
    
    GuiWidget_setNbChild(ret->widget, CHILD_NB);
    ret->incbutton = GuiButton_new(ret->widget, 100, NULL);
    GuiWidget_setChild(ret->widget, CHILD_INCBUTTON, GuiButton_widget(ret->incbutton));
    ret->decbutton = GuiButton_new(ret->widget, 100, NULL);
    GuiWidget_setChild(ret->widget, CHILD_DECBUTTON, GuiButton_widget(ret->decbutton));
    ret->slider = GuiButton_new(ret->widget, 0, NULL);
    GuiWidget_setChild(ret->widget, CHILD_SLIDER, GuiButton_widget(ret->slider));

    GuiScrollbar_sizeChanged(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiScrollbar_del(GuiScrollbar scrollbar)
{
    GuiButton_del(scrollbar->incbutton);
    GuiButton_del(scrollbar->decbutton);
    GuiButton_del(scrollbar->slider);
    GuiWidget_del(scrollbar->widget);
    FREE(scrollbar);
}

/*----------------------------------------------------------------------------*/
GuiWidget
GuiScrollbar_widget(GuiScrollbar scrollbar)
{
    return scrollbar->widget;
}

/*----------------------------------------------------------------------------*/
void
GuiScrollbar_setLimits(GuiScrollbar scrollbar, Sint16 min, Sint16 max)
{
    scrollbar->min = min;
    scrollbar->max = (max < min) ? min : max;
    
    /*correct the slider and field*/
    GuiScrollbar_scaleChanged(scrollbar);
}

/*----------------------------------------------------------------------------*/
void
GuiScrollbar_setIncrements(GuiScrollbar scrollbar, Uint16 smallinc, Uint16 largeinc)
{
    scrollbar->smallinc = smallinc;
    scrollbar->largeinc = largeinc;
}

/*----------------------------------------------------------------------------*/
void
GuiScrollbar_setValue(GuiScrollbar scrollbar, Sint16 val)
{
    val = MAX(val, scrollbar->min);
    val = MIN(val, scrollbar->max);
    scrollbar->value = val;
    
    GuiScrollbar_valueChanged(scrollbar);
    
    /*throw event to the parent*/
    GuiWidget_throwEvent(scrollbar->widget, GUIEVENT_CHANGED);
}

/*----------------------------------------------------------------------------*/
Sint16
GuiScrollbar_getValue(GuiScrollbar scrollbar)
{
    return scrollbar->value;
}
