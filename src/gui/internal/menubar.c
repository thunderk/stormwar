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
 *   Panel that stands on the left of the screen.                             *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/menubar.h"

#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

#include "gui/guitexture.h"
#include "gui/internal/guipopupmenu.h"
#include "graphics/gltextrender.h"
#include "graphics/gl2dobject.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static GuiTexture tex;

static GlSurface draw_surf;

static Gl2DObject globj;

static unsigned int nbmenu;
static String* menunames;
static String* menucoms;
static GlRect* menurcts;

static GlTextRender textrender;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static Bool
processEvent(GlExtID data, GlEvent* event)
{
    unsigned int i;
    Sint16 x;
    GuiPopupMenu menu;
    String s;
    
    (void)data; /*not used*/
    
    if (event->type == GLEVENT_MOUSE)
    {
        if (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
        {
            /*an entry may have been clicked*/
            x = event->event.mouseevent.x - Gl2DObject_getX(globj);
            for (i = 0; i < nbmenu; i++)
            {
                if ((x >= menurcts[i].x) && (x < menurcts[i].x + menurcts[i].w))
                {
                    /*we have a clicked entry*/
                    if (String_get(menucoms[i])[0] == '%')
                    {
                        /*open a menu*/
                        /*TODO: keep trace of child menu*/
                        s = String_new(String_get(menucoms[i]) + 1);
                        menu = GuiPopupMenu_new(s);
                        String_del(s);
                        GuiPopupMenu_setPos(menu, menurcts[i], MENU_BELOW);
                    }
                    else
                    {
                        /*execute a command*/
                        shellExec(menucoms[i]);
                    }
                }
            }
        }
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
menubarDraw()
{
    unsigned int i;
    GlRect rct;
    
    GlSurface_getRect(draw_surf, &rct);
    GuiTexture_draw(tex, draw_surf, rct);
    for (i = 0; i < nbmenu; i++)
    {
        GlTextRender_render(textrender, menunames[i], draw_surf, menurcts[i].x, menurcts[i].y, menurcts[i].h);
    }

    Gl2DObject_redraw(globj);
}

/*----------------------------------------------------------------------------*/
static void
menubarEmptyMenus()
{
    unsigned int i;
    
    if (nbmenu != 0)
    {
        for (i = 0; i < nbmenu; i++)
        {
            String_del(menunames[i]);
            String_del(menucoms[i]);
        }
        FREE(menunames);
        FREE(menucoms);
        FREE(menurcts);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Menubar functions                            #*
 *############################################################################*
 ******************************************************************************/
void
menubarInit()
{
    /*Arbitrary values*/
    tex = GuiTexture_new();
    draw_surf = GlSurface_new(10, 10, TRUE);
    globj = Gl2DObject_new(draw_surf, NULL, processEvent);
    textrender = GlTextRender_new();
    Gl2DObject_setAlt(globj, GL2D_LOWEST);
    shellPrint(LEVEL_INFO, "Menubar loaded.");
    nbmenu = 0;
    menunames = NULL;
    menucoms = NULL;
    menurcts = NULL;
}

/*----------------------------------------------------------------------------*/
void
menubarUninit()
{
    GuiTexture_del(tex);
    Gl2DObject_del(globj);
    GlSurface_del(draw_surf);
    GlTextRender_del(textrender);
    menubarEmptyMenus();
    shellPrint(LEVEL_INFO, "Menubar unloaded.");
}

/*----------------------------------------------------------------------------*/
Uint16
menubarSet(Var varset)
{
    VarValidator valid;
    Uint16 ret;
    Var entries;
    Var entry;
    Var name, com;
    String s_name, s_com;
    unsigned int i;

    valid = VarValidator_new();
    
    /*default values*/
    VarValidator_declareIntVar(valid, "height", 10);
    VarValidator_declareArrayVar(valid, "texture");
    VarValidator_declareStringVar(valid, "fontname", "");
    VarValidator_declareArrayVar(valid, "fontoptions");
    VarValidator_declareIntVar(valid, "startx", 0);
    VarValidator_declareIntVar(valid, "starty", 0);
    VarValidator_declareIntVar(valid, "entryspace", 10);
    VarValidator_declareArrayVar(valid, "entries");
    
    /*validate*/
    VarValidator_validate(valid, varset);
    VarValidator_del(valid);

    /*setting*/
    ret = Var_getValueInt(Var_getArrayElemByCName(varset, "height"));
    GuiTexture_set(tex, Var_getArrayElemByCName(varset, "texture"));
    
    GlTextRender_setFont(textrender, Var_getValueString(Var_getArrayElemByCName(varset, "fontname")));
    GlTextRender_setOptionsFromVar(textrender, Var_getArrayElemByCName(varset, "fontoptions"));
    
    entries = Var_getArrayElemByCName(varset, "entries");
    menubarEmptyMenus();
    nbmenu = Var_getArraySize(entries);
    s_name = String_new("name");
    s_com = String_new("com");
    if (nbmenu != 0)
    {
        menunames = MALLOC(sizeof(String) * nbmenu);
        menucoms = MALLOC(sizeof(String) * nbmenu);
        menurcts = MALLOC(sizeof(GlRect) * nbmenu);
        for (i = 0; i < nbmenu; i++)
        {
            /*TODO: improve this*/
            entry = Var_getArrayElemByPos(entries, i);
            if (Var_getType(entry) != VAR_ARRAY)
            {
                shellPrintf(LEVEL_ERROR, "Menubar entry incorrectly formatted : %s", Var_gets(entry));
                menunames[i] = String_new("");
                menucoms[i] = String_new("");
            }
            else
            {
                name = Var_getArrayElemByName(entry, s_name);
                com = Var_getArrayElemByName(entry, s_com);
                if ((name == NULL) || (Var_getType(name) != VAR_STRING) || (com == NULL) || (Var_getType(com) != VAR_STRING))
                {
                    shellPrintf(LEVEL_ERROR, "Menubar entry's 'name' or 'com' not found or incorrect : %s", Var_gets(entry));
                    menunames[i] = String_new("");
                    menucoms[i] = String_new("");
                }
                else
                {
                    menunames[i] = String_newByCopy(Var_getValueString(name));
                    menucoms[i] = String_newByCopy(Var_getValueString(com));
                }
            }
            if (i == 0)
            {
                menurcts[0].x = Var_getValueInt(Var_getArrayElemByCName(varset, "startx"));
                menurcts[0].y = Var_getValueInt(Var_getArrayElemByCName(varset, "starty"));
            }
            else
            {
                menurcts[i].x = menurcts[i - 1].x + menurcts[i - 1].w + Var_getValueInt(Var_getArrayElemByCName(varset, "entryspace"));
                menurcts[i].y = menurcts[i - 1].y;
            }
            GlTextRender_guessSize(textrender, menunames[i], &menurcts[i].w, &menurcts[i].h);
        }
    }
    
    /*free memory*/
    String_del(s_name);
    String_del(s_com);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
menubarSetSize(Uint16 w, Uint16 h)
{
    GlSurface_resize(draw_surf, w, h, SURFACE_UNDEFINED);
    Gl2DObject_relink(globj, draw_surf);
    menubarDraw();
}
