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
 *   Ground management                                                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GROUND_H_
#define _SW_GROUND_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/world.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef WorldCoord GroundCoord;

/******************************************************************************
 *############################################################################*
 *#                              World functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the ground module.
 */
void groundInit(void);

/*!
 * \brief Destroy the ground.
 */
void groundUninit(void);

/*!
 * \brief Acknowledge a world size's change.
 *
 * A tile is a square, whose width and height are 1.0.
 * This will clear the ground.
 * \param nbx - Width of the world, in number of tiles
 * \param nby - Height of the world, in number of tiles
 */
void groundWorldSizeChanged(GroundCoord nbx, GroundCoord nby);

/*!
 * \brief Empty the ground.
 */
void groundClear(void);

/*!
 * \brief Generates a player area.
 *
 * \param minw - Width of the minimal rectangle required.
 * \param minh - Height of the minimal rectangle required.
 * \param x - Returned X position of the minimal rectangle (north-west corner).
 * \param y - Returned Y position of the minimal rectangle (north-west corner).
 * \return TRUE on failure.
 */
Bool groundAddPlayerArea(Uint16 minw, Uint16 minh, GroundCoord* x, GroundCoord* y);

/*!
 * \brief Set the ground state at a position.
 *
 * Given position can be outside the limits without error.
 * \param x - X position.
 * \param y - Y position.
 * \param state - New state.
 */
void groundSetState(GroundCoord x, GroundCoord y, Bool state);

/*!
 * \brief Check the state at a position.
 *
 * Given position can be outside the limits without error.
 * \param x - X position.
 * \param y - Y position.
 * \return TRUE if there is ground at the given position.
 */
Bool groundGetState(GroundCoord x, GroundCoord y);

#endif
