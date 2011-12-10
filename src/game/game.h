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
 *   Game engine                                                              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GAME_H_
#define _SW_GAME_H_ 1

/*!
 * \file
 * \brief This is the game engine.
 *
 * The game engine handles all player interactions and game rules.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "graphics/glsurface.h"

/******************************************************************************
 *############################################################################*
 *#                           Game engine functions                          #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the game engine.
 *
 * Graphical engine must be loaded before this.
 */
void gameInit(void);

/*!
 * \brief Destroy the game engine.
 *
 * Graphical engine should be still alive at this point.
 */
void gameUninit(void);

/*!
 * \brief Create a new empty game.
 *
 * This will :
 *  \li Clear the ground.
 *  \li Remove all players.
 */
void gameNew(void);

/*!
 * \brief Add a local player.
 *
 * There can only be one local player. It is the one who controls inputs.
 * This player must be added first.
 * \param name - Player's name.
 */
void gameAddLocalPlayer(String name);

/*void gameAddRemotePlayer(String name, ...);*/

/*void gameAddAIPlayer(String name, ...);*/

#endif
