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
 *   World natural environnement                                              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/env.h"

#include "world/internal/flocking.h"
#include "world/internal/thunderbolt.h"
#include "world/internal/skybox.h"

#include "graphics/light.h"

#include "core/core.h"
#include "core/ptrarray.h"

#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static CoreID MOD_ID = CORE_INVALID_ID;
static PtrArray _lights;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
datasCallback(Var datas)
{
    VarValidator valid;
    Var v;
    VarArrayPos i;
    Light l;
    
    flockingClear();
    PtrArray_clear(_lights);
    
    valid = VarValidator_new();
    
    VarValidator_declareArrayVar(valid, "flocks");
    VarValidator_declareArrayVar(valid, "thunderbolts");
    VarValidator_declareArrayVar(valid, "skybox");
    VarValidator_declareArrayVar(valid, "lights");
    
    VarValidator_validate(valid, datas);
    VarValidator_del(valid);
    
    /*flocks of boids*/
    v = Var_getArrayElemByCName(datas, "flocks");
    i = 0;
    while (i < Var_getArraySize(v))
    {
        flockingAddGroup(Var_getArrayElemByPos(v, i));
        i++;
    }

    /*thunderbolt parameters*/
    thunderboltSet(Var_getArrayElemByCName(datas, "thunderbolts"));
    
    /*skybox*/
    skyboxSet(Var_getArrayElemByCName(datas, "skybox"));
    
    /*global lights*/
    v = Var_getArrayElemByCName(datas, "lights");
    i = 0;
    while (i < Var_getArraySize(v))
    {
        l = lightAdd();
        lightSetFromVar(l, Var_getArrayElemByPos(v, i));
        lightToggle(l, TRUE);
        i++;
        PtrArray_append(_lights, l);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
envInit()
{
    skyboxInit();
    flockingInit();
    thunderboltInit();
    
    _lights = PtrArray_newFull(2, 2, (PtrFunc)lightDel, NULL);

    MOD_ID = coreDeclareModule("env", NULL, datasCallback, NULL, NULL, NULL, NULL);
}

/*----------------------------------------------------------------------------*/
void
envUninit()
{
    thunderboltUninit();
    flockingUninit();
    skyboxUninit();
    
    PtrArray_del(_lights);
    
    shellPrint(LEVEL_INFO, "Environment module unloaded.");
}
