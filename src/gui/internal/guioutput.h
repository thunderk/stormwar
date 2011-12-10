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
 *   Object for text output                                                   *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This file defines a GUI object used to render a text output.
 *
 * A GuiOutput is used to render some text. The rendering is done by line.<br>
 * This object contains a graphical buffer to store previously rendered text.<br>
 * Each line drawn will push the buffer upwards (to make room for the new line).<br>
 * If the graphical buffer is bigger than the widget itself, a scrollbar will be
 * added to let the user scroll the text up and down.
 */

#ifndef _SW_GUIOUTPUT_H_
#define _SW_GUIOUTPUT_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/guiwidget.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure of a GuiOutput.
 */
typedef struct pv_GuiOutput pv_GuiOutput;

/*!
 * \brief Abstract type for an output area.
 */
typedef pv_GuiOutput* GuiOutput;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create an output area.
 *
 * \param parent - Parent widget.
 * \param height - Height of the graphical buffer used to render the output (if bigger than the widget, a scrollbar will be added)
 *                 If 0, the output's height will match the widget's.
 * \return The newly allocated output area.
 */
GuiOutput GuiOutput_new(GuiWidget parent, Uint16 height);

/*!
 * \brief Delete an output area.
 *
 * \param output - The output area.
 */
void GuiOutput_del(GuiOutput output);

/*!
 * \brief Get the widget associated with an output area.
 *
 * \param output - The output area.
 * \return Associated widget.
 */
GuiWidget GuiOutput_widget(GuiOutput output);

/*!
 * \brief Set the GuiOutput from a definition variable.
 *
 * \param output - The output area.
 * \param v - The definition variable.
 */
void GuiOutput_set(GuiOutput output, Var v);

/*!
 * \brief Change the text drawing color.
 *
 * \param output - The output area.
 * \param color - Drawing color.
 */
void GuiOutput_setFontColor(GuiOutput output, GlColorRGBA color);

/*!
 * \brief Print a text inside the output.
 *
 * This will push up the graphical buffer to make room for the new text.
 * \param output - The output area.
 * \param s - Text to render.
 */
void GuiOutput_print(GuiOutput output, String s);

#endif
