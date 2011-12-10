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
 *   Rectangle functions                                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GLRECT_H_
#define _SW_GRAPHICS_GLRECT_H_ 1

/*!
 * \file
 * \brief This file brings some basic functions to handle rectangles.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "graphics/types.h"

/******************************************************************************
 *############################################################################*
 *#                             GlRect functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Modify coordinates to fall inside the rectangle.
 *
 * \param rct - The clipping rectangle (will not be modified).
 * \param x   - X coordinate (will be modified).
 * \param y   - Y coordinate (will be modified).
 */
void returnInsideRect(GlRect* rct, Gl2DCoord* x, Gl2DCoord* y);

/*!
 * \brief Check if coordinates are inside a rectangle.
 * 
 * \param rct - The rectangle.
 * \param x - X coordinate to check.
 * \param y - Y coordinate to check.
 * \return TRUE if (x,y) is in rct.
 */
Bool isInRect(GlRect* rct, Gl2DCoord x, Gl2DCoord y);

/*!
 * \brief Clip a GlRect inside another (make a rect intersection).
 *
 * This can produce a flat rectangle (of size 0*y, x*0 or 0*0) if \a rct is fully outside \a clip.
 * \param rct  - Rectangle to clip (will be modified).
 * \param clip - Clipping rectangle (will not be modified).
 * \return TRUE if \a rct was modified, FALSE otherwise.
 */
Bool GlRect_clip(GlRect* rct, GlRect* clip);

/*!
 * \brief Compares two rectangles.
 *
 * \param rct1 - First rectangle.
 * \param rct2 - First rectangle.
 * \return rct1==rct2.
 */
Bool GlRect_equal(GlRect rct1, GlRect rct2);

/*!
 * \brief Apply borders cropping to a rectangle.
 *
 * This can produce a flat rectangle if borders are too big.
 * \param rct - Pointer to a rectangle (will be modified).
 * \param bleft - Size of the left border.
 * \param bright - Size of the right border.
 * \param btop - Size of the top border.
 * \param bbottom - Size of the bottom border.
 */
void GlRect_cropBorders(GlRect* rct, Gl2DSize bleft, Gl2DSize bright, Gl2DSize btop, Gl2DSize bbottom);

/*!
 * \brief Fill the values of a GlRect
 *
 * \param _rect_ - (\ref GlRect) The rectangle
 * \param _x_ - (\ref Gl2DCoord) X position
 * \param _y_ - (\ref Gl2DCoord) Y position
 * \param _w_ - (\ref Gl2DSize) width
 * \param _h_ - (\ref Gl2DSize) height
 */
#define GlRect_MAKE(_rect_,_x_,_y_,_w_,_h_) \
    (_rect_).x = _x_;\
    (_rect_).y = _y_;\
    (_rect_).w = _w_;\
    (_rect_).h = _h_

/*!
 * \brief Fill a GlRect structure with a named array variable.
 *
 * \param rct - Rectangle to fill.
 * \param v - Variable to read values from.
 */
void GlRect_makeFromVar(GlRect* rct, Var v);

#endif
