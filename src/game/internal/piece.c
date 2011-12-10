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
 *   Pieces dropped by players                                                *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/internal/piece.h"

#include "core/string.h"
#include "graphics/graphics.h"
#include "graphics/gl3dobject.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_Piece
{
    Entity* entity;
    Player owner;
    Gl3DObject obj;
};

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
Piece
Piece_new(Entity* entity, Player owner, WorldCoord x, WorldCoord y)
{
    Piece ret;
    
    ret = (Piece)MALLOC(sizeof(pv_Piece));
    ret->entity = entity;
    ret->owner = owner;
    ret->obj = Gl3DObject_new(NULL, (entity->blended) ? global_grouptransparent : global_groupnormal, NULL);
    Gl3DObject_setMesh(ret->obj, entity->mesh);
    /***DEBUG***/
    /*this will be done by the behavior rules*/
    Gl3DObject_setAnim(ret->obj, String_new("idle"), 0.0, 0);
    /***DEBUG***/
    Gl3DObject_setPos(ret->obj, (Gl3DCoord)x, (Gl3DCoord)0.0, (Gl3DCoord)y, 0);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Piece_del(Piece piece)
{
    Gl3DObject_del(piece->obj);
    FREE(piece);
}

/*----------------------------------------------------------------------------*/
void
Piece_getPos(Piece piece, Gl3DCoord* x, Gl3DCoord* y, Gl3DCoord* z)
{
    *x = Gl3DObject_getX(piece->obj);
    *y = Gl3DObject_getY(piece->obj);
    *z = Gl3DObject_getZ(piece->obj);
}

/*----------------------------------------------------------------------------*/
Entity*
Piece_getEntity(Piece piece)
{
    return piece->entity;
}

/*----------------------------------------------------------------------------*/
Gl3DObject
Piece_getObject(Piece piece)
{
    return piece->obj;
}
