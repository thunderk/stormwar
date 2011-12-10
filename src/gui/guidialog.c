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
 *   Graphical dialog template                                                *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/gui.h"
#include "gui/guidialog.h"
#include "gui/internal/guiwidget.h"
#include "gui/internal/guitheme.h"
#include "gui/internal/guibutton.h"
#include "gui/internal/guiinput.h"

#include "core/string.h"
#include "core/var.h"

#include "graphics/graphics.h"
#include "graphics/glrect.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define BUTTONS_SPACE 5
#define BUTTONS_NB 5
#define VERTICAL_SPACE 2

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef enum
{
    GUIDIALOGCHILD_BUTTON,
    GUIDIALOGCHILD_PARAGRAPH,
    GUIDIALOGCHILD_INPUT_TEXT
} GuiDialogChildType;

typedef struct
{
    GuiDialogChildType  type;
    GlRect              rct;
    void*               child;
    
    union {
        GuiDialogButton         button;
        String                  paragraph;
        struct {
            String name;
            String label;
            Uint16 xoffset;
            void* gui_obj;
        }                       inputfield;
    } data;
} GuiDialogChild;

struct pv_GuiDialog
{
    Bool launched;
    GuiWidget widget;
    GuiDialogCallback callback;
    unsigned int nbchildren;
    unsigned int nbbuttons;
    GuiDialogChild* children;
    GlRect rct;
    /*Uint16 alt;*/
    Sint16 grabx;
    Sint16 graby;
};

static Gl2DCoord _startx = 50;
static Gl2DCoord _starty = 50;
static Gl2DCoord _maxx = 50;
static Gl2DCoord _maxy = 50;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static Bool
downEventCallback(GlExtID widget, GlEvent* event)
{
    GuiDialog dialog;
    unsigned int i;
    
    dialog = (GuiDialog)GuiWidget_getHolder(widget);
    ASSERT(dialog != NULL, return FALSE);
    ASSERT(dialog->widget == widget, return FALSE);

    if (event->type == GLEVENT_DELETE)
    {
        for (i = 0; i < dialog->nbchildren; i++)
        {
            /*TODO: maybe some of those things could have been deleted at launch*/
            switch (dialog->children[i].type)
            {
                case GUIDIALOGCHILD_BUTTON:
                    GuiButton_del((GuiButton)dialog->children[i].child);
                    break;
    
                case GUIDIALOGCHILD_PARAGRAPH:
                    String_del(dialog->children[i].data.paragraph);
                    break;
    
                case GUIDIALOGCHILD_INPUT_TEXT:
                    String_del(dialog->children[i].data.inputfield.name);
                    String_del(dialog->children[i].data.inputfield.label);
                    GuiInput_del((GuiInput)dialog->children[i].data.inputfield.gui_obj);
                    break;
            }
        }
    
        FREE(dialog->children);
        FREE(dialog);
    }
    else if (event->type == GLEVENT_MOUSE)
    {
        if (event->event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
        {
            dialog->grabx = event->event.mouseevent.x;
            dialog->graby = event->event.mouseevent.y;
            return TRUE;
        }
        if ((event->event.mouseevent.type == MOUSEEVENT_MOVE) && (event->event.mouseevent.button == MOUSEBUTTON_LEFT))
        {
            dialog->rct.x += (event->event.mouseevent.x - dialog->grabx);
            dialog->rct.y += (event->event.mouseevent.y - dialog->graby);
            GuiWidget_topLevelSet(widget, -1, dialog->rct);
            return TRUE;
        }
    }
    else if (event->type == GLEVENT_RESIZE)
    {
        if (dialog->rct.x + dialog->rct.w > event->event.resizeevent.width)
        {
            dialog->rct.x = event->event.resizeevent.width - dialog->rct.w;
            GuiWidget_topLevelSet(widget, -1, dialog->rct);
        }
        if (dialog->rct.x < 0)
        {
            dialog->rct.x = 0;
            GuiWidget_topLevelSet(widget, -1, dialog->rct);
        }
        if (dialog->rct.y + dialog->rct.h > event->event.resizeevent.height)
        {
            dialog->rct.y = event->event.resizeevent.height - dialog->rct.h;
            GuiWidget_topLevelSet(widget, -1, dialog->rct);
        }
        if (dialog->rct.y < 0)
        {
            dialog->rct.y = 0;
            GuiWidget_topLevelSet(widget, -1, dialog->rct);
        }
        _maxx = event->event.resizeevent.width - 100;
        _maxy = event->event.resizeevent.height - 100;
        if (_startx > _maxx)
        {
            _startx = 50;
        }
        if (_starty > _maxy)
        {
            _starty = 50;
        }
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
upEventCallback(GuiWidget widget, unsigned int childnb, GuiEvent event)
{
    GuiDialog dialog;
    Var v, vf;
    unsigned int i;
    
    dialog = (GuiDialog)GuiWidget_getHolder(widget);
    ASSERT(dialog != NULL, return);
    ASSERT(dialog->widget == widget, return);
    
    if ((childnb < dialog->nbbuttons) && (event == GUIEVENT_CLICKED))
    {
        ASSERT(dialog->children[childnb].type == GUIDIALOGCHILD_BUTTON, return);
        
        /*a button has been clicked*/
        
        /*collect input fields*/
        v = Var_new();
        Var_setArray(v);
        for (i = dialog->nbbuttons; i < dialog->nbchildren; i++)
        {
            switch (dialog->children[i].type)
            {
                case GUIDIALOGCHILD_BUTTON:
                case GUIDIALOGCHILD_PARAGRAPH:
                    break;
                case GUIDIALOGCHILD_INPUT_TEXT:
                    vf = Var_new();
                    Var_setName(vf, String_get(dialog->children[i].data.inputfield.name));
                    Var_setString(vf, GuiInput_getText((GuiInput)dialog->children[i].data.inputfield.gui_obj));
                    Var_addToArray(v, vf);
                    break;
            }
        }
        
        /*raise the callback*/
        if (dialog->callback != NULL)
        {
            dialog->callback(dialog, dialog->children[childnb].data.button, v);
        }
        
        /*delete the dialog*/
        Var_del(v);
        guiDelDialog(dialog);
    }
}

/*----------------------------------------------------------------------------*/
static void
createButtons(GuiDialog dialog)
{
    unsigned int i;
    GuiButton button;
    String s;
    
    s = String_new("");
    
    GuiWidget_setNbChild(dialog->widget, dialog->nbbuttons);
    for (i = 0; i < dialog->nbbuttons; i++)
    {
        button = GuiButton_new(dialog->widget, 0, NULL);
        GuiWidget_setChild(dialog->widget, i, GuiButton_widget(button));
        dialog->children[i].child = (void*)button;
        switch (dialog->children[i].data.button)
        {
            case GUIDIALOGBUTTON_OK:
                String_replace(s, _("OK"));
                GuiButton_setCaption(button, s);
                break;
            case GUIDIALOGBUTTON_CANCEL:
                String_replace(s, _("Cancel"));
                GuiButton_setCaption(button, s);
                break;
            case GUIDIALOGBUTTON_YES:
                String_replace(s, _("Yes"));
                GuiButton_setCaption(button, s);
                break;
            case GUIDIALOGBUTTON_NO:
                String_replace(s, _("No"));
                GuiButton_setCaption(button, s);
                break;
            case GUIDIALOGBUTTON_RETRY:
                String_replace(s, _("Retry"));
                GuiButton_setCaption(button, s);
                break;
        }
    }

    String_del(s);
}

/******************************************************************************
 *############################################################################*
 *#                             Static functions                             #*
 *############################################################################*
 ******************************************************************************/
void
GuiDialog_popupMsg(const char* text)
{
    GuiDialog dia;
    
    dia = GuiDialog_new(GUIDIALOG_OK, NULL);
    GuiDialog_addParagraph(dia, text);
    GuiDialog_launch(dia);
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GuiDialog
GuiDialog_new(GuiDialogType type, GuiDialogCallback callback)
{
    GuiDialog ret;
    
    ret = MALLOC(sizeof(pv_GuiDialog));
    ret->launched = FALSE;
    ret->widget = GuiWidget_new(NULL, ret, NULL, FALSE, upEventCallback, downEventCallback);
    GuiWidget_show(ret->widget, FALSE);
    ret->callback = callback;
    /*ret->alt = 5;*/
    ret->grabx = 0;
    ret->graby = 0;
    GlRect_MAKE(ret->rct, 0, 0, 10, 10);
    
    /*define the buttons*/
    switch (type)
    {
        case GUIDIALOG_OK:
            ret->nbchildren = ret->nbbuttons = 1;
            ret->children = MALLOC(sizeof(GuiDialogChild) * ret->nbbuttons);
        
            ret->children[0].type = GUIDIALOGCHILD_BUTTON;
            ret->children[0].data.button = GUIDIALOGBUTTON_OK;
            break;
        
        case GUIDIALOG_OKCANCEL:
            ret->nbchildren = ret->nbbuttons = 2;
            ret->children = MALLOC(sizeof(GuiDialogChild) * ret->nbbuttons);
        
            ret->children[0].type = GUIDIALOGCHILD_BUTTON;
            ret->children[0].data.button = GUIDIALOGBUTTON_OK;
        
            ret->children[1].type = GUIDIALOGCHILD_BUTTON;
            ret->children[1].data.button = GUIDIALOGBUTTON_CANCEL;
            break;
        
        case GUIDIALOG_YESNO:
            ret->nbchildren = ret->nbbuttons = 2;
            ret->children = MALLOC(sizeof(GuiDialogChild) * ret->nbbuttons);
        
            ret->children[0].type = GUIDIALOGCHILD_BUTTON;
            ret->children[0].data.button = GUIDIALOGBUTTON_YES;
        
            ret->children[1].type = GUIDIALOGCHILD_BUTTON;
            ret->children[1].data.button = GUIDIALOGBUTTON_NO;
            break;
        
        case GUIDIALOG_YESNOCANCEL:
            ret->nbchildren = ret->nbbuttons = 3;
            ret->children = MALLOC(sizeof(GuiDialogChild) * ret->nbbuttons);
        
            ret->children[0].type = GUIDIALOGCHILD_BUTTON;
            ret->children[0].data.button = GUIDIALOGBUTTON_YES;
        
            ret->children[1].type = GUIDIALOGCHILD_BUTTON;
            ret->children[1].data.button = GUIDIALOGBUTTON_NO;
        
            ret->children[2].type = GUIDIALOGCHILD_BUTTON;
            ret->children[2].data.button = GUIDIALOGBUTTON_CANCEL;
            break;
        
        case GUIDIALOG_RETRYCANCEL:
            ret->nbchildren = ret->nbbuttons = 2;
            ret->children = MALLOC(sizeof(GuiDialogChild) * ret->nbbuttons);
        
            ret->children[0].type = GUIDIALOGCHILD_BUTTON;
            ret->children[0].data.button = GUIDIALOGBUTTON_RETRY;
        
            ret->children[1].type = GUIDIALOGCHILD_BUTTON;
            ret->children[1].data.button = GUIDIALOGBUTTON_CANCEL;
            break;
        
        default:
            /*should not happen*/
            error("GuiDialog", "GuiDialog_new", "Invalid dialog type.");
            ret->nbbuttons = ret->nbchildren = 0;
    }
    
    createButtons(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiDialog_del(GuiDialog dialog)
{
    GuiWidget_del(dialog->widget);
}

/*----------------------------------------------------------------------------*/
void
GuiDialog_launch(GuiDialog dialog)
{
    unsigned int i;
    Uint16 w, h, hbuttons;
    Sint16 x, y;
    GuiTheme* theme;
    GlSurface surf;
    GlRect rct;
    
    if (dialog->launched)
    {
        shellPrint(LEVEL_ERROR, "Try to double-launch a dialog.");
        return;
    }
    dialog->launched = TRUE;
    
    theme = guithemeGetStd(GUITHEME_DIALOG);
    
    w = BUTTONS_SPACE * (dialog->nbbuttons - 1);
    h = 0;
    hbuttons = 0;
    
    /*compute arbitrary children rects*/
    for (i = 0; i < dialog->nbchildren; i++)
    {
        switch (dialog->children[i].type)
        {
            case GUIDIALOGCHILD_BUTTON:
                GuiButton_requiredSize((GuiButton)dialog->children[i].child, &dialog->children[i].rct);
                w += dialog->children[i].rct.w;
                hbuttons = h = MAX(h, dialog->children[i].rct.h);
                break;

            case GUIDIALOGCHILD_PARAGRAPH:
                GlTextRender_setWidthLimit(theme->textrender, 0);
                GlTextRender_guessSize(theme->textrender, dialog->children[i].data.paragraph, &dialog->children[i].rct.w, &dialog->children[i].rct.h);
                w = MAX(w, dialog->children[i].rct.w);
                h += dialog->children[i].rct.h + VERTICAL_SPACE;
                break;
            
            case GUIDIALOGCHILD_INPUT_TEXT:
                GlTextRender_setWidthLimit(theme->textrender, 0);
                GlTextRender_guessSize(theme->textrender, dialog->children[i].data.inputfield.label, &dialog->children[i].rct.w, &dialog->children[i].rct.h);
                dialog->children[i].rct.h = imax(GuiInput_getHeight((GuiInput)dialog->children[i].data.inputfield.gui_obj), dialog->children[i].rct.h);
                w = imax(w, dialog->children[i].rct.w + 100);        /*TODO: get the 100 value from GuiInput*/
                h += dialog->children[i].rct.h + VERTICAL_SPACE;
                break;
        }
    }
    
    /*adjust widget*/
    dialog->rct.w = w + theme->borderleft + theme->borderright;
    dialog->rct.h = h + theme->bordertop + theme->borderbottom;
    dialog->rct.x = _startx;
    dialog->rct.y = _starty;
    _startx += 20;
    if (_startx > _maxx)
    {
        _startx = 50;
    }
    _starty += 20;
    if (_starty > _maxy)
    {
        _starty = 50;
    }
    GuiWidget_topLevelSet(dialog->widget, -1, dialog->rct);

    /*adjust children rectangles to final position*/
    x = theme->borderleft;
    y = dialog->rct.h - theme->borderbottom - hbuttons;
    for (i = 0; i < dialog->nbbuttons; i++)
    {
        dialog->children[i].rct.x = x;
        dialog->children[i].rct.y = y;
        x += dialog->children[i].rct.w + BUTTONS_SPACE;
    }
    x = theme->borderleft;
    y = theme->bordertop;
    for (i = dialog->nbbuttons; i < dialog->nbchildren; i++)
    {
        dialog->children[i].rct.x = x;
        dialog->children[i].rct.y = y;
        y += dialog->children[i].rct.h + VERTICAL_SPACE;
    }
    
    /*render background*/
    surf = GuiWidget_getDrawingSurface(dialog->widget);
    GlSurface_getRect(surf, &rct);
    GuiTexture_draw(theme->bgtex, surf, rct);
    
    /*render objects and adjust children*/
    for (i = 0; i < dialog->nbchildren; i++)
    {
        switch (dialog->children[i].type)
        {
            case GUIDIALOGCHILD_BUTTON:
                GuiWidget_setChildRect(dialog->widget, i, dialog->children[i].rct);
                break;
            case GUIDIALOGCHILD_PARAGRAPH:
                GlTextRender_directRender(theme->textrender, dialog->children[i].data.paragraph, GuiWidget_getDrawingSurface(dialog->widget), dialog->children[i].rct, 0, 0);
                break;
            case GUIDIALOGCHILD_INPUT_TEXT:
                GlTextRender_directRender(theme->textrender, dialog->children[i].data.inputfield.label, GuiWidget_getDrawingSurface(dialog->widget), dialog->children[i].rct, 0, 0);
                dialog->children[i].rct.x += dialog->children[i].rct.w;
                dialog->children[i].rct.w = w - dialog->children[i].rct.w;
                GuiWidget_setChildRect(dialog->widget, i, dialog->children[i].rct);
                break;
        }
    }

    GuiWidget_show(dialog->widget, TRUE);
}

/*----------------------------------------------------------------------------*/
void
GuiDialog_addParagraph(GuiDialog dialog, const char* text)
{
    if (dialog->launched)
    {
        shellPrint(LEVEL_ERROR, "Try to add a paragraph to a launched dialog.");
        return;
    }
    
    dialog->nbchildren++;
    dialog->children = REALLOC(dialog->children, sizeof(GuiDialogChild) * dialog->nbchildren);
    dialog->children[dialog->nbchildren - 1].type = GUIDIALOGCHILD_PARAGRAPH;
    dialog->children[dialog->nbchildren - 1].data.paragraph = String_new(text);
}

/*----------------------------------------------------------------------------*/
void
GuiDialog_addTextInput(GuiDialog dialog, const char* name, const char* label, const char* tooltip, const char* defaulttext)
{
    GuiInput input;
    
    if (dialog->launched)
    {
        shellPrint(LEVEL_ERROR, "Try to add a text input to a launched dialog.");
        return;
    }
    
    dialog->nbchildren++;
    dialog->children = REALLOC(dialog->children, sizeof(GuiDialogChild) * dialog->nbchildren);
    dialog->children[dialog->nbchildren - 1].type = GUIDIALOGCHILD_INPUT_TEXT;
    dialog->children[dialog->nbchildren - 1].data.inputfield.name = String_new(name);
    dialog->children[dialog->nbchildren - 1].data.inputfield.label = String_new(label);
    
    /*create the input box*/
    input = GuiInput_new(dialog->widget, 0, tooltip);
    GuiWidget_setNbChild(dialog->widget, dialog->nbchildren);
    GuiWidget_setChild(dialog->widget, dialog->nbchildren - 1, GuiInput_widget(input));
    GuiInput_setText(input, defaulttext);
    dialog->children[dialog->nbchildren - 1].data.inputfield.gui_obj = (void*)input;
}
