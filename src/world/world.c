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
 *   World parameters                                                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/world.h"
#include "world/ground.h"
#include "graphics/types.h"
#include "world/internal/skybox.h"
#include "world/internal/flocking.h"
#include "tools/fonct.h"

#include "core/core.h"

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID RES_WIDTH = CORE_INVALID_ID;
static CoreID RES_HEIGHT = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
worldInit()
{
    MOD_ID = coreDeclareModule("world", NULL, NULL, NULL, NULL, NULL, NULL);
    RES_WIDTH = coreCreateResource(MOD_ID, "world_width", VAR_INT, FALSE);
    RES_HEIGHT = coreCreateResource(MOD_ID, "world_height", VAR_INT, FALSE);
}

/*----------------------------------------------------------------------------*/
void
worldUninit()
{
    shellPrint(LEVEL_INFO, "World module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
worldSetSize(WorldCoord nbx, WorldCoord nby)
{
    nbx = MAX(nbx, 10);
    nby = MAX(nby, 10);
    
    Var_setInt(temp_corevar, nbx);
    coreSetResourceValue(MOD_ID, RES_WIDTH, temp_corevar);
    Var_setInt(temp_corevar, nby);
    coreSetResourceValue(MOD_ID, RES_HEIGHT, temp_corevar);
    
    /*graphicsWorldSizeChanged((Gl3DCoord)nbx, (Gl3DCoord)nby);*/
    groundWorldSizeChanged((Gl3DCoord)nbx, (Gl3DCoord)nby);
    flockingWorldSizeChanged(nbx, nby);
}
