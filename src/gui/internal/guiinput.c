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
 *   Object for user's text input                                             *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiinput.h"

#include "graphics/glrect.h"
#include "graphics/glsurface.h"
#include "gui/internal/guitheme.h"

#include "core/string.h"

#include <ctype.h>
#include <string.h>     /*for strncmp, TODO: check*/

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GuiInput
{
    GuiWidget widget;           /*!< Widget. */
    GlSurface bg;               /*!< Background picture. */
    unsigned int historysize;   /*!< Size of the history. */
    unsigned int historypos;    /*!< Current position in the history. */
    String* history;            /*!< History of the input, history[historypos] is the current input text. */
    CompletionList completion;  /*!< Auto-completion list, NULL if auto-completion is disabled. */
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
GuiInput_render(GuiInput input)
{
    GlSurface surf;
    GuiTheme* theme;
    GlRect rect;
    
    theme = guithemeGetStd(GUITHEME_INPUT);
    surf = GuiWidget_getDrawingSurface(input->widget);
    GlSurface_copyRect(input->bg, surf, NULL, NULL);
    GlSurface_getRect(surf, &rect);
    GlRect_cropBorders(&rect, theme->borderleft, theme->borderright, theme->bordertop, theme->borderbottom);
    GlTextRender_directRender(theme->textrender, input->history[input->historypos], surf, rect, 0, 0);
    
    GuiWidget_redraw(input->widget);
}

/*----------------------------------------------------------------------------*/
static Bool
downEventCallback(GuiWidget widget, GlEvent* event)
{
    String s;
    unsigned int i;
    Bool change = FALSE;
    GuiInput input;
    
    char* st;
    CompletionResults results;
    
    input = (GuiInput)GuiWidget_getHolder(widget);
    ASSERT(input != NULL, return FALSE);
    ASSERT(input->widget == widget, return FALSE);
    
    if (event->type == GLEVENT_RESIZE)
    {
        GlRect rct;
        
        GlSurface_resize(input->bg, GlSurface_getWidth(GuiWidget_getDrawingSurface(widget)), GlSurface_getHeight(GuiWidget_getDrawingSurface(widget)), SURFACE_UNDEFINED);
        GlSurface_getRect(input->bg, &rct);
        GuiTexture_draw(guithemeGetStd(GUITHEME_INPUT)->bgtex, input->bg, rct);
        
        GuiInput_render(input);
    }
    else if ((event->type == GLEVENT_KEYBOARD) && (event->event.keyevent.type == KEYEVENT_PRESSED))
    {
        if ((event->event.keyevent.keychar == '\n') || (event->event.keyevent.keychar == '\r'))
        {
            /*FIXME: an event up should be the last thing done*/
            GuiWidget_throwEvent(widget, GUIEVENT_VALIDATE);
            if (input->historysize > 1)
            {
                if (input->historypos > 0)
                {
                    /*TODO: this should always be done when the string already exists in the history*/
                    /*move the history entry to be the last one*/
                    s = input->history[input->historypos];
                    for (i = input->historypos; i > 1; i--)
                    {
                        input->history[i] = input->history[i - 1];
                    }
                    input->history[1] = s;
                }
                else
                {
                    if (input->history[input->historysize - 1] != NULL)
                    {
                        String_del(input->history[input->historysize - 1]);
                    }
                    /*move all history entries to make room for the new one*/
                    for (i = input->historysize - 1; i > 1; i--)
                    {
                        input->history[i] = input->history[i - 1];
                    }
                    input->history[1] = String_newByCopy(input->history[0]);
                }
            }
            input->historypos = 0;
            String_replace(input->history[0], "");
            change = TRUE;
        }
        else if (event->event.keyevent.keychar == '\b')
        {
            if (input->historypos > 0)
            {
                String_replace(input->history[0], String_get(input->history[input->historypos]));
                input->historypos = 0;
            }
            if (String_getLength(input->history[0]) != 0)
            {
                String_erase(input->history[0], String_getLength(input->history[0]) - 1, 1);
            }
            change = TRUE;
        }
        else if ((event->event.keyevent.keychar == '\t') && (input->completion != NULL))
        {
            /*auto-completion*/
            if (input->historypos > 0)
            {
                String_replace(input->history[0], String_get(input->history[input->historypos]));
                input->historypos = 0;
            }
            
            /*we find the beginning in the area*/
            st = String_get(input->history[0]);
            i = String_getLength(input->history[0]);
            /*TODO: maybe make a 'isname' function*/
            while ((i > 0) && (isalpha(st[i - 1]) || isdigit(st[i - 1]) || st[i - 1] == '_'))
            {
                i--;
            }
            if (i != String_getLength(input->history[0]))
            {   /*...else the last character is a control one*/
                st += i;
            }
            i = String_getLength(input->history[0]) - i;    /*length of the beginning*/
            
            /*get completion results*/
            results = CompletionResults_new();
            CompletionList_query(input->completion, st, results);
            s = CompletionResults_getNextResult(results, FALSE);
            if (s != NULL)
            {
                /*complete the input*/
                String_append(input->history[0], String_get(s) + i);
                change = TRUE;
            }
            CompletionResults_del(results);
        }
        else if (event->event.keyevent.keyfunc == KEY_UP)
        {
            /*history up*/
            if ((input->historypos < input->historysize - 1) && (input->history[input->historypos + 1] != NULL))
            {
                input->historypos++;
                change = TRUE;
            }
        }
        else if (event->event.keyevent.keyfunc == KEY_DOWN)
        {
            /*history down*/
            if (input->historypos > 0)
            {
                input->historypos--;
                change = TRUE;
            }
        }
        else if (event->event.keyevent.keychar >= 0x20)
        {
            if (input->historypos > 0)
            {
                String_replace(input->history[0], String_get(input->history[input->historypos]));
                input->historypos = 0;
            }
            String_appendChar(input->history[0], event->event.keyevent.keychar);
            change = TRUE;
        }
    }
    if (change)
    {
        /*FIXME: an event up should be the last thing done*/
        GuiWidget_throwEvent(input->widget, GUIEVENT_CHANGED);
        GuiInput_render(input);
    }
    return change;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GuiInput
GuiInput_new(GuiWidget parent, unsigned int history, const char* tooltip)
{
    GuiInput ret;
    unsigned int i;
    
    ret = (GuiInput)MALLOC(sizeof(pv_GuiInput));
    ret->widget = GuiWidget_new(parent, ret, tooltip, FALSE, NULL, (GlEventCallback)downEventCallback);
    ret->historysize = history + 1;
    ret->historypos = 0;
    ret->history = MALLOC(sizeof(String) * ret->historysize);
    ret->history[0] = String_new("");
    ret->completion = NULL;
    ret->bg = GlSurface_new(10, 10, TRUE);
    for (i = 1; i < ret->historysize; i++)
    {
        ret->history[i] = NULL;
    }
    
    /*GuiInput_render(ret);*/
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiInput_del(GuiInput input)
{
    unsigned int i;
    
    GuiWidget_del(input->widget);
    for (i = 0; i < input->historysize; i++)
    {
        if (input->history[i] != NULL)
        {
            String_del(input->history[i]);
        }
    }
    GlSurface_del(input->bg);
    FREE(input->history);
    FREE(input);
}

/*----------------------------------------------------------------------------*/
GuiWidget
GuiInput_widget(GuiInput input)
{
    return input->widget;
}

/*----------------------------------------------------------------------------*/
Uint16
GuiInput_getHeight(GuiInput input)
{
    (void)input;
    return GlTextRender_getLineHeight(guithemeGetStd(GUITHEME_INPUT)->textrender);
}

/*----------------------------------------------------------------------------*/
void
GuiInput_setText(GuiInput input, const char* text)
{
    input->historypos = 0;
    String_replace(input->history[0], text);
    GuiInput_render(input);
    
    GuiWidget_throwEvent(input->widget, GUIEVENT_CHANGED);
}

/*----------------------------------------------------------------------------*/
String
GuiInput_getText(GuiInput input)
{
    return input->history[input->historypos];
}

/*----------------------------------------------------------------------------*/
void
GuiInput_setCompletionList(GuiInput input, CompletionList completion)
{
    input->completion = completion;
}
