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
 *   Game kernel                                                              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_KERNEL_H_
#define _SW_KERNEL_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "world/world.h"

/******************************************************************************
 *############################################################################*
 *#                             Kernel functions                             #*
 *############################################################################*
 ******************************************************************************/

/*! \brief Initialize the kernel
 *
 *  This will initialize every module.
 *  This function will return only on exit (when the kernel is ready for uninit).
 */
void kernelInit(void);

/*! \brief Uninitialize the kernel
 *
 *  This will uninitialize every module.
 */
void kernelUninit(void);

/*!
 * \brief Set internationalisation language.
 *
 * This can be called at any time, even before the kernel is initialized.
 * Will be applied on the next mod loading.
 * \param lang - Language code, NULL for default.
 */
void kernelSetLanguage(char* lang);

/*!
 * \brief Start the demo with the default mod.
 */
void kernelTitle(void);

/*!
 * \brief Start a new game.
 *
 * \param sizex - Number of horizontal tiles for the world.
 * \param sizey - Number of vertical tiles for the world.
 * \param mod - Name of the requested mod.
 * \param name - Player's name.
 */
void kernelNewGame(WorldCoord sizex, WorldCoord sizey, String mod, String name);

#endif
