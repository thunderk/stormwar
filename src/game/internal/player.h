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

#ifndef _SW_PLAYER_H_
#define _SW_PLAYER_H_ 1

/*!
 * \file
 * \brief This object manages players (local, remote and artificial).
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"
#include "game/game.h"
#include "graphics/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Player types.
 */
typedef enum
{
    PLAYER_LOCAL,       /*!< Local player. */
    PLAYER_REMOTE,      /*!< Remote player. */
    PLAYER_AI           /*!< Artificial intelligence. */
} PlayerType;

/*!
 * \brief Private structure for a Player.
 */
typedef struct pv_Player pv_Player;
 
/*!
 * \brief Abstract type for a player.
 */
typedef pv_Player* Player;

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "game/internal/piece.h"

/******************************************************************************
 *############################################################################*
 *#                             Player functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new player.
 *
 * \param type - Player type.
 * \param name - Player's name, will be copied.
 * \return The newly allocated player, NULL in case of error.
 */
Player Player_new(PlayerType type, String name);

/*!
 * \brief Delete a player.
 *
 * \param player - The player.
 */
void Player_del(Player player);

/*!
 * \brief Process a local selection event.
 *
 * \param player - The player, must be PLAYER_LOCAL.
 * \param event - The selection event.
 * \return The piece created, NULL if no piece was created.
 */
Piece Player_processSelectionEvent(Player player, GlEvent* event);

/*!
 * \brief Select an entity for building by its position in the GameEntitiesInfo.
 *
 * \param player - The player, must be PLAYER_LOCAL.
 * \param entity - Entity required, -1 for none.
 * \return The really selected entity, -1 for none.
 */
int Player_selectEntity(Player player, int entity);

#endif
