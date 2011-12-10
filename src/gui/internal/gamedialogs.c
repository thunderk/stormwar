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
 *   Game dialogs                                                             *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/gamedialogs.h"

#include "gui/guidialog.h"
#include "core/core.h"

#include "kernel.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID FUNC_NEWGAME = CORE_INVALID_ID;

static GuiDialog dlg_newgame = NULL;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
dialogCallback(GuiDialog dlg, GuiDialogButton button, Var vinput)
{
    if (dlg == dlg_newgame)
    {
        if (button == GUIDIALOGBUTTON_OK)
        {
            kernelNewGame(50, 50,
                          Var_getValueString(Var_getArrayElemByCName(vinput, "modname")),
                          Var_getValueString(Var_getArrayElemByCName(vinput, "playername"))
                         );
        }
        dlg_newgame = NULL;
    }
}

/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNC_NEWGAME)
    {
        if (dlg_newgame != NULL)
        {
            GuiDialog_del(dlg_newgame);
            dlg_newgame = NULL;
        }
        dlg_newgame = GuiDialog_new(GUIDIALOG_OKCANCEL, dialogCallback);
            GuiDialog_addTextInput(dlg_newgame, "playername", _("Player name : "), NULL, "Player");
            GuiDialog_addTextInput(dlg_newgame, "modname", _("Mod : "), _("Enter here the name of the modification you want to use."), "default");
        GuiDialog_launch(dlg_newgame);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
gamedialogsInit()
{
    MOD_ID = coreDeclareModule("dialogs", NULL, NULL, shellCallback, NULL, NULL, NULL);
    FUNC_NEWGAME = coreDeclareShellFunction(MOD_ID, "newgame", VAR_VOID, 0);
    
    dlg_newgame = NULL;
}

/*----------------------------------------------------------------------------*/
void
gamedialogsUninit()
{
    if (dlg_newgame != NULL)
    {
        GuiDialog_del(dlg_newgame);
        dlg_newgame = NULL;
    }
}
