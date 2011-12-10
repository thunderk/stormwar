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
 *   Object for menus that popup on the screen and menus manager              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guipopupmenu.h"

#include "gui/internal/guitheme.h"

#include "core/shell.h"
#include "core/string.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

#include "graphics/graphics.h"
#include "graphics/color.h"
#include "graphics/gl2dobject.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/* An item's command is the command executed on mouse click.
 * If the command starts with a '%', then the following is considered to be
 * a path to a submenu file. */
typedef struct
{
    String text;        /*!< Item's text */
    String com;         /*!< Item's command */
    GlRect rct;         /*!< Item's rct inside the menu's drawing surface */
} MenuItem;

typedef struct
{
    String name;                /*!< Template name */
    GlSurface surf;             /*!< Buffer surface */
    unsigned int nbitem;        /*!< Number of MenuItem */
    MenuItem* items;            /*!< Items contained by the menu */
} pv_MenuTemplate;

typedef pv_MenuTemplate* MenuTemplate;

struct pv_GuiPopupMenu
{
    MenuTemplate template;          /*!< Menu template */
    GlRect parent;                  /*!< Rectangle of the menu's creator */
    MenuLayout layout;              /*!< Relative placement from the parent */
    Gl2DObject globj;               /*!< Object inserted into the graphical engine */
    void* child;                    /*!< Submenu or NULL if not */
    unsigned int highlighted;       /*!< Currently highlighted item */
};

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static PtrArray _menutemplates;
static PtrArray _menus;
static GlEventCollector _collector;  /*event collector*/
static Sint16 _nextz;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
MenuTemplate_createSurface(MenuTemplate menu, GuiTheme* theme)
{
    Uint16 w, h;
    GlRect r;
    unsigned int i;
    
    w = 0;
    h = 0;
    
    r.x = theme->borderleft;
    r.y = theme->bordertop;
    
    GlTextRender_setWidthLimit(theme->textrender, 0);
    for (i = 0; i < menu->nbitem; i++)
    {
        GlTextRender_guessSize(theme->textrender, menu->items[i].text, &r.w, &r.h);
        menu->items[i].rct = r;
        r.y += r.h;
        w = MAX(r.w, w);
        h += r.h;
    }
    
    /*items width must be the whole width, so we correct*/
    for (i = 0; i < menu->nbitem; i++)
    {
        menu->items[i].rct.w = w;
    }

    w += theme->borderleft + theme->borderright;
    h += theme->bordertop + theme->borderbottom;

    menu->surf = GlSurface_new(MAX(w, 1), MAX(h, 1), TRUE);
}

/*----------------------------------------------------------------------------*/
static void
MenuTemplate_draw(MenuTemplate menu, GuiTheme* theme)
{
    unsigned int i;
    GlRect rct;

    GlSurface_getRect(menu->surf, &rct);
    GuiTexture_draw(theme->bgtex, menu->surf, rct);
    
    if (menu->nbitem != 0)
    {
        GlTextRender_setWidthLimit(theme->textrender, menu->items[0].rct.w);
        for (i = 0; i < menu->nbitem; i++)
        {
            GlTextRender_render(theme->textrender, menu->items[i].text, menu->surf, menu->items[i].rct.x, menu->items[i].rct.y, menu->items[i].rct.h);
        }
    }
}

/*----------------------------------------------------------------------------*/
static MenuTemplate
MenuTemplate_new(Var v)
{
    MenuTemplate ret;
    VarValidator valid;
    Var entries;
    Var entry;
    Var name;
    Var com;
    GuiTheme* theme;
    unsigned int i;
    
    valid = VarValidator_new();
    
    VarValidator_declareStringVar(valid, "name", "");
    VarValidator_declareArrayVar(valid, "entries");
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    ret = (MenuTemplate)MALLOC(sizeof(pv_MenuTemplate));
    
    ret->name = String_newByCopy(Var_getValueString(Var_getArrayElemByCName(v, "name")));
    
    shellPrintf(LEVEL_ERRORSTACK, "For menu named '%s'.", String_get(ret->name));
    
    /*fill items*/
    entries = Var_getArrayElemByCName(v, "entries");
    ret->nbitem = Var_getArraySize(entries);
    ret->items = MALLOC(sizeof(MenuItem) * ret->nbitem);
    /*TODO: improve this with validators*/
    for (i = 0; i < ret->nbitem; i++)
    {
        entry = Var_getArrayElemByPos(entries, i);
        if (Var_getType(entry) != VAR_ARRAY)
        {
            shellPrintf(LEVEL_ERROR, "Menu entry incorrectly formatted : %s", Var_gets(entry));
            ret->items[i].text = String_new("");
            ret->items[i].com = String_new("");
        }
        else
        {
            name = Var_getArrayElemByCName(entry, "name");
            com = Var_getArrayElemByCName(entry, "com");
            if ((name == NULL) || (Var_getType(name) != VAR_STRING) || (com == NULL) || (Var_getType(com) != VAR_STRING))
            {
                shellPrintf(LEVEL_ERROR, "Menu entry's 'name' or 'com' not found or incorrect : %s", Var_gets(entry));
                ret->items[i].text = String_new("");
                ret->items[i].com = String_new("");
            }
            else
            {
                ret->items[i].text = String_newByCopy(Var_getValueString(name));
                ret->items[i].com = String_newByCopy(Var_getValueString(com));
            }
        }
    }
    
    /*draw the menu template*/
    theme = guithemeGetStd(GUITHEME_MENU);
    MenuTemplate_createSurface(ret, theme);
    MenuTemplate_draw(ret, theme);
    
    shellPopErrorStack();
    
    return ret;
}

/*----------------------------------------------------------------------------*/
static void
MenuTemplate_del(Ptr menu_)
{
    MenuTemplate menu = (MenuTemplate)menu_;
    unsigned int i;
    
    GlSurface_del(menu->surf);
    String_del(menu->name);
    if (menu->nbitem != 0)
    {
        for (i = 0; i < menu->nbitem; i++)
        {
            String_del(menu->items[i].com);
            String_del(menu->items[i].text);
        }
        FREE(menu->items);
    }
    FREE(menu);
}

/*----------------------------------------------------------------------------*/
static int
MenuTemplate_cmp(Ptr* menu1, Ptr* menu2)
{
    return String_cmp(&(((MenuTemplate)(*menu1))->name), &(((MenuTemplate)(*menu2))->name));
}

/*----------------------------------------------------------------------------*/
/*!
 * \brief Choose better position for a menu.
 *
 * Menu sizes must be set before calling this.
 * \param menu - The menu
 */
static void
computePos(GuiPopupMenu menu)
{
    Sint16 x = 0, y = 0;
    
    switch (menu->layout)
    {
        case MENU_RIGHT:
            x = menu->parent.x + menu->parent.w;
            y = menu->parent.y;
            break;
        case MENU_BELOW:
            x = menu->parent.x;
            y = menu->parent.y + menu->parent.h;
            break;
    }
    Gl2DObject_setPos(menu->globj, x, y, 0);
}

/*----------------------------------------------------------------------------*/
static void
destroyAllMenus()
{
    if (PtrArray_SIZE(_menus) > 0)
    {
        graphicsDelEventCollector(_collector);
    }
    PtrArray_clear(_menus);
}

/*----------------------------------------------------------------------------*/
static unsigned int
findEntry(MenuTemplate menu, Sint16 x, Sint16 y)
{
    unsigned int i;
    
    for (i = 0; i < menu->nbitem; i++)
    {
        if (isInRect(&menu->items[i].rct, x, y))
        {
            return i;
        }
    }
    return menu->nbitem;
}

/*----------------------------------------------------------------------------*/
static Bool
GuiPopupMenu_processEvent(GlExtID data, GlEvent* event)
{
    Sint16 x, y;
    unsigned int i;
    String s;
    GuiPopupMenu menu;
    GuiPopupMenu submenu;
    GlRect rct;
    
    if (data == _collector)
    {
        if (event->type == GLEVENT_MOUSE)
        {
            if (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
            {
                destroyAllMenus();
            }
        }
    }
    else
    {
        menu = (GuiPopupMenu)data;
    
        switch (event->type)
        {
            case GLEVENT_DELETE:
                FREE(menu);
                return TRUE;
            
            case GLEVENT_MOUSE:
                switch (event->event.mouseevent.type)
                {
                    case MOUSEEVENT_BUTTONPRESSED:
                        break;
                    case MOUSEEVENT_BUTTONRELEASED:
                        x = event->event.mouseevent.x - Gl2DObject_getX(menu->globj);
                        y = event->event.mouseevent.y - Gl2DObject_getY(menu->globj);
                        if ((i = findEntry(menu->template, x, y)) == menu->template->nbitem)
                        {
                            /*mouse released outside the menu*/
                            destroyAllMenus();
                        }
                        else
                        {
                            if (String_get(menu->template->items[i].com)[0] != '%')
                            {
                                /*an item has been clicked*/
                                s = String_newByCopy(menu->template->items[i].com);
                                destroyAllMenus();
                                shellExec(s);
                                String_del(s);
                            }
                        }
                        break;
                    case MOUSEEVENT_MOVE:
                        x = event->event.mouseevent.x - Gl2DObject_getX(menu->globj);
                        y = event->event.mouseevent.y - Gl2DObject_getY(menu->globj);
                        if ((i = findEntry(menu->template, x, y)) == menu->template->nbitem)
                        {
                            menu->highlighted = menu->template->nbitem;
                        }
                        else
                        {
                            if (i != menu->highlighted)
                            {
                                if (menu->child != NULL)
                                {
                                    GuiPopupMenu_del(menu->child);
                                    menu->child = NULL;
                                }
                                menu->highlighted = i;
                                if (String_get(menu->template->items[i].com)[0] == '%')
                                {
                                    /*open a submenu*/
                                    s = String_new(String_get(menu->template->items[i].com) + 1);
                                    submenu = GuiPopupMenu_new(s);
                                    String_del(s);
                                    GlRect_MAKE(rct, Gl2DObject_getX(menu->globj) + menu->template->items[i].rct.x, Gl2DObject_getY(menu->globj) + menu->template->items[i].rct.y, menu->template->items[i].rct.w, menu->template->items[i].rct.h);
                                    GuiPopupMenu_setPos(submenu, rct, MENU_RIGHT);
                                    menu->child = submenu;
                                }
                            }
                        }
                        break;
                    default:
                        ;
                }
                return TRUE;
                break;
            default:
                ;
        }
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
GuiPopupMenu_delFinal(GuiPopupMenu menu)
{
    if (menu->globj != NULL)
    {
        Gl2DObject_del(menu->globj);
        _nextz--;
    }
    else
    {
        FREE(menu);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
menusInit()
{
    _menus = PtrArray_newFull(5, 3, (PtrFunc)GuiPopupMenu_delFinal, NULL);
    _menutemplates = PtrArray_newFull(2, 3, MenuTemplate_del, MenuTemplate_cmp);
    _nextz = 2 * (GL2D_LOWEST + GL2D_HIGHEST) / 3;
    shellPrint(LEVEL_INFO, "Menus manager loaded.");
}

/*----------------------------------------------------------------------------*/
void
menusUninit()
{
    destroyAllMenus();
    PtrArray_del(_menus);
    PtrArray_del(_menutemplates);
    shellPrint(LEVEL_INFO, "Menus manager unloaded.");
}

/*----------------------------------------------------------------------------*/
void
menusSet(Var v)
{
    int i;
    
    PtrArray_clear(_menutemplates);
    
    ASSERT(Var_getType(v) == VAR_ARRAY, return);
    
    for (i = 0; i < Var_getArraySize(v); i++)
    {
        PtrArray_insertSorted(_menutemplates, MenuTemplate_new(Var_getArrayElemByPos(v, i)));
    }
}

/*----------------------------------------------------------------------------*/
GuiPopupMenu
GuiPopupMenu_new(String name)
{
    GuiPopupMenu ret;
    pv_MenuTemplate tempseek;
    Ptr* template;
    GlColorRGBA col;
    
    ret = (GuiPopupMenu)MALLOC(sizeof(pv_GuiPopupMenu));
    
    tempseek.name = name;
    template = PtrArray_findSorted(_menutemplates, &tempseek);
    if (template == NULL)
    {
        ret->template = NULL;
    }
    else
    {
        ret->template = *((MenuTemplate*)template);
    }
    
    GlRect_MAKE(ret->parent, 0, 0, 10, 10);
    ret->layout = MENU_RIGHT;
    if (ret->template == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Menu template '%s' not found.", String_get(name));
        ret->globj = NULL;
    }
    else
    {
        ret->globj = Gl2DObject_new(ret->template->surf, ret, GuiPopupMenu_processEvent);
        GlColorRGBA_MAKE(col, 0xFF, 0xFF, 0xFF, 0x00);
        Gl2DObject_setColor(ret->globj, col, 0);
        col.a = 0xFF;
        Gl2DObject_setColor(ret->globj, col, 250);
        Gl2DObject_setAlt(ret->globj, _nextz++);
    }
    ret->child = NULL;
    ret->highlighted = 0;
    
    if (PtrArray_SIZE(_menus) == 0)
    {
        _collector = graphicsAddEventCollector(GLEVENT_MOUSE, GuiPopupMenu_processEvent);
    }
    
    PtrArray_append(_menus, ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiPopupMenu_del(GuiPopupMenu menu)
{
    if (menu->child != NULL)
    {
        GuiPopupMenu_del(menu->child);
        menu->child = NULL;
    }
    
    PtrArray_removeFast(_menus, menu);
    
    if (PtrArray_SIZE(_menus) == 0)
    {
        graphicsDelEventCollector(_collector);
    }
}

/*----------------------------------------------------------------------------*/
void
GuiPopupMenu_setPos(GuiPopupMenu menu, GlRect parent, MenuLayout layout)
{
    menu->parent = parent;
    menu->layout = layout;
    if (menu->globj != NULL)
    {
        computePos(menu);
    }
}
