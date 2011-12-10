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
 *   Game engine internal functions                                           *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_INT_GAME_H_
#define _SW_INT_GAME_H_ 1

/*!
 * \file
 * \brief These are the game engine internal functions.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "game/internal/entity.h"
#include "game/internal/player.h"
#include "core/types.h"

/******************************************************************************
 *                                  Globals                                   *
 ******************************************************************************/
extern Player global_localplayer;

/******************************************************************************
 *############################################################################*
 *#                           Game engine functions                          #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Update the list of available entities for the local player.
 *
 * Only the local player can call this.
 * \param nb - Number of entities.
 * \param entities - Array of pointer to entities.
 */
void gameUpdateEntitiesAvailable(int nb, Entity** entities);

/*!
 * \brief Get all entities.
 *
 * \param r_nb - Pointer to an integer, will be filled with the number of entities.
 * \return Array of entities. \readonly
 */
Entity* gameGetAllEntities(int* r_nb);

/*!
 * \brief Retrieve an entity by its name.
 *
 * \param name - Entity's name.
 * \return The requested entity, NULL if the entity wasn't found.
 */
Entity* gameGetEntity(String name);

/*!
 * \brief Initialize camera control.
 */
void gameCameraInit(void);

/*!
 * \brief Quit camera control.
 */
void gameCameraUninit(void);

/*!
 * \brief Reset the camera position.
 *
 * \param x - X looked-at position.
 * \param y - Y looked-at position.
 */
void gameCameraReset(WorldCoord x, WorldCoord y);

#endif
