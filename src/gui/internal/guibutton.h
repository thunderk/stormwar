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
 *   Buttons for the GUI                                                      *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief Buttons for graphical interface.
 */

#ifndef _SW_GUIBUTTON_H_
#define _SW_GUIBUTTON_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiwidget.h"
#include "graphics/glrect.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure for a GuiButton.
 */
typedef struct pv_GuiButton pv_GuiButton;
    
/*!
 * \brief Abstract type for a graphical button.
 */
typedef pv_GuiButton* GuiButton;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a button.
 *
 * \param parent - Parent widget.
 * \param clickrepeat - Time in milliseconds between two GUIEVENT_CLICKED throws, 0 to disable repeating.
 * \param tooltip - Tooltip text, NULL if not.
 * \return The newly allocated button.
 */
GuiButton GuiButton_new(GuiWidget parent, Uint16 clickrepeat, const char* tooltip);

/*!
 * \brief Delete a button.
 *
 * \param button - The button.
 */
void GuiButton_del(GuiButton button);

/*!
 * \brief Get the widget associated with a button.
 *
 * \param button - The button.
 * \return Associated widget.
 */
GuiWidget GuiButton_widget(GuiButton button);

/*!
 * \brief Get the size that a button will normally require.
 *
 * Call this after setting all parameters.
 * \param button - The button
 * \param rct - Pointer to an allocated rectangle structure. Only size fields will be modified.
 */
void GuiButton_requiredSize(GuiButton button, GlRect* rct);

/*!
 * \brief Set the text content of a button.
 *
 * This will redraw the button.
 * \param button - The button.
 * \param caption - The text, will be copied.
 */
void GuiButton_setCaption(GuiButton button, String caption);

#endif
