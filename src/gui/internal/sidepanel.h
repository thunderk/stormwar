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
 *   Panel that stands on the left of the screen.                             *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_SIDEPANEL_H_
#define _SW_SIDEPANEL_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/glsurface.h"
#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                            Sidepanel functions                           #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the panel
 */
void sidepanelInit(void);

/*!
 * \brief Uninitialize the panel
 */
void sidepanelUninit(void);

/*!
 * \brief Sets the sidepanel from a definition variable.
 *
 *  This will change the map too.
 *  \param v - The definition variable.
 */
void sidepanelSet(Var v);

/*!
 * \brief Sets the effective height of the panel.
 *
 * \param h - New height.
 */
void sidepanelSetHeight(Uint16 h);

/*!
 * \brief Sets the vertical offset.
 *
 * \param y - New vertical position.
 */
void sidepanelSetYPos(Sint16 y);

#endif
