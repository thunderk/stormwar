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
 *   Graphical user interface management                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/gui.h"
#include "core/core.h"

#include "graphics/graphics.h"
#include "graphics/glrect.h"

#include "core/string.h"
#include "tools/varvalidator.h"

#include "gui/worldmap.h"
#include "gui/internal/guitheme.h"
#include "gui/internal/guitooltip.h"
#include "gui/internal/sidepanel.h"
#include "gui/internal/menubar.h"
#include "gui/internal/guipopupmenu.h"
#include "gui/internal/guishell.h"
#include "gui/internal/gamedialogs.h"
#include "gui/guidialog.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static GlRect rct_sidepanel;
static GlRect rct_menubar;
static Uint16 screen_w;
static Uint16 screen_h;

static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID FUNC_SHOWMSG = CORE_INVALID_ID;
static CoreID FUNC_ASKEXIT = CORE_INVALID_ID;

static PtrArray _dialogs_to_del;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
exitDialogCallback(GuiDialog dlg, GuiDialogButton button, Var vinput)
{
    (void)vinput;
    (void)dlg;
    if (button == GUIDIALOGBUTTON_YES)
    {
        coreStop();
    }
}

/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* funct)
{
    if (funct->id == FUNC_SHOWMSG)
    {
        GuiDialog_popupMsg(String_get(Var_getValueString(funct->params[0])));
    }
    if (funct->id == FUNC_ASKEXIT)
    {
        GuiDialog dia;
        
        dia = GuiDialog_new(GUIDIALOG_YESNO, exitDialogCallback);
        GuiDialog_addParagraph(dia, _("Really quit (current game will be lost) ?"));
        GuiDialog_launch(dia);
    }
    Var_setVoid(funct->ret);
}

/*----------------------------------------------------------------------------*/
static void
datasCallback(Var datas)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    
    VarValidator_declareArrayVar(valid, "menubar");
    VarValidator_declareArrayVar(valid, "sidepanel");
    VarValidator_declareArrayVar(valid, "shell");
    VarValidator_declareArrayVar(valid, "worldmap");
    VarValidator_declareArrayVar(valid, "menus");
    VarValidator_declareArrayVar(valid, "themes");
    
    VarValidator_validate(valid, datas);
    VarValidator_del(valid);
    
    guithemeSet(Var_getArrayElemByCName(datas, "themes"));
    
    rct_menubar.h = menubarSet(Var_getArrayElemByCName(datas, "menubar"));
    if (rct_menubar.h > 300)
    {
        shellPrint(LEVEL_INFO, "Menubar height request was too big, reduced to 300.");
        rct_menubar.h = 300;
    }
    menubarSetSize(rct_menubar.w, rct_menubar.h);
    
    rct_sidepanel.y = rct_menubar.h;
    rct_sidepanel.h = screen_h - rct_menubar.h;
    sidepanelSet(Var_getArrayElemByCName(datas, "sidepanel"));
    sidepanelSetHeight(rct_sidepanel.h);
    sidepanelSetYPos(rct_sidepanel.y);
    
    guishellSet(Var_getArrayElemByCName(datas, "shell"));
    worldmapSet(Var_getArrayElemByCName(datas, "worldmap"));
    menusSet(Var_getArrayElemByCName(datas, "menus"));
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime duration)
{
    (void)thread;
    (void)duration;
    
    /*delete the dialogs that wanted to be*/
    PtrArray_clear(_dialogs_to_del);
}

/*----------------------------------------------------------------------------*/
static Bool
eventCollector(GlExtID id, GlEvent* event)
{
    (void)id;
    
    if (event->type == GLEVENT_RESIZE)
    {
        screen_w = event->event.resizeevent.width;
        screen_h = event->event.resizeevent.height;
        
        rct_menubar.w = screen_w;
        rct_sidepanel.h = screen_h - rct_menubar.h;
        rct_sidepanel.y = rct_menubar.h;
        
        sidepanelSetHeight(rct_sidepanel.h);
        sidepanelSetYPos(rct_sidepanel.y);
        menubarSetSize(rct_menubar.w, rct_menubar.h);
        /*guishellScreenSizeChanged(w, h);*/
        /*worldmapScreenSizeChanged();*/
    }
    else if (event->type == GLEVENT_KEYBOARD)
    {
        if ((event->event.keyevent.type == KEYEVENT_RELEASED) && (event->event.keyevent.keyfunc == KEY_F1))
        {
            guishellToggle();
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
guiInit()
{
    _dialogs_to_del = PtrArray_newFull(5, 5, (PtrFunc)GuiDialog_del, NULL);
    guithemeInit();
    guiwidgetInit();
    guitooltipInit();
    guishellInit();
    menusInit();
    GlRect_MAKE(rct_menubar, 0, 0, 10, 10);
    GlRect_MAKE(rct_sidepanel, 0, 0, 10, 10);
    screen_w = 640;
    screen_h = 480;
    sidepanelInit();
    menubarInit();
    worldmapInit();
    gamedialogsInit();
    
    graphicsAddEventCollector(GLEVENT_RESIZE | GLEVENT_KEYBOARD, eventCollector);
    
    MOD_ID = coreDeclareModule("gui", NULL, datasCallback, shellCallback, NULL, NULL, threadCallback);
    FUNC_SHOWMSG = coreDeclareShellFunction(MOD_ID, "showmsg", VAR_VOID, 1, VAR_STRING);
    FUNC_ASKEXIT = coreDeclareShellFunction(MOD_ID, "askexit", VAR_VOID, 0);
    coreRequireThreadSlot(MOD_ID, coreGetThreadID(NULL));
}

/*----------------------------------------------------------------------------*/
void
guiUninit()
{
    PtrArray_del(_dialogs_to_del);
    
    worldmapUninit();
    menubarUninit();
    sidepanelUninit();
    menusUninit();
    guishellUninit();
    guitooltipUninit();
    guiwidgetUninit();
    guithemeUninit();
    shellPrint(LEVEL_INFO, "Interface module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
guiDelDialog(GuiDialog dia)
{
    PtrArray_append(_dialogs_to_del, (Ptr)dia);
}

/*----------------------------------------------------------------------------*/
void
GuiLayout_paramFromVar(GuiLayout* layout, Var vlay)
{
    VarValidator valid;
    int i;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "dock", GUIDOCK_TOPLEFT);
    VarValidator_declareIntVar(valid, "offsetx", 0);
    VarValidator_declareIntVar(valid, "offsety", 0);
    VarValidator_declareIntVar(valid, "layer", 0);
    
    VarValidator_validate(valid, vlay);
    VarValidator_del(valid);
    
    i = Var_getValueInt(Var_getArrayElemByCName(vlay, "dock"));
    if ((i < 1) || (i > 9))
    {
        shellPrint(LEVEL_ERROR, "Invalid screen docking (must be 1-9).");
        shellPrintf(LEVEL_ERROR, "   In '%s'.", Var_gets(vlay));
        i = GUIDOCK_TOPLEFT;
    }
    layout->dock = (GuiDock)i;
    
    layout->xoffset = Var_getValueInt(Var_getArrayElemByCName(vlay, "offsetx"));
    layout->yoffset = Var_getValueInt(Var_getArrayElemByCName(vlay, "offsety"));
    layout->layer = (GuiLayer)Var_getValueInt(Var_getArrayElemByCName(vlay, "layer"));
}

/*----------------------------------------------------------------------------*/
void
GuiLayout_update(GuiLayout* layout)
{
    ASSERT(layout->dock >= 1, return)
    ASSERT(layout->dock <= 9, return)
    
    switch (layout->dock)
    {
        case GUIDOCK_TOPLEFT:
            layout->rect.x = 0;
            layout->rect.y = 0;
            break;
        case GUIDOCK_TOP:
            layout->rect.x = (layout->parentrect.w - layout->rect.w) / 2;
            layout->rect.y = 0;
            break;
        case GUIDOCK_TOPRIGHT:
            layout->rect.x = layout->parentrect.w - layout->rect.w;
            layout->rect.y = 0;
            break;
        case GUIDOCK_LEFT:
            layout->rect.x = 0;
            layout->rect.y = (layout->parentrect.h - layout->rect.h) / 2;
            break;
        case GUIDOCK_CENTER:
            layout->rect.x = (layout->parentrect.w - layout->rect.w) / 2;
            layout->rect.y = (layout->parentrect.h - layout->rect.h) / 2;
            break;
        case GUIDOCK_RIGHT:
            layout->rect.x = layout->parentrect.w - layout->rect.w;
            layout->rect.y = (layout->parentrect.h - layout->rect.h) / 2;
            break;
        case GUIDOCK_BOTTOMLEFT:
            layout->rect.x = 0;
            layout->rect.y = layout->parentrect.h - layout->rect.h;
            break;
        case GUIDOCK_BOTTOM:
            layout->rect.x = (layout->parentrect.w - layout->rect.w) / 2;
            layout->rect.y = layout->parentrect.h - layout->rect.h;
            break;
        case GUIDOCK_BOTTOMRIGHT:
            layout->rect.x = layout->parentrect.w - layout->rect.w;
            layout->rect.y = layout->parentrect.h - layout->rect.h;
            break;
    }
    layout->rect.x += layout->xoffset + layout->parentrect.x;
    layout->rect.y += layout->yoffset + layout->parentrect.y;
}
