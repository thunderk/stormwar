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
 *   Environement box around the world                                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_SKYBOX_H_
#define _SW_SKYBOX_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                             Skybox functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the skybox.
 */
void skyboxInit(void);

/*!
 * \brief Destroy the skybox.
 */
void skyboxUninit(void);

/*!
 * \brief Set the skybox from a definition variable.
 *
 * \param v - Definition variable.
 */
void skyboxSet(Var v);

/*!
 * \brief Acknowledge a world size's change.
 *
 * \param width - New width.
 * \param height - New height.
 * \param depth - New depth
 */
void skyboxWorldSizeChanged(Gl3DCoord width, Gl3DCoord height, Gl3DCoord depth);

/*!
 * \brief Set or unset lightning mode.
 *
 * \param l - TRUE if lightning is active.
 */
void skyboxSetLightning(Bool l);

#endif
