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

#ifndef _SW_PIECE_H_
#define _SW_PIECE_H_ 1

/*!
 * \file
 * \brief This manages pieces dropped by players.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Private structure of a game piece. */
typedef struct pv_Piece pv_Piece;
    
/*! \brief Abstract type for a game piece. */
typedef pv_Piece* Piece;

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "game/internal/entity.h"
#include "world/world.h"
#include "game/internal/player.h"
#include "graphics/types.h"

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new piece.
 *
 * \param entity - Type of the piece.
 * \param owner - The owner.
 * \param x - Initial X position.
 * \param y - Initial Y position.
 * \return The newly allocated piece.
 */
Piece Piece_new(Entity* entity, Player owner, WorldCoord x, WorldCoord y);

/*!
 * \brief Delete a piece.
 *
 * \param piece - The piece.
 */
void Piece_del(Piece piece);

/*!
 * \brief Retrieve a piece position.
 *
 * \param piece - The piece.
 * \param x - Pointer to a x position to fill.
 * \param y - Pointer to a y position to fill.
 * \param z - Pointer to a z position to fill.
 */
void Piece_getPos(Piece piece, Gl3DCoord* x, Gl3DCoord* y, Gl3DCoord* z);

/*!
 * \brief Retrieve the type (entity) of a piece.
 *
 * \param piece - The piece.
 * \return The piece's entity.
 */
Entity* Piece_getEntity(Piece piece);

/*!
 * \brief Retrieve the graphical object of a piece.
 *
 * \param piece - The piece.
 * \return The piece's object.
 */
Gl3DObject Piece_getObject(Piece piece);

#endif
