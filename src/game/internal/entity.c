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
 *   Type for the game pieces                                                 *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/internal/entity.h"

#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

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
Entity_set(Entity* entity, Var v)
{
    VarValidator valid;
    
    shellPrintf(LEVEL_INFO, "Loading '%s' entity.", String_get(Var_getName(v)));
    
    valid = VarValidator_new();
    VarValidator_declareArrayVar(valid, "mesh");
    VarValidator_declareIntVar(valid, "width", 1);
    VarValidator_declareIntVar(valid, "height", 1);
    VarValidator_declareFloatVar(valid, "rangedist", 0.0);
    VarValidator_declareFloatVar(valid, "rangeangle", 0.0);
    VarValidator_declareArrayVar(valid, "rules_drop");
    VarValidator_declareIntVar(valid, "keepdrop", 0);
    VarValidator_declareIntVar(valid, "static", 1);
    VarValidator_declareIntVar(valid, "selectable", 1);
    VarValidator_declareIntVar(valid, "blended", 0);
    
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    entity->name = String_newByCopy(Var_getName(v));
    shellPrintf(LEVEL_ERRORSTACK, "For entity named '%s'.", String_get(entity->name));
    
    entity->mesh = GlMesh_new(Var_getArrayElemByCName(v, "mesh"));
    entity->rangedist = Var_getValueFloat(Var_getArrayElemByCName(v, "rangedist"));
    entity->rangeangle = Var_getValueFloat(Var_getArrayElemByCName(v, "rangeangle"));
    entity->droprules_var = Var_new();
    Var_setFromVar(entity->droprules_var, Var_getArrayElemByCName(v, "rules_drop"));
    entity->droprules = NULL;
    entity->keepdrop = (Var_getValueInt(Var_getArrayElemByCName(v, "keepdrop")) != 0) ? TRUE : FALSE;
    entity->isstatic = (Var_getValueInt(Var_getArrayElemByCName(v, "static")) != 0) ? TRUE : FALSE;
    entity->selectable = (Var_getValueInt(Var_getArrayElemByCName(v, "selectable")) != 0) ? TRUE : FALSE;
    entity->blended = (Var_getValueInt(Var_getArrayElemByCName(v, "blended")) != 0) ? TRUE : FALSE;
    entity->width = imax(Var_getValueInt(Var_getArrayElemByCName(v, "width")), 1);
    entity->height = imax(Var_getValueInt(Var_getArrayElemByCName(v, "height")), 1);
    
    shellPopErrorStack();
    shellPrintf(LEVEL_INFO, "Entity '%s' loaded.", String_get(entity->name));
}

/*----------------------------------------------------------------------------*/
void
Entity_activate(Entity* entity)
{
    shellPrintf(LEVEL_INFO, "Activating '%s' entity.", String_get(entity->name));
    entity->droprules = DropRules_new(entity->droprules_var);
    Var_del(entity->droprules_var);
    entity->droprules_var = NULL;
}

/*----------------------------------------------------------------------------*/
void
Entity_unset(Entity* entity)
{
    GlMesh_del(entity->mesh);
    String_del(entity->name);
    DropRules_del(entity->droprules);
}
