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
 *   Environement box around the world                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/internal/skybox.h"

#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

#include "graphics/graphics.h"
#include "graphics/color.h"
#include "graphics/gl3dobject.h"
#include "graphics/glmesh.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Gl3DObject globj;
static GlMesh glmesh;
static GlColorRGBA col;
static GlColorRGBA collightning;
static CoreTime lightningfadein;
static CoreTime lightningfadeout;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
skyboxInit()
{
    glmesh = NULL;
    
    globj = Gl3DObject_new(NULL, global_groupbackground, NULL);
    Gl3DObject_setMesh(globj, glmesh);
    lightningfadein = 0;
    lightningfadeout = 0;
    
    shellPrint(LEVEL_INFO, "Skybox module loaded.");
}

/*----------------------------------------------------------------------------*/
void
skyboxUninit()
{
    Gl3DObject_del(globj);
    if (glmesh != NULL)
    {
        GlMesh_del(glmesh);
    }
    shellPrint(LEVEL_INFO, "Skybox module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
skyboxSet(Var v)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    
    VarValidator_declareArrayVar(valid, "mesh");
    VarValidator_declareStringVar(valid, "anim", "");
    VarValidator_declareArrayVar(valid, "col");
    VarValidator_declareArrayVar(valid, "collightning");
    VarValidator_declareIntVar(valid, "lightningfadein", 20);
    VarValidator_declareIntVar(valid, "lightningfadeout", 30);
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    if (glmesh != NULL)
    {
        GlMesh_del(glmesh);
    }
    glmesh = GlMesh_new(Var_getArrayElemByCName(v, "mesh"));
    Gl3DObject_setMesh(globj, glmesh);
    Gl3DObject_setAnim(globj, Var_getValueString(Var_getArrayElemByCName(v, "anim")), 0, 0);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(v, "col"));
    Gl3DObject_setColor(globj, col, 0);
    GlColorRGBA_makeFromVar(&collightning, Var_getArrayElemByCName(v, "collightning"));
    lightningfadein = (CoreTime)Var_getValueInt(Var_getArrayElemByCName(v, "lightningfadein"));
    lightningfadeout = (CoreTime)Var_getValueInt(Var_getArrayElemByCName(v, "lightningfadeout"));
}

/*----------------------------------------------------------------------------*/
void
skyboxSetLightning(Bool l)
{
    if (l)
    {
        Gl3DObject_setColor(globj, collightning, lightningfadein);
    }
    else
    {
        Gl3DObject_setColor(globj, col, lightningfadeout);
    }
}
