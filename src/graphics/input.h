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
 *   Functions to control input behavior                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_INPUT_H_
#define _SW_GRAPHICS_INPUT_H_ 1

/*!
 * \file
 * \brief Functions to control input behavior.
 *
 * \par Control mode.
 * While the mouse is in control mode, the cursor won't move. All relative mouse
 * moves will be passed as \a x and \a y fields in a \ref MOUSEEVENT_CONTROL event.
 *
 * \par Active edges.
 * The screen can have active edges. When this is the case, \ref MOUSEEVENT_EDGES events will
 * be produced if the cursor is inside active area. \a x and \a y fields of these events will
 * be filled with the 'distance' of the cursor inside the active area.<br>
 * If the active area is partially or totally outside the screen, the cursor can virtually
 * go outside but will physically be limited inside the screen (meaning events such as
 * \ref MOUSEEVENT_MOVE will have coordinates inside the screen).<br>
 * If a reset trigger is set, the cursor will be brought back at the limit of the active area
 * after the \ref MOUSEEVENT_EDGES event is produced.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure of a Cursor.
 */
typedef struct pv_Cursor pv_Cursor;

/*!
 * \brief Abstract type for a cursor.
 */
typedef pv_Cursor* Cursor;

/******************************************************************************
 *############################################################################*
 *#                            cursor.c functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Put the mouse in control mode or not.
 *
 * \param control - Control mode (TRUE) or not (FALSE).
 */
void inputSetMouseControl(Bool control);

/*!
 * \brief Set screen edges to be active with the mouse.
 *
 * To disable active edges, just put 0 for \a insize and \a outsize.
 * \param insize - Size of the active area inside the screen.
 * \param outsize - Size of the active area outside the screen.
 * \param reset - Bring back the cursor after an event is produced.
 */
void inputSetMouseActiveEdges(Gl2DSize insize, Gl2DSize outsize, Bool reset);

/*!
 * \brief Set the mouse acceleration parameters.
 *
 * \param factor - Acceleration factor.
 * \param deadzone - Dead zone.
 */
void inputSetMouseAcceleration(Uint16 factor, Uint16 deadzone);

/*!
 * \brief Get a cursor by its name.
 *
 * The cursor will be available up to the next CORE_DATALOADING event.
 * \param name - The cursor name.
 * \return The required cursor, a default usable cursor if not found.
 */
Cursor inputGetCursor(String name);

/*!
 * \brief Set the current cursor.
 *
 * \param cursor - The cursor to set as current one.
 */
void inputSetCurrentCursor(Cursor cursor);

/*!
 * \brief Set a cursor for one frame.
 *
 * \param cursor - The cursor to set for one frame.
 */
void inputSetTempCursor(Cursor cursor);

#endif
