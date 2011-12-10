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
 *   Screen management                                                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_IMPL_GLSCREEN_H_
#define _SW_GRAPHICS_IMPL_GLSCREEN_H_ 1

/*!
 * \file
 * \brief Screen management.
 *
 * \todo Decide if the resolution is handled here or in graphics.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"

/******************************************************************************
 *                              Shared variables                              *
 ******************************************************************************/
/*!
 * \brief Width of the screen.
 */
extern Gl2DSize global_screenwidth;

/*!
 * \brief Height of the screen.
 */
extern Gl2DSize global_screenheight;

/******************************************************************************
 *############################################################################*
 *#                             Screen functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the screen module.
 */
void glscreenInit(void);

/*!
 * \brief Destroy the screen module.
 */
void glscreenUninit(void);

/*!
 * \brief Change the screen size and set the video mode for fullscreen.
 *
 * \param width      - Pointer to an horizontal size, can be changed.
 * \param height     - Pointer to a vertical size, can be changed.
 * \param fullscreen - Specifies if the screen area must take the whole screen, can be changed.
 */
void glscreenSetVideo(Gl2DSize* width, Gl2DSize* height, Bool* fullscreen);

/*!
 * \brief Update screen on rendering device.
 */
#define glscreenUpdate() SDL_GL_SwapBuffers()

#endif
