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
 *   Graphical cursor                                                         *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This module handles the graphical cursor.
 *
 * This module handles:
 *  \li Graphical cursor representation.
 *  \li Cursor acceleration.
 *  \li SDL cursor grabbing.
 *  \li Camera control mode (either by edge modes or by control key).
 *
 * Events produced:
 *  \li MOUSEEVENT_BUTTONPRESSED
 *  \li MOUSEEVENT_BUTTONRELEASED
 *  \li MOUSEEVENT_MOVE
 *  \li MOUSEEVENT_WHEELUP
 *  \li MOUSEEVENT_WHEELDOWN
 *  \li MOUSEEVENT_CONTROL
 *  \li MOUSEEVENT_EDGES
 *
 * \todo Get rid of the #include "SDL.h"
 * \todo Shell functions
 */

#ifndef _SW_GRAPHICS_IMPL_CURSOR_H_
#define _SW_GRAPHICS_IMPL_CURSOR_H_ 1

/******************************************************************************
 *                                   Includes                                 *
 ******************************************************************************/
#include "main.h"

#include "graphics/types.h"
#include "core/types.h"
#include "SDL.h"

/******************************************************************************
 *############################################################################*
 *#                             Cursor functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initializes cursor module.
 */
void cursorInit(void);

/*!
 * \brief Destroy cursor module.
 */
void cursorUninit(void);

/*!
 * \brief Set the cursors from an array.
 *
 * \param v - Definition array containing all cursors definition variables.
 */
void cursorSetCursors(Var v);

/*!
 * \brief Process an SDL event.
 *
 * \param event - SDL event, won't be modified.
 */
void cursorProcessEvent(SDL_Event* event);

/*!
 * \brief Finalize cursor events for an input frame.
 *
 * \mainthread
 * This must be called at each input frame and will send events to graphics engine.
 * This function will handle if no mouse events were received since last call and produce
 * a 'void' mouse motion event.
 */
void cursorCollectEvents(void);

/*!
 * \brief Retrieve the current cursor position.
 *
 * This is the position of the cursor inside the screen.
 * \param posx - Returned x position.
 * \param posy - Returned y position.
 */
void cursorGetPos(Gl2DCoord* posx, Gl2DCoord* posy);

#endif
