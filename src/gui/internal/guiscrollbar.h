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
 *   Graphical scrollbars                                                     *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This file defines a GUI object used to modify a value by a slider and buttons.
 *
 * A GuiScrollbar allows the user to change a value directly by a slider or by increments
 * with the buttons.
 */

#ifndef _SW_GUISCROLLBAR_H_
#define _SW_GUISCROLLBAR_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiwidget.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure of a GuiScrollbar.
 */
typedef struct pv_GuiScrollbar pv_GuiScrollbar;

/*!
 * \brief Abstract type for a graphical scrollbar.
 */
typedef pv_GuiScrollbar* GuiScrollbar;

/*!
 * \brief Scrollbar orientation.
 */
typedef enum
{
    GUISCROLLBAR_HORIZONTAL = 0,        /*!< Horizontal orientation. */
    GUISCROLLBAR_VERTICAL = 1,          /*!< Vertical orientation, 0 is at bottom. */
    GUISCROLLBAR_VERTICALINV = 2        /*!< Vertical orientation, 0 is on top. */
} GuiScrollbarType;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a scrollbar.
 *
 * \param parent - Parent widget.
 * \param type - Scrollbar type.
 * \param tooltip - Tooltip text, NULL if not.
 * \return The newly allocated scrollbar.
 */
GuiScrollbar GuiScrollbar_new(GuiWidget parent, GuiScrollbarType type, const char* tooltip);

/*!
 * \brief Delete a scrollbar.
 *
 * \param scrollbar - The scrollbar.
 */
void GuiScrollbar_del(GuiScrollbar scrollbar);

/*!
 * \brief Get the widget associated with a scrollbar.
 *
 * \param scrollbar - The scrollbar.
 * \return Associated widget.
 */
GuiWidget GuiScrollbar_widget(GuiScrollbar scrollbar);

/*!
 * \brief Set the value limits for a scrollbar.
 *
 * This will correct the current value if needed.
 * \param scrollbar - The scrollbar.
 * \param min - Lower limit.
 * \param max - Upper limit.
 */
void GuiScrollbar_setLimits(GuiScrollbar scrollbar, Sint16 min, Sint16 max);

/*!
 * \brief Set the value increments for a scrollbar.
 *
 * \param scrollbar - The scrollbar.
 * \param smallinc - Small increment.
 * \param largeinc - Large increment.
 */
void GuiScrollbar_setIncrements(GuiScrollbar scrollbar, Uint16 smallinc, Uint16 largeinc);

/*!
 * \brief Change the value of a scrollbar.
 *
 * The value will be checked against the scrollbar limits and corrected.
 * \param scrollbar - The scrollbar.
 * \param val - New value.
 */
void GuiScrollbar_setValue(GuiScrollbar scrollbar, Sint16 val);

/*!
 * \brief Get the current value of a scrollbar.
 *
 * \param scrollbar - The scrollbar.
 * \return The current scrollbar value.
 */
Sint16 GuiScrollbar_getValue(GuiScrollbar scrollbar);

#endif
