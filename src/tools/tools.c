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
 *   Internal tools interface (shell module)                                  *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/core.h"

#include "tools/internal/texturizer.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static CoreID MOD_ID = 0;
static CoreID FUNC_TEXTURIZE = 0;
static CoreID FUNC_TEXTURESYM = 0;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNC_TEXTURIZE)
    {
        texturize(Var_getValueString(func->params[0]), Var_getValueString(func->params[1]));
    }
    else if (func->id == FUNC_TEXTURESYM)
    {
        texturesym(Var_getValueString(func->params[0]), Var_getValueString(func->params[1]));
    }
    Var_setVoid(func->ret);
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
toolsInit()
{
    MOD_ID = coreDeclareModule("tools", NULL, NULL, shellCallback, NULL, NULL, NULL);
    FUNC_TEXTURIZE = coreDeclareShellFunction(MOD_ID, "texturize", VAR_VOID, 2, VAR_STRING, VAR_STRING);
    FUNC_TEXTURESYM = coreDeclareShellFunction(MOD_ID, "texturesym", VAR_VOID, 2, VAR_STRING, VAR_STRING);
}

/*----------------------------------------------------------------------------*/
void
toolsUninit()
{
    shellPrint(LEVEL_INFO, "Tools module unloaded.");
}
