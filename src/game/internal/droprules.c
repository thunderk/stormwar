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
 *   Rules to drop entities                                                   *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/internal/droprules.h"

#include "game/internal/game.h"
#include "game/internal/entity.h"
#include "game/internal/register.h"
#include "core/string.h"
#include "tools/varvalidator.h"
#include "world/ground.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_DropRules
{
    Bool needground;            /* Need ground under. */
    Bool denyground;            /* Can't drop if ground is present. */
    Bool groundconnexity;       /* Ground connexity required. */
    Entity** ground_entities;   /* Entities that can play the role of ground. */
    Entity** allowed_entities;  /* Entities explicitly allowed at the same location. */
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a NULL-terminated list of entities from an array of names.
 */
static Entity**
createEntityList(Var vlist)
{
    int i, j;
    Entity** ret;
    Var vent;
    Entity* ent;
    
    shellPrintf(LEVEL_ERRORSTACK, "While processing entity list {%s}.", Var_gets(vlist));
    
    ret = MALLOC(sizeof(Entity*) * (Var_getArraySize(vlist) + 1));
    i = j = 0;
    for (i = 0; i < Var_getArraySize(vlist); i++)
    {
        vent = Var_getArrayElemByPos(vlist, i);
        if (Var_getType(vent) != VAR_STRING)
        {
            shellPrintf(LEVEL_ERROR, "Invalid entity name {%s}.", Var_gets(vent));
        }
        else
        {
            ent = gameGetEntity(Var_getValueString(vent));
            if (ent == NULL)
            {
                shellPrintf(LEVEL_ERROR, "Entity '%s' not found.", String_get(Var_getValueString(vent)));
            }
            else
            {
                ret[j++] = ent;
            }
        }
    }
    
    ret[j] = NULL;
    
    shellPopErrorStack();
    return ret;
}

/*----------------------------------------------------------------------------*/
static Bool
searchEntityList(Entity** list, Entity* entity)
{
    while ((*list != NULL) && (*list != entity))
    {
        list++;
    }
    return *list != NULL;
}

/*----------------------------------------------------------------------------*/
/*check if at least on entity of the list if in the register results*/
static Bool
matchOne(Entity** list, RegisterResult* res)
{
    while (res->piece != NULL)
    {
        if (searchEntityList(list, Piece_getEntity(res->piece)))
        {
            /*one match found*/
            return TRUE;
        }
        res++;
    }
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
DropRules
DropRules_new(Var v)
{
    DropRules ret;
    VarValidator valid;
    
    shellPrint(LEVEL_ERRORSTACK, "For drop rules.");
    
    ret = MALLOC(sizeof(pv_DropRules));
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "needground", 0);
    VarValidator_declareIntVar(valid, "denyground", 0);
    VarValidator_declareIntVar(valid, "groundconnex", 0);
    VarValidator_declareArrayVar(valid, "ground_entities");
    VarValidator_declareArrayVar(valid, "allowed_entities");
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    ret->needground = ((Var_getValueInt(Var_getArrayElemByCName(v, "needground")) == 0) ? FALSE : TRUE);
    ret->denyground = ((Var_getValueInt(Var_getArrayElemByCName(v, "denyground")) == 0) ? FALSE : TRUE);
    ret->groundconnexity = ((Var_getValueInt(Var_getArrayElemByCName(v, "groundconnex")) == 0) ? FALSE : TRUE);
    ret->ground_entities = createEntityList(Var_getArrayElemByCName(v, "ground_entities"));
    ret->allowed_entities = createEntityList(Var_getArrayElemByCName(v, "allowed_entities"));
    
    shellPopErrorStack();
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
DropRules_del(DropRules rules)
{
    FREE(rules->allowed_entities);
    FREE(rules->ground_entities);
    FREE(rules);
}

/*----------------------------------------------------------------------------*/
Bool
DropRules_check(DropRules rules, WorldCoord posx, WorldCoord posy)
{
    RegisterResult* regi;
    RegisterResult* reg_pres;
    
    /*check if it can be registered*/
    if (registerIsOutbound(posx, posy))
    {
        return FALSE;
    }
    
    /*check denyground*/
    if (rules->denyground)
    {
        if (groundGetState(posx, posy))
        {
            return FALSE;
        }
    }
    
    reg_pres = registerQuery(posx, posy, NULL, NULL, REGISTER_PRESENCE);
    
    /*check needground*/
    if (rules->needground)
    {
        if (!groundGetState(posx, posy))
        {
            /*there is no ground under*/
            /*check if there is an entity that can play the role of ground*/
            if (!matchOne(rules->ground_entities, reg_pres))
            {
                FREE(reg_pres);
                return FALSE;
            }
        }
    }
    
    /*check allowed entities*/
    regi = reg_pres;
    while (regi->piece != NULL)
    {
        if (!searchEntityList(rules->allowed_entities, Piece_getEntity(regi->piece)))
        {
            /*an unallowed entity was found*/
            FREE(reg_pres);
            return FALSE;
        }
        regi++;
    }
    
    /*check ground connexity*/
    if (rules->groundconnexity)
    {
        Bool pass = FALSE;
        
        if (!registerIsOutbound(posx - 1, posy))
        {
            if (groundGetState(posx - 1, posy))
            {
                pass = TRUE;
            }
            else
            {
                RegisterResult* r;
                
                r = registerQuery(posx - 1, posy, NULL, NULL, REGISTER_PRESENCE);
                if (matchOne(rules->ground_entities, r))
                {
                    pass = TRUE;
                }
                FREE(r);
            }
        }
        if ((!pass) && (!registerIsOutbound(posx + 1, posy)))
        {
            if (groundGetState(posx + 1, posy))
            {
                pass = TRUE;
            }
            else
            {
                RegisterResult* r;
                
                r = registerQuery(posx + 1, posy, NULL, NULL, REGISTER_PRESENCE);
                if (matchOne(rules->ground_entities, r))
                {
                    pass = TRUE;
                }
                FREE(r);
            }
        }
        if ((!pass) && (!registerIsOutbound(posx, posy - 1)))
        {
            if (groundGetState(posx, posy - 1))
            {
                pass = TRUE;
            }
            else
            {
                RegisterResult* r;
                
                r = registerQuery(posx, posy - 1, NULL, NULL, REGISTER_PRESENCE);
                if (matchOne(rules->ground_entities, r))
                {
                    pass = TRUE;
                }
                FREE(r);
            }
        }
        if ((!pass) && (!registerIsOutbound(posx, posy + 1)))
        {
            if (groundGetState(posx, posy + 1))
            {
                pass = TRUE;
            }
            else
            {
                RegisterResult* r;
                
                r = registerQuery(posx, posy + 1, NULL, NULL, REGISTER_PRESENCE);
                if (matchOne(rules->ground_entities, r))
                {
                    pass = TRUE;
                }
                FREE(r);
            }
        }
        
        if (!pass)
        {
            FREE(reg_pres);
            return FALSE;
        }
    }
    
    /*all checks passed*/
    FREE(reg_pres);
    return TRUE;
}
