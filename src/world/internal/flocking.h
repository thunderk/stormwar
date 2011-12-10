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
 *   Module that controls the flocking of entities (such as fishes, birds...) *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_FLOCKING_H_
#define _SW_FLOCKING_H_ 1

/*!
 * \file
 * \brief Flocking implementation.
 *
 * \todo Optimize algorithms (don't use Gl3DObject_get... for position).
 * \todo Use its own 3d group.
 * \todo Maybe use Gl3DObject animation for position when it's done.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/world.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Private structure for a BoidGroup */
typedef struct pv_BoidGroup pv_BoidGroup;

/*! \brief Group of boids, to control the parameters of this group */
typedef pv_BoidGroup* BoidGroup;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the module.
 */
void flockingInit(void);

/*!
 * \brief Destroy the module.
 */
void flockingUninit(void);

/*!
 * \brief Delete all boid groups.
 * This must be called by the MAIN thread.
 */
void flockingClear(void);

/*!
 * \brief Add a group of boids in the flocking system.
 *
 * This must be called by the MAIN thread.
 * \param params - Named array containing the flocking parameters of the group.
 * \return The newly allocated group, NULL in case of error.
 */
BoidGroup flockingAddGroup(Var params);

/*!
 * \brief Remove a group from the flocking system.
 *
 * This must be called by the MAIN thread.
 * The group and its boids will be destroyed.
 * \param group - The group to remove.
 */
void flockingDelGroup(BoidGroup group);

/*!
 * \brief Acknowledge a world size change.
 *
 * \param w - New world width.
 * \param h - New world height.
 */
void flockingWorldSizeChanged(WorldCoord w, WorldCoord h);

#endif
