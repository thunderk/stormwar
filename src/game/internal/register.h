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
 *   Game register                                                            *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_REGISTER_H_
#define _SW_REGISTER_H_ 1

/*!
 * \file
 * \brief The register keeps track of game pieces using flags.
 *
 * \todo Protection against multithreaded accesses.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "game/internal/piece.h"
#include "game/internal/entity.h"
#include "world/world.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Kind of flags that can be set in the register.
 */
typedef enum
{
    REGISTER_NONE,          /*!< No flag. */
    REGISTER_PRESENCE,      /*!< Flag used to set the presence of a piece at a location. */
    REGISTER_RANGE          /*!< Flag to mark a location to be inside a piece's range. */
} RegisterFlag;

/*!
 * \brief Type returned by register queries.
 */
typedef struct
{
    Piece piece;            /*!< The piece that set a flag. */
    RegisterFlag flag;      /*!< The flag. */
} RegisterResult;

/******************************************************************************
 *############################################################################*
 *#                            Register functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the game register.
 */
void registerInit(void);

/*!
 * \brief Destroy the game register.
 */
void registerUninit(void);

/*!
 * \brief Clear the register.
 */
void registerClear(void);

/*!
 * \brief Set a flag in the game register.
 *
 * If position is outside the register, it will be silently ignored.
 * \param x - World X position.
 * \param y - World Y position.
 * \param piece - Piece that set the flag.
 * \param flag - The flag to set.
 */
void registerSetFlag(WorldCoord x, WorldCoord y, Piece piece, RegisterFlag flag);

/*!
 * \brief Unset a flag in the game register.
 *
 * If the flag isn't set or if position is outside the register, it will
 * be silently ignored.
 * \param x - World X position.
 * \param y - World Y position.
 * \param piece - Piece that unset the flag.
 * \param flag - The flag to unset.
 */
void registerUnsetFlag(WorldCoord x, WorldCoord y, Piece piece, RegisterFlag flag);

/*!
 * \brief Check if coordinates are outside the register.
 *
 * \param x - X coordinate.
 * \param y - Y coordinate.
 * \return TRUE if coordinates are outside the register.
 */
Bool registerIsOutbound(WorldCoord x, WorldCoord y);

/*!
 * \brief Perform a query in the game register.
 *
 * Results returned will be limited to the given criteria.
 * \param x - X coordinate to query.
 * \param y - Y coordinate to query.
 * \param piece - Piece criterion, NULL if not.
 * \param entity - Entity criterion, NULL if not.
 * \param flag - Flag criterion, REGISTER_NONE if not.
 * \return A list of results that match the query. The last element has a NULL piece field. Don't forget to FREE it after use!
 */
RegisterResult* registerQuery(WorldCoord x, WorldCoord y, Piece piece, Entity* entity, RegisterFlag flag);

/*!
 * \brief Perform a check in the game register.
 *
 * This function works like \ref registerQuery but only returns a boolean answer.
 * \param x - X coordinate to query.
 * \param y - Y coordinate to query.
 * \param piece - Piece criterion, NULL if not.
 * \param entity - Entity criterion, NULL if not.
 * \param flag - Flag criterion, REGISTER_NONE if not.
 * \return TRUE if at least one result matched the query.
 */
Bool registerCheck(WorldCoord x, WorldCoord y, Piece piece, Entity* entity, RegisterFlag flag);

#endif
