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
 *   Module to handle tooltip display                                         *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief Module to handle tooltip displaying.
 *
 * This module is mainly called by Guiwidget objects to display
 * associated tooltips.
 * Pay attention to not set tooltips on overlapping widgets since they will
 * all produce a tooltip alternatively (which will cause a CPU overhead). It is
 * a good idea to put tooltips only on end-level widgets such as buttons.
 *
 * \todo Replace the thread slot by using GLEVENT_DRAW.
 */

#ifndef _SW_GUITOOLTIP_H_
#define _SW_GUITOOLTIP_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "gui/internal/guiwidget.h"

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create the tooltip module.
 */
void guitooltipInit(void);

/*!
 * \brief Destroy the tooltip module.
 */
void guitooltipUninit(void);

/*!
 * \brief Set a tooltip.
 *
 * Call this each time the mouse is moved over a tooltip-ed widget.
 * \param text - Text to display in the tooltip.
 * \param rect - Absolute screen rectangle.
 * \param caller - Widget that called this function (used to handle multiple calls).
 */
void guitooltipSet(String text, GlRect* rect, GuiWidget caller);

#endif
