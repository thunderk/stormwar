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
 *   Object to virtually manage a player                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/internal/player.h"

#include "core/core.h"

#include "game/game.h"
#include "game/internal/game.h"
#include "game/internal/entity.h"
#include "world/ground.h"
#include "core/string.h"

#include "graphics/graphics.h"
#include "graphics/color.h"
#include "graphics/gl3dobject.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_Player
{
    PlayerType type;                /* Player type */
    String name;                    /* Player's name */
    int entities_nb;                /* Number of available entities for building */
    Entity** entities;              /* Entities available for building */
    union
    {
        struct
        {
            Gl3DObject creation_object;                 /* Creation object */
            int creation_entity_nb;                     /* Entity selected for building */
        } local;        /* Specific to local player */
        
        struct
        {
            void* temp;
        } ai;           /* Specific to AI players */
        
        struct
        {
            void* temp;
        } remote;       /* Specific to remote players */
    } p;                            /* Specific fields */
};

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
adjustEntities(Player player)
{
    /***TEMP***/
    int i;
    Entity* entities = gameGetAllEntities(&player->entities_nb);
    if (player->entities_nb != 0)
    {
        player->entities = MALLOC(sizeof(Entity*) * player->entities_nb);
        for (i = 0; i < player->entities_nb; i++)
        {
            player->entities[i] = &(entities[i]);
        }
    }
    /***TEMP***/
    
    gameUpdateEntitiesAvailable(player->entities_nb, player->entities);
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Player
Player_new(PlayerType type, String name)
{
    Player ret;
    GroundCoord x, y;
    
    if (groundAddPlayerArea(3, 3, &x, &y))
    {
        return NULL;
    }
    
    ret = MALLOC(sizeof(pv_Player));
    ret->type = type;
    ret->name = String_newByCopy(name);
    ret->entities_nb = 0;
    ret->entities = NULL;
    adjustEntities(ret);
    
    if (type == PLAYER_LOCAL)
    {
        gameCameraReset(x, y);
        ret->p.local.creation_entity_nb = -1;
        ret->p.local.creation_object = Gl3DObject_new(NULL, global_groupghost, NULL);
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Player_del(Player player)
{
    ASSERT(player != NULL, return);
    
    if (player->entities_nb != 0)
    {
        FREE(player->entities);
    }
    
    if (player->type == PLAYER_LOCAL)
    {
        Gl3DObject_del(player->p.local.creation_object);
    }
    
    String_del(player->name);
    FREE(player);
}

/*----------------------------------------------------------------------------*/
Piece
Player_processSelectionEvent(Player player, GlEvent* event)
{
    GlColorRGBA col;
    ASSERT(player != NULL, return NULL);
    ASSERT(event->type == GLEVENT_SELECTION, return NULL);
    
    if (event->event.selectionevent.type == SELECTIONEVENT_GROUNDMOVE)
    {
        Gl3DObject_setPos(player->p.local.creation_object, (Gl3DCoord)event->event.selectionevent.groundx, 0.0, (Gl3DCoord)event->event.selectionevent.groundy, 0);
        
        /*check against dropping rules for ghost mode*/
        if (player->p.local.creation_entity_nb >= 0)
        {
            if (DropRules_check(player->entities[player->p.local.creation_entity_nb]->droprules, event->event.selectionevent.groundx, event->event.selectionevent.groundy))
            {
                /*TODO: read this color from mod files*/
                GlColorRGBA_MAKE(col, 0x00, 0xFF, 0x00, 0xA0);
                Gl3DObject_setColor(player->p.local.creation_object, col, 0);
            }
            else
            {
                /*TODO: read this color from mod files*/
                GlColorRGBA_MAKE(col, 0xFF, 0x60, 0x60, 0xA0);
                Gl3DObject_setColor(player->p.local.creation_object, col, 0);
            }
        }
    }

    if ((event->event.selectionevent.type == SELECTIONEVENT_RIGHTCLICK) && (player->p.local.creation_entity_nb != -1))
    {
            /*TODO: rotate the piece*/
    }

    if ((event->event.selectionevent.type == SELECTIONEVENT_LEFTCLICK)
     && (player->p.local.creation_entity_nb >= 0))
    {
        /*check against dropping rules*/
        if (DropRules_check(player->entities[player->p.local.creation_entity_nb]->droprules, event->event.selectionevent.groundx, event->event.selectionevent.groundy))
        {
            /*we assume another object can't be dropped in the same place*/
            GlColorRGBA_MAKE(col, 0xFF, 0x60, 0x60, 0xA0);
            Gl3DObject_setColor(player->p.local.creation_object, col, 0);
            
            /*drop the piece*/
            return Piece_new(player->entities[player->p.local.creation_entity_nb], player, event->event.selectionevent.groundx, event->event.selectionevent.groundy);
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

/*----------------------------------------------------------------------------*/
int
Player_selectEntity(Player player, int entity)
{
    ASSERT(player != NULL, return -1);
    ASSERT(player->type == PLAYER_LOCAL, return -1);
    
    if ((entity >= 0) & (entity < player->entities_nb))
    {
        String s;
        
        player->p.local.creation_entity_nb = entity;
        Gl3DObject_setMesh(player->p.local.creation_object, player->entities[entity]->mesh);
        s = String_new("static");
        Gl3DObject_setAnim(player->p.local.creation_object, s, 0.0, 0);
        String_del(s);
    }
    else
    {
        player->p.local.creation_entity_nb = -1;
        Gl3DObject_setMesh(player->p.local.creation_object, NULL);
    }
    return player->p.local.creation_entity_nb;
}
