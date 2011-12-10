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
 *   Graphical surface management                                             *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GLSURFACE_H_
#define _SW_GRAPHICS_GLSURFACE_H_ 1

/*!
 * \file
 * \brief Graphical 2D surfaces.
 *
 * \todo See if it is still needed to convert surfaces to display format.
 * \todo Anti-aliased lines and circles.
 * \todo Discs and ovoids.
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
 * \brief Resize modes.
 */
typedef enum
{
    SURFACE_UNDEFINED,      /*!< Undefined result. */
    SURFACE_KEEP,           /*!< Keep the source as much as possible. */
    SURFACE_RESAMPLE        /*!< Resample to fit in the new size. */
} GlSurfaceResizeMode;

/******************************************************************************
 *############################################################################*
 *#                            GlSurface functions                           #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new 2D surface.
 *
 * \param width  - Horizontal size.
 * \param height - Vertical size.
 * \param alpha  - Specifies if alpha blending will be enabled for this surface as source.
 * \return The newly allocated surface.
 */
GlSurface GlSurface_new(Gl2DSize width, Gl2DSize height, Bool alpha);

/*!
 * \brief Create a new 2D surface, copying exactly another.
 *
 * \param src - The surface to copy.
 * \return The newly allocated surface.
 */
GlSurface GlSurface_newByCopy(GlSurface src);

/*!
 * \brief Create a graphical surface from an image file.
 *
 * \param imagefile - Path and name of the file from which the surface is created (path relative to the mod folder).
 * \return The newly allocated surface, a default surface if failed.
 */
GlSurface GlSurface_newFromFile(String imagefile);

/*!
 * \brief Destroy a surface.
 *
 * \param surf - The surface.
 */
void GlSurface_del(GlSurface surf);

/*!
 * \brief Resize a surface.
 *
 * This function invalidates every GlIterator created for this surface, they need to be re-created.
 * \param surf - The surface.
 * \param w - New width.
 * \param h - New height.
 * \param mode - Resize mode.
 */
void GlSurface_resize(GlSurface surf, Gl2DSize w, Gl2DSize h, GlSurfaceResizeMode mode);

/*!
 * \brief Save a surface content in an ascii PNM file.
 *
 * \param surf - The surface to save.
 * \param file - File path and name (full path or relative to the current folder).
 */
void GlSurface_saveToPNM(GlSurface surf, String file);

/*!
 * \brief Get the width of a surface.
 *
 * \param surface - The surface.
 * \return The surface's width.
 */
Gl2DSize GlSurface_getWidth(GlSurface surface);

/*! 
 * \brief Get the height of a surface.
 *
 * \param surface - The surface.
 * \return The surface's height.
 */
Gl2DSize GlSurface_getHeight(GlSurface surface);

/*!
 * \brief Get the surface's rectangle.
 *
 * \param surface - The surface.
 * \param rect - A pointer to a valid rectangle structure to fill.
 */
void GlSurface_getRect(GlSurface surface, GlRect* rect);

/*!
 * \brief Get a pointer to the first pixel of the surface.
 *
 * READ ONLY! Use it with extreme cautious.
 * \param surface - The surface.
 * \return A pointer to the first pixel.
 */
GlColor* GlSurface_getPixels(GlSurface surface);

/*!
 * \brief Get a pixel color inside a surface.
 *
 * No check is performed, position is supposed to be inside the surface.
 * \param surf - The surface.
 * \param x - X position of the pixel.
 * \param y - Y position of the pixel.
 * \return Pixel color.
 */
GlColor GlSurface_getColor(GlSurface surf, Gl2DCoord x, Gl2DCoord y);

/*!
 * \brief Fill a whole surface with a color
 *
 * \param surface - The surface
 * \param color - The color to fill the surface with.
 */
void GlSurface_clear(GlSurface surface, GlColor color);

/*!
 * \brief Draw a pixel at a position inside the surface without check.
 *
 * \param surf - The surface.
 * \param x    - Horizontal position.
 * \param y    - Vertical position.
 * \param col  - Drawing color.
 */
void GlSurface_drawPixel(GlSurface surf, Gl2DCoord x, Gl2DCoord y, GlColor col);

/*!
 * \brief Draw a pixel at a position inside the surface with check.
 *
 * \param surf - The surface.
 * \param x    - Horizontal position.
 * \param y    - Vertical position.
 * \param col  - Drawing color.
 */
void GlSurface_drawPixelSecure(GlSurface surf, Gl2DCoord x, Gl2DCoord y, GlColor col);

/*!
 * \brief Draw a rectangle, no check.
 *
 * \param surf      - The surface.
 * \param rct       - Rectangle.
 * \param colfill   - Filling color.
 */
void GlSurface_drawRect(GlSurface surf, GlRect rct, GlColor colfill);

/*!
 * \brief Draw a rectangle and its border, no check.
 *
 * \param surf      - The surface.
 * \param rct       - Overall rectangle.
 * \param border    - Border size (in pixels, can be 0).
 * \param colborder - Border color.
 * \param colfill   - Filling color.
 */
void GlSurface_drawBorderRect(GlSurface surf, GlRect rct, Gl2DSize border, GlColor colborder, GlColor colfill);

/*!
 * \brief Draw a line supposed to be inside the surface.
 *
 * This function is quicker than non-secure ones but hope this line doesn't go out of the surface.
 * \param surf - The surface.
 * \param x1   - Horizontal start position.
 * \param y1   - Vertical start position.
 * \param x2   - Horizontal end position.
 * \param y2   - Vertical end position.
 * \param col  - Drawing color.
 */
void GlSurface_drawLine(GlSurface surf, Gl2DCoord x1, Gl2DCoord y1, Gl2DCoord x2, Gl2DCoord y2, GlColor col);

/*!
 * \brief Draw a line, checking for each pixel if it's inside the surface.
 *
 * Slow but efficient with small lines near edges (only a small part outside).
 * \param surf - The surface.
 * \param x1   - Horizontal start position.
 * \param y1   - Vertical start position.
 * \param x2   - Horizontal end position.
 * \param y2   - Vertical end position.
 * \param col  - Drawing color.
 */
void GlSurface_drawLineSecure(GlSurface surf, Gl2DCoord x1, Gl2DCoord y1, Gl2DCoord x2, Gl2DCoord y2, GlColor col);

/*!
 * \brief Draw a line, cutting it to fit into the surface
 *
 * Better for wide lines that come across the whole surface and extends very outside.
 * \param surf - The surface
 * \param x1   - Horizontal start position.
 * \param y1   - Vertical start position.
 * \param x2   - Horizontal end position.
 * \param y2   - Vertical end position.
 * \param col  - Drawing color
 */
void GlSurface_drawLineCut(GlSurface surf, Gl2DCoord x1, Gl2DCoord y1, Gl2DCoord x2, Gl2DCoord y2, GlColor col);

/*!
 * \brief Draw a circle, no check.
 *
 * \param surf    - The surface.
 * \param centerx - Horizontal position of center.
 * \param centery - Vertical position of center.
 * \param radius  - Radius of the circle.
 * \param col     - Drawing color.
 */
void GlSurface_drawCircle(GlSurface surf, Gl2DCoord centerx, Gl2DCoord centery, Gl2DSize radius, GlColor col);

/*!
 * \brief Draw a circle, checking for each pixel if it's in the surface.
 *
 * \param surf    - The surface.
 * \param centerx - Horizontal position of center.
 * \param centery - Vertical position of center.
 * \param radius  - Radius of the circle.
 * \param col     - Drawing color.
 */
void GlSurface_drawCircleSecure(GlSurface surf, Gl2DCoord centerx, Gl2DCoord centery, Gl2DSize radius, GlColor col);

/*!
 * \brief Perform a blit of a surface inside another.
 *
 * \param src - Source surface.
 * \param dest - Destination surface.
 * \param srcrect - Rectangle of source that will be copied; if NULL, the entire surface is taken.
 * \param destrect - Rectangle in which source will be copied; if NULL, blit is done in the whole surface from top-left.
 */
void GlSurface_doBlit(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect);
    
/*!
 * \brief This function copies a part of a surface inside another without check.
 *
 * No check is performed, srcrect and destrect must be inside their surface and of the same size.
 * \param src      - Source surface.
 * \param dest     - Destination surface.
 * \param srcrect  - Rectangle of source that will be copied; if NULL, the entire surface is taken.
 * \param destrect - Rectangle in which source will be copied; if NULL, copy is done in the whole surface from top-left.
 */
void GlSurface_copyRect(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect);

/*!
 * \brief This function copies a part of a surface inside another with check.
 *
 * Clipping and size checks are performed before doing the copy.
 * Rects passed aren't modified.
 * \param src      - Source surface.
 * \param dest     - Destination surface.
 * \param srcrect  - Rectangle of source that will be copied; if NULL, the entire surface is taken.
 * \param destrect - Rectangle in which source will be copied; if NULL, copy is done in the whole surface from top-left.
 */
void GlSurface_copyRectSecure(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect);

/*!
 * \brief This function copies a part of a surface inside another, tiling the picture to fill destination rect.
 *
 * This is a secure function (checks like \ref GlSurface_copyRectSecure).
 * Tiling starts from the top-right corner of destrect.
 * \param src      - Source surface.
 * \param dest     - Destination surface.
 * \param srcrect  - Rectangle of source that will be copied; if NULL, the entire surface is taken.
 * \param destrect - Rectangle in which source will be copied; if NULL, copy is done in the whole dest surface from top-left.
 */
void GlSurface_copyRectTiled(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect);

/*!
 * \brief This moves a part of the surface (useful for scrolling).
 *
 * No check is performed, the initial rect must be able to be moved.
 * \param surf - The surface.
 * \param rct  - Rectangle bounding the part.
 * \param dx   - Horizontal move.
 * \param dy   - Vertical move.
 */
void GlSurface_movePart(GlSurface surf, GlRect rct, Gl2DCoord dx, Gl2DCoord dy);

#endif
