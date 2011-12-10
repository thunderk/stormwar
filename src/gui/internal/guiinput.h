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
 *   Object for user's text input                                             *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GUIINPUT_H_
#define _SW_GUIINPUT_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiwidget.h"
#include "tools/completion.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure of a GuiInput.
 */
typedef struct pv_GuiInput pv_GuiInput;

/*!
 * \brief Abstract type for an input area.
 */
typedef pv_GuiInput* GuiInput;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create an input area.
 *
 * \param parent - Parent widget.
 * \param history - Size of the history, 0 for no history.
 * \param tooltip - Tooltip text, NULL if not.
 * \return The newly allocated input area.
 */
GuiInput GuiInput_new(GuiWidget parent, unsigned int history, const char* tooltip);

/*!
 * \brief Delete an input area.
 *
 * \param input - The input area.
 */
void GuiInput_del(GuiInput input);

/*!
 * \brief Get the widget associated with an input area.
 *
 * \param input - The input area.
 * \return Associated widget.
 */
GuiWidget GuiInput_widget(GuiInput input);

/*!
 * \brief Get the minimal height required for an input area.
 *
 * \param input - The input area.
 * \return Required height.
 */
Uint16 GuiInput_getHeight(GuiInput input);

/*!
 * \brief Set the input area's text.
 *
 * This will produce a CHANGE event.
 * \param input - The input area.
 * \param text - Text to set.
 */
void GuiInput_setText(GuiInput input, const char* text);

/*!
 * \brief Get the current input text.
 *
 * \param input - The input area.
 * \return The input text.
 */
String GuiInput_getText(GuiInput input);

/*!
 * \brief Set the completion list for an input area.
 *
 * Completion will be done by the TAB key.
 * \param input - The input area.
 * \param completion - The completion list. If NULL, auto-completion will be disabled for this input area.
 */
void GuiInput_setCompletionList(GuiInput input, CompletionList completion);

#endif
