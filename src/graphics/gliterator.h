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
 *   Iterator to progress through a GlSurface                                 *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GLITERATOR_H_
#define _SW_GRAPHICS_GLITERATOR_H_ 1

/*!
 * \file
 * \brief Iterators to progress through a 2D surface.
 *
 * This iterators are used to wander easily on a \ref GlSurface.
 * \todo Maybe make the structure private.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private type of an iterator.
 * 
 * Don't change these fields directly! Use the functions and macros provided.
 */
typedef struct
{
    /*! Clipping rectangle */
    GlRect  rct;
    /*! Global surface rectangle */
    GlRect  defaultrct;
    /*! Specifies if the iterator has reached the end of surface */
    Bool    end;
    /*! Progressing factor along y (in number of PIXELS, not bytes). */
    Uint32  yprogr;
    /*! Current position in memory */
    GlColor* pos;
    /*! Position in memory of the top-left corner */
    GlColor* initpos;
    /*! Current horizontal position */
    Gl2DCoord  x;
    /*! Current vertical position */
    Gl2DCoord  y;
    /*! Horizontal limit for x */
    Gl2DCoord  xlim;
    /*! Vertical limit for y */
    Gl2DCoord  ylim;
} GlIterator;

/*!
 * \brief Starting position for an iterator.
 */
typedef enum
{
    CORNER_TOPLEFT,
    CORNER_TOPRIGHT,
    CORNER_BOTTOMLEFT,
    CORNER_BOTTOMRIGHT
} GlIteratorCorner;

/******************************************************************************
 *############################################################################*
 *#                            GlIterator functions                          #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Build an iterator for a given surface.
 *
 * \param iterator     - The iterator structure to init.
 * \param surf         - Surface from which extract information.
 */
void GlIterator_init(GlIterator* iterator, GlSurface surf);

/*!
 * \brief Increments horizontal position, then vertical if needed.
 *
 * The iterator progress through the clipping rectangle, scanning each
 * line from top to bottom and going left to right on each line.
 * \param iterator - The iterator.
 */
void GlIterator_fwd(GlIterator* iterator);

/*!
 * \brief Same as GlIterator_fwd but a second iterator is also pushed.
 *
 * Iterators must be well initialized and have the same position and same clip rectangle's size.
 * \param iterator1 - First iterator.
 * \param iterator2 - Second iterator.
 */
void GlIterator_fwdCoupled(GlIterator* iterator1, GlIterator* iterator2);

/*!
 * \brief Decrements horizontal position, then vertical if needed.
 *
 * The iterator progress through the clipping rectangle, scanning each
 * line from bottom to top and going right to left on each line.
 * \param iterator - The iterator.
 */
void GlIterator_bwd(GlIterator* iterator);

/*!
 * \brief Same as GlIterator_bwd but a second iterator is also pushed.
 *
 * Iterators must be well initialized and have the same position and same clip rectangle's size.
 * \param iterator1 - First iterator.
 * \param iterator2 - Second iterator.
 */
void GlIterator_bwdCoupled(GlIterator* iterator1, GlIterator* iterator2);

/*!
 * \brief Go up by one line in the surface.
 *
 * End is reached when the clip rectangle is encountered (no scan like fwd and bwd).
 * \param iterator - The iterator.
 */
void GlIterator_up(GlIterator* iterator);

/*!
 * \brief Same as GlIterator_up but a second iterator is also pushed.
 *
 * Iterators must be well initialized and have the same position and same clip rectangle's size.
 * \param iterator1 - First iterator.
 * \param iterator2 - Second iterator.
 */
void GlIterator_upCoupled(GlIterator* iterator1, GlIterator* iterator2);

/*!
 * \brief Go down by one line in the surface.
 *
 * End is reached when the clip rectangle is encountered (no scan like fwd and bwd).
 * \param iterator - The iterator.
 */
void GlIterator_down(GlIterator* iterator);

/*!
 * \brief Same as GlIterator_down but a second iterator is also pushed.
 *
 * Iterators must be well initialized and have the same position and same clip rectangle's size.
 * \param iterator1 - First iterator.
 * \param iterator2 - Second iterator.
 */
void GlIterator_downCoupled(GlIterator* iterator1, GlIterator* iterator2);

/*!
 * \brief Sets the iterator position to given coordinates, no check.
 *
 * Sets the position in coordinates relative to the clipping rectangle.
 * Must be used with caution, no check is performed.
 * \param iterator - The iterator.
 * \param x - Horizontal position.
 * \param y - Vertical position.
 */
void GlIterator_setPos(GlIterator* iterator, Gl2DCoord x, Gl2DCoord y);

/*!
 * \brief Set the color at the current position.
 *
 * Don't do this if the iterator is outside the surface!
 * \param _iterator_ - (\ref GlIterator) The iterator.
 * \param _color_ - (\ref GlColor) The color to set.
 */
#define GlIterator_setColor(_iterator_, _color_) *((_iterator_).pos)=(_color_)

/*!
 * \brief Get the color at the current position.
 *
 * Don't do this if the iterator is outside the surface!
 * \param _iterator_ - (\ref GlIterator) The iterator.
 * \return (\ref GlColor) The color.
 */
#define GlIterator_getColor(_iterator_) (*((_iterator_).pos))

/*!
 * \brief Sets the clipping rectangle that the iterator will scan.
 *
 * No check is performed, clipping rectangle must fit well into the surface.
 * \param iterator - The iterator.
 * \param rct - Clipping rectangle; if NULL, whole surface is considered.
 * \param corner - The corner where to place the iterator.
 */
void GlIterator_setClipRect(GlIterator* iterator, GlRect* rct, GlIteratorCorner corner);

/*!
 * \brief Check if the iterator has reached the end of the clipping rectangle.
 *
 * The end can only be caused by a fwd (or bwd) call.
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (BOOL) TRUE if the end of the surface was reached after a fwd or bwd call.
 */
#define GlIterator_endReached(_iterator_) ((_iterator_).end)

/*!
 * \brief Return the current absolute X position of the iterator.
 *
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (\ref Gl2DCoord) X absolute position.
 */
#define GlIterator_getX(_iterator_) ((_iterator_).x + (_iterator_).rct.x)

/*!
 * \brief Return the current absolute Y position of the iterator.
 *
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (\ref Gl2DCoord) Y absolute position.
 */
#define GlIterator_getY(_iterator_) ((_iterator_).y + (_iterator_).rct.y)

/*!
 * \brief Return the current X position (relative to the clipping rectangle) of the iterator.
 *
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (\ref Gl2DCoord) X relative position.
 */
#define GlIterator_getXclipped(_iterator_) ((_iterator_).x)

/*!
 * \brief Return the current Y position (relative to the clipping rectangle) of the iterator.
 *
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (\ref Gl2DCoord) Y relative position.
 */
#define GlIterator_getYclipped(_iterator_) ((_iterator_).y)

/*!
 * \brief Return the size in bytes of a clip line.
 *
 * The size is the length of a surface's line, inside the clipping rectangle.
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (Uint32) Surface's line size.
 */
#define GlIterator_getLineSize(_iterator_) ((_iterator_).rct.w * 4)

/*!
 * \brief Return the clipping rectangle for an iterator.
 *
 * \param _iterator_ - (GlIterator) The iterator.
 * \return (GlRect) Iterator's clipping rectangle.
 */
#define GlIterator_getRect(_iterator_) ((_iterator_).rct)

#endif
