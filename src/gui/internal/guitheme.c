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
 *   Graphical theme manager                                                  *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guitheme.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define GUITHEME_NBSTD 7

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static GuiTheme _stdthemes[GUITHEME_NBSTD];

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
GuiTheme_new(GuiTheme* theme)
{
    theme->bgtex = GuiTexture_new();
    theme->textrender = GlTextRender_new();
    theme->borderleft = 0;
    theme->borderright = 0;
    theme->bordertop = 0;
    theme->borderbottom = 0;
}

/*----------------------------------------------------------------------------*/
static void
GuiTheme_del(GuiTheme* theme)
{
    GuiTexture_del(theme->bgtex);
    GlTextRender_del(theme->textrender);
}

/*----------------------------------------------------------------------------*/
static void
GuiTheme_set(GuiTheme* theme, Var v)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "fontname", "");
    VarValidator_declareArrayVar(valid, "fontoptions");
    VarValidator_declareArrayVar(valid, "bgtex");
    VarValidator_declareIntVar(valid, "borderleft", 0);
    VarValidator_declareIntVar(valid, "borderright", 0);
    VarValidator_declareIntVar(valid, "bordertop", 0);
    VarValidator_declareIntVar(valid, "borderbottom", 0);
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    GlTextRender_setFont(theme->textrender, Var_getValueString(Var_getArrayElemByCName(v, "fontname")));
    GlTextRender_setOptionsFromVar(theme->textrender, Var_getArrayElemByCName(v, "fontoptions"));
    
    GuiTexture_set(theme->bgtex, Var_getArrayElemByCName(v, "bgtex"));
    
    theme->bordertop = Var_getValueInt(Var_getArrayElemByCName(v, "bordertop"));
    theme->borderbottom = Var_getValueInt(Var_getArrayElemByCName(v, "borderbottom"));
    theme->borderleft = Var_getValueInt(Var_getArrayElemByCName(v, "borderleft"));
    theme->borderright = Var_getValueInt(Var_getArrayElemByCName(v, "borderright"));
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
guithemeInit()
{
    unsigned int i;
    
    for (i = 0; i < GUITHEME_NBSTD; i++)
    {
        GuiTheme_new(_stdthemes + i);
    }
    
    shellPrint(LEVEL_INFO, "Themes module loaded.");
}

/*----------------------------------------------------------------------------*/
void
guithemeUninit()
{
    unsigned int i;
    
    for (i = 0; i < GUITHEME_NBSTD; i++)
    {
        GuiTheme_del(_stdthemes + i);
    }

    shellPrint(LEVEL_INFO, "Themes module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
guithemeSet(Var v)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareArrayVar(valid, "button");
    VarValidator_declareArrayVar(valid, "buttonpushed");
    VarValidator_declareArrayVar(valid, "dialog");
    VarValidator_declareArrayVar(valid, "menu");
    VarValidator_declareArrayVar(valid, "input");
    VarValidator_declareArrayVar(valid, "tooltip");
    VarValidator_declareArrayVar(valid, "scrollbar");
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    GuiTheme_set(_stdthemes + GUITHEME_DIALOG, Var_getArrayElemByCName(v, "dialog"));
    GuiTheme_set(_stdthemes + GUITHEME_BUTTON, Var_getArrayElemByCName(v, "button"));
    GuiTheme_set(_stdthemes + GUITHEME_BUTTONPUSHED, Var_getArrayElemByCName(v, "buttonpushed"));
    GuiTheme_set(_stdthemes + GUITHEME_MENU, Var_getArrayElemByCName(v, "menu"));
    GuiTheme_set(_stdthemes + GUITHEME_INPUT, Var_getArrayElemByCName(v, "input"));
    GuiTheme_set(_stdthemes + GUITHEME_TOOLTIP, Var_getArrayElemByCName(v, "tooltip"));
    GuiTheme_set(_stdthemes + GUITHEME_SCROLLBAR, Var_getArrayElemByCName(v, "scrollbar"));
}

/*----------------------------------------------------------------------------*/
GuiTheme*
guithemeGetStd(GuiThemeStd type)
{
    ASSERT(type < GUITHEME_NBSTD, type = 0);
    
    return _stdthemes + type;
}
