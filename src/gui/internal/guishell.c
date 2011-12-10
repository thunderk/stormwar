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
 *   Graphical interface for shell input/output                               *
 *                                                                            *
  ***************************************************************************/


/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guishell.h"

#include "gui/gui.h"
#include "gui/internal/guiwidget.h"
#include "gui/internal/guiinput.h"
#include "gui/internal/guioutput.h"
#include "core/core.h"

#include "graphics/glsurface.h"
#include "graphics/gl2dobject.h"
#include "graphics/graphics.h"
#include "graphics/color.h"
#include "graphics/glrect.h"
#include "graphics/gltextrender.h"

#include "core/string.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define CHILD_INPUT 0
#define CHILD_OUTPUT 1

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static GuiLayout layout;
static GuiWidget _widget;
static GlRect _rect;
static GlRect rct_output;
static GlRect rct_input;
static GuiOutput output;
static GuiInput input;
static Bool vis;
static GlColorRGBA colors[LEVEL_NB];

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static Bool
downEventCallback(void* widget, GlEvent* event)
{
    ASSERT(widget == _widget, return FALSE);
    (void)widget;
    
    if (event->type == GLEVENT_RESIZE)
    {
        layout.parentrect.w = event->event.resizeevent.width;
        layout.parentrect.h = event->event.resizeevent.height;
        GuiLayout_update(&layout);
        GuiWidget_topLevelSet(_widget, layout.layer, layout.rect);
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
upEventCallback(GuiWidget widget, unsigned int childnb, GuiEvent event)
{
    ASSERT(widget == _widget, return);
    (void)widget;
    
    if (childnb == CHILD_INPUT)
    {
        if (event == GUIEVENT_VALIDATE)
        {
            shellPrintf(LEVEL_USER, "$ %s", String_get(GuiInput_getText(input)));
            shellExec(GuiInput_getText(input));
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
shellPrintCb(ShellLevel level, const char* st)
{
    String s;
    
    if (output != NULL)
    {
        s = String_new(st);
        GuiOutput_setFontColor(output, colors[level]);
        GuiOutput_print(output, s);
        
        String_del(s);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
guishellInit()
{
    layout.dock = GUIDOCK_TOPLEFT;
    layout.xoffset = 0;
    layout.yoffset = 0;
    layout.layer = 0;
    layout.parentrect.x = 0;
    layout.parentrect.y = 0;
    _widget = GuiWidget_new(NULL, NULL, NULL, FALSE, upEventCallback, downEventCallback);
    GlSurface_getRect(GuiWidget_getDrawingSurface(_widget), &_rect);
    GuiWidget_setNbChild(_widget, 2);
    GlRect_MAKE(rct_output, 0, 0, 10, 10);
    GlRect_MAKE(rct_input, 0, 0, 10, 10);

    guishellShow(FALSE);
    input = NULL;
    output = NULL;
    
    shellSetPrintCallback(shellPrintCb);
    shellPrint(LEVEL_INFO, "Graphical shell loaded.");
}

/*----------------------------------------------------------------------------*/
void
guishellUninit()
{
    GuiWidget_del(_widget);
    if (output != NULL)
    {
        GuiOutput_del(output);
    }
    if (input != NULL)
    {
        GuiInput_del(input);
    }
    
    shellSetPrintCallback(NULL);
    shellPrint(LEVEL_INFO, "Graphical shell unloaded.");
}

/*----------------------------------------------------------------------------*/
void
guishellSet(Var varset)
{
    VarValidator valid;
    GlSurface newsurf;
    
    valid = VarValidator_new();
    
    VarValidator_declareArrayVar(valid, "screen_layout");
    VarValidator_declareStringVar(valid, "bg", "");
    VarValidator_declareArrayVar(valid, "output");
    VarValidator_declareArrayVar(valid, "rect_output");
    VarValidator_declareArrayVar(valid, "rect_input");
    VarValidator_declareArrayVar(valid, "shellcolor_user");
    VarValidator_declareArrayVar(valid, "shellcolor_error");
    VarValidator_declareArrayVar(valid, "shellcolor_info");
    VarValidator_declareArrayVar(valid, "shellcolor_debug");
    VarValidator_declareArrayVar(valid, "shellcolor_harddebug");
    
    VarValidator_validate(valid, varset);
    VarValidator_del(valid);

    newsurf = GlSurface_newFromFile(Var_getValueString(Var_getArrayElemByCName(varset, "bg")));
    _rect.w = GlSurface_getWidth(newsurf);
    _rect.h = GlSurface_getHeight(newsurf);
    GuiLayout_paramFromVar(&layout, Var_getArrayElemByCName(varset, "screen_layout"));
    layout.rect.w = _rect.w;
    layout.rect.h = _rect.h;
    GuiWidget_topLevelSet(_widget, 2, _rect);
    GlSurface_copyRect(newsurf, GuiWidget_getDrawingSurface(_widget), NULL, NULL);
    GuiWidget_redraw(_widget);
    GlSurface_del(newsurf);

    /*input area*/
    if (input != NULL)
    {
        GuiInput_del(input);
    }
    input = GuiInput_new(_widget, 50, _("Enter your command here"));
    GuiWidget_setChild(_widget, CHILD_INPUT, GuiInput_widget(input));
    GuiInput_setCompletionList(input, coreGetCompletionList());
    GlRect_makeFromVar(&rct_input, Var_getArrayElemByCName(varset, "rect_input"));
    GlRect_clip(&rct_input, &_rect);
    GuiWidget_setChildRect(_widget, CHILD_INPUT, rct_input);

    /*output area*/
    if (output == NULL)
    {
        output = GuiOutput_new(_widget, 1000);
        GuiWidget_setChild(_widget, CHILD_OUTPUT, GuiOutput_widget(output));
    }
    GuiOutput_set(output, Var_getArrayElemByCName(varset, "output"));
    GlRect_makeFromVar(&rct_output, Var_getArrayElemByCName(varset, "rect_output"));
    GlRect_clip(&rct_output, &_rect);
    GuiWidget_setChildRect(_widget, CHILD_OUTPUT, rct_output);

    /*colors*/
    GlColorRGBA_makeFromVar(colors + LEVEL_USER, Var_getArrayElemByCName(varset, "shellcolor_user"));
    GlColorRGBA_makeFromVar(colors + LEVEL_ERROR, Var_getArrayElemByCName(varset, "shellcolor_error"));
    GlColorRGBA_makeFromVar(colors + LEVEL_INFO, Var_getArrayElemByCName(varset, "shellcolor_info"));
    GlColorRGBA_makeFromVar(colors + LEVEL_DEBUG, Var_getArrayElemByCName(varset, "shellcolor_debug"));
    GlColorRGBA_makeFromVar(colors + LEVEL_HARDDEBUG, Var_getArrayElemByCName(varset, "shellcolor_harddebug"));
}

/*----------------------------------------------------------------------------*/
void
guishellShow(Bool show)
{
    GuiWidget_show(_widget, show);
    vis = show;
}

/*----------------------------------------------------------------------------*/
void
guishellToggle()
{
    guishellShow(!vis);
}
