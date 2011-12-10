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
 *   Object for text output                                                   *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guioutput.h"

#include "tools/fonct.h"
#include "graphics/color.h"
#include "graphics/glsurface.h"
#include "graphics/gltextrender.h"
#include "gui/gui.h"
#include "gui/internal/guiwidget.h"
#include "gui/internal/guiscrollbar.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GuiOutput
{
    GuiWidget widget;           /*!< Widget. */
    GlSurface surf;             /*!< Buffer surface. */
    GlColor clearcolor;         /*!< Background color. */
    GlTextRender textrender;    /*!< Text renderer. */
    GlRect srcrect;             /*!< Source rectangle (in the buffer). */
    GlRect destrect;            /*!< Destination rectangle (in the widget). */
    Uint16 scrollwidth;         /*!< Width of the scrollbar. */
    GuiScrollbar scroll;        /*!< Scrollbar, NULL if not. */
};

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define CHILD_NB 1
#define CHILD_SCROLLBAR 0

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
GuiOutput_clear(GuiOutput output)
{
    GlSurface_clear(output->surf, output->clearcolor);
}

/*----------------------------------------------------------------------------*/
static void
GuiOutput_redraw(GuiOutput output)
{
    /*copy the buffer*/
    GlSurface_copyRect(output->surf, GuiWidget_getDrawingSurface(output->widget), &output->srcrect, &output->destrect);
    
    /*redraw the widget*/
    GuiWidget_redraw(output->widget);
}

/*----------------------------------------------------------------------------*/
static Bool
downEventCallback(GlExtID widg, GlEvent* event)
{
    GuiOutput output;
    GuiWidget widget = (GuiWidget)widg;
    GlRect scrollrect;
    
    output = (GuiOutput)GuiWidget_getHolder(widget);
    ASSERT(output != NULL, return FALSE);
    ASSERT(output->widget == widget, return FALSE);
    
    if (event->type == GLEVENT_RESIZE)
    {
        /*set scrollbar rect*/
        scrollrect.w = MIN(event->event.resizeevent.width - 1, output->scrollwidth);
        scrollrect.x = event->event.resizeevent.width - scrollrect.w;
        scrollrect.y = 0;
        scrollrect.h = event->event.resizeevent.height;
        GuiWidget_setChildRect(output->widget, CHILD_SCROLLBAR, scrollrect);
        
        output->srcrect.w = output->destrect.w = event->event.resizeevent.width - scrollrect.w;
        GlSurface_resize(output->surf, output->srcrect.w, GlSurface_getHeight(output->surf), SURFACE_KEEP);
        output->srcrect.h = output->destrect.h = imin(event->event.resizeevent.height, GlSurface_getHeight(output->surf));

        output->srcrect.y = GlSurface_getHeight(output->surf) - output->srcrect.h;
        
        GlTextRender_setWidthLimit(output->textrender, output->srcrect.w);
        
        /*correct scrollbar parameters*/
        if (GlSurface_getHeight(output->surf) <= output->destrect.h)
        {
            GuiScrollbar_setLimits(output->scroll, 0, 0);
        }
        else
        {
            GuiScrollbar_setLimits(output->scroll, 0, GlSurface_getHeight(output->surf) - output->destrect.h);
            GuiScrollbar_setValue(output->scroll, GlSurface_getHeight(output->surf) - output->destrect.h);
        }
        
        GuiOutput_clear(output);
        GuiOutput_redraw(output);
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
upEventCallback(GuiWidget widget, unsigned int childnb, GuiEvent event)
{
    GuiOutput output;
    
    output = (GuiOutput)GuiWidget_getHolder(widget);
    ASSERT(output != NULL, return);
    ASSERT(output->widget == widget, return);

    if ((childnb == CHILD_SCROLLBAR) & (event == GUIEVENT_CHANGED))
    {
        /*scrollbar's value has changed*/
        ASSERT(GuiScrollbar_getValue(output->scroll) >= 0, return);
        ASSERT(GuiScrollbar_getValue(output->scroll) <= GlSurface_getHeight(output->surf) - output->destrect.h, return);
        output->srcrect.y = GuiScrollbar_getValue(output->scroll);
        GuiOutput_redraw(output);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GuiOutput
GuiOutput_new(GuiWidget parent, Uint16 height)
{
    GuiOutput ret;
    
    ret = (GuiOutput)MALLOC(sizeof(pv_GuiOutput));
    ret->widget = GuiWidget_new(parent, ret, NULL, FALSE, upEventCallback, downEventCallback);
    ret->srcrect.w = ret->destrect.w = GlSurface_getWidth(GuiWidget_getDrawingSurface(ret->widget));
    ret->srcrect.x = ret->destrect.x = 0;
    ret->srcrect.y = ret->destrect.y = 0;
    ret->srcrect.h = ret->destrect.h = imin(height, GlSurface_getHeight(GuiWidget_getDrawingSurface(ret->widget)));
    
    ret->surf = GlSurface_new(ret->srcrect.w, height, TRUE);
    ret->clearcolor = 0x00000000;
    ret->textrender = GlTextRender_new();
    
    ret->scroll = GuiScrollbar_new(ret->widget, GUISCROLLBAR_VERTICALINV, NULL);
    ret->scrollwidth = 1;
    GuiWidget_setNbChild(ret->widget, CHILD_NB);
    GuiWidget_setChild(ret->widget, CHILD_SCROLLBAR, GuiScrollbar_widget(ret->scroll));
    
    /*TODO: create a scrollbar only if the buffer is taller than the widget*/
    /*TODO: set scrollbar parameters*/
    
    GuiOutput_clear(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiOutput_del(GuiOutput output)
{
    GlSurface_del(output->surf);
    GlTextRender_del(output->textrender);
    GuiWidget_del(output->widget);
    GuiScrollbar_del(output->scroll);
    
    FREE(output);
}

/*----------------------------------------------------------------------------*/
GuiWidget
GuiOutput_widget(GuiOutput output)
{
    return output->widget;
}

/*----------------------------------------------------------------------------*/
void
GuiOutput_set(GuiOutput output, Var v)
{
    VarValidator valid;
    GlColorRGBA col;
    
    /*validate*/
    valid = VarValidator_new();
    VarValidator_declareArrayVar(valid, "bgcolor");
    VarValidator_declareStringVar(valid, "font", "");
    VarValidator_declareArrayVar(valid, "fontopt");
    VarValidator_declareIntVar(valid, "scrollbarwidth", 1);
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    /*set clearcolor*/
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(v, "bgcolor"));
    output->clearcolor = GlColorRGBA_to_GlColor(col);
    
    /*set text renderer*/
    GlTextRender_setFont(output->textrender, Var_getValueString(Var_getArrayElemByCName(v, "font")));
    GlTextRender_setOptionsFromVar(output->textrender, Var_getArrayElemByCName(v, "fontopt"));
    
    /*set scrollbar parameters*/
    output->scrollwidth = imax(Var_getValueInt(Var_getArrayElemByCName(v, "scrollbarwidth")), 1);
    GuiScrollbar_setIncrements(output->scroll, GlTextRender_getLineHeight(output->textrender), GlTextRender_getLineHeight(output->textrender) * 5);
    /*TODO: throw the realloc event to the child widget*/
    
    GuiOutput_clear(output);
    GuiOutput_redraw(output);
}

/*----------------------------------------------------------------------------*/
void
GuiOutput_setFontColor(GuiOutput output, GlColorRGBA color)
{
    GlTextRender_setColor(output->textrender, GlColorRGBA_to_GlColor(color));
}

/*----------------------------------------------------------------------------*/
void
GuiOutput_print(GuiOutput output, String s)
{
    Sint16 h;
    GlRect rct1, rct2;
    GlTextRenderStatus status;
    
    h = GlTextRender_getLineHeight(output->textrender);
    if (h > 0)
    {
        rct1.x = rct2.x = 0;
        rct1.y = h;
        if (h >= GlSurface_getHeight(output->surf))
        {
            rct1.h = 0;
            rct2.h = GlSurface_getHeight(output->surf);
            rct2.y = 0;
        }
        else
        {
            rct1.h = GlSurface_getHeight(output->surf) - h;
            rct2.h = h;
            rct2.y = GlSurface_getHeight(output->surf) - h;
        }
        rct1.w = rct2.w = GlSurface_getWidth(output->surf);
        
        status.breakevent = GLTEXTRENDER_NEWLINE;
        do
        {
            /*handle new lines and carriage returns*/
            if (status.breakevent == GLTEXTRENDER_NEWLINE)
            {
                if (rct1.h == 0)
                {
                    GuiOutput_clear(output);
                }
                else
                {
                    GlSurface_movePart(output->surf, rct1, 0, -h);
                }
                GlSurface_drawRect(output->surf, rct2, output->clearcolor);
            }
            else if (status.breakevent == GLTEXTRENDER_RETURN)
            {
                GlSurface_drawRect(output->surf, rct2, output->clearcolor);
            }
            
            /*render a line*/
            status = GlTextRender_render(output->textrender, s, output->surf, rct2.x, rct2.y, rct2.h);
        } while (status.breakevent != GLTEXTRENDER_END);
        
        GuiOutput_redraw(output);
    }
}
