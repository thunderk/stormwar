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
 *   Text renderer                                                            *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief Object used for text rendering.
 *
 * A GlTextRender is used to render lines of text on 2D objects.
 * It allows to control rendering options that will be applied to the font.
 *
 * This file also acts like a module to automatically watch over font modifications.
 * There is also a security system to be sure not to draw offlimits (which could happen
 * if a font is modified or deleted during a rendering).
 *
 * The renderer renders the text until a breaking event :
 * \li A new line character.
 * \li A carriage return character.
 * \li The end of the allocated graphical area (same result as a new line character).
 * \li The end of the text.
 *
 * When the rendering is broken by such an event, the renderer returns a GlTextRenderStatus object
 * to let the caller know of the breaking event and the actually rendered size.
 * Calling the rendering function again will continue the rendering where it broke.
 *
 * This can be used like in this example to render a multiline string named \a text in a surface named \a surf :
 * \code
 *    GlTextRenderStatus status;
 *    GlTextRender tr;
 *    Sint16 posy = 0;
 *
 *    tr = GlTextRender_new();
 *    GlTextRender_setFont(tr, ...);
 *    GlTextRender_setOptions(tr, ...);
 *    GlTextRender_setWidthLimit(tr, GlSurface_getWidth(surf));
 *
 *    do
 *    {
 *        status = GlTextRender_render(tr, text, surf, 0, posy, 0);
 *        if (status.breakevent == GLTEXTRENDER_NEWLINE)
 *            posy += status.height;
 *    } while (status.breakevent != GLTEXTRENDER_END);
 *
 *    GlTextRender_del(tr);
 * \endcode
 * This example supposes that the text will fit in the surface vertically. Multiline rendering is automatically done
 * because a \a GLTEXTRENDER_NEWLINE is returned either when the string contains a new line symbol or when the end of the
 * surface is reached.
 */

#ifndef _SW_GRAPHICS_GLTEXTRENDER_H_
#define _SW_GRAPHICS_GLTEXTRENDER_H_ 1

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
 * \brief Private structure for a GlTextRender.
 */
typedef struct pv_GlTextRender pv_GlTextRender;

/*!
 * \brief Abstract type for a text renderer.
 */
typedef pv_GlTextRender* GlTextRender;

/*!
 * \brief Event that can break a text rendering.
 */
typedef enum
{
    GLTEXTRENDER_END,       /*!< The end of the text has been reached. */
    GLTEXTRENDER_NEWLINE,   /*!< A 'new line' character or the end the allocated area has been encountered. */
    GLTEXTRENDER_RETURN     /*!< A 'carriage return' character has been encountered. */
} GlTextRenderEvent;

/*!
 * \brief Type returned by the renderer to give information after rendering a chunk.
 */
typedef struct
{
    GlTextRenderEvent breakevent;   /*!< Event that stopped the rendering. */
    Gl2DSize width;                 /*!< Width used to render the chunk. */
    Gl2DSize height;                /*!< Height used to render the chunk. */
} GlTextRenderStatus;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new text renderer.
 *
 * \return The newly created text renderer.
 */
GlTextRender GlTextRender_new();

/*!
 * \brief Delete a text renderer.
 *
 * \param tr - The text renderer.
 */
void GlTextRender_del(GlTextRender tr);

/*!
 * \brief Set the font that will be used for drawing.
 *
 * This may change rendering sizes.<br>
 * If the font is changed or deleted during the renderer's lifetime, it will be automatically handled.
 * \param tr - The text renderer.
 * \param fontname - Name of the font to use, will be copied.
 */
void GlTextRender_setFont(GlTextRender tr, String fontname);

/*!
 * \brief Set rendering options.
 *
 * This may change rendering sizes.
 * \param tr - The text renderer.
 * \param monospace - TRUE if the font will be used in monospace mode.
 */
void GlTextRender_setOptions(GlTextRender tr, Bool monospace);

/*!
 * \brief Set rendering options from a definition variable.
 *
 * This may change rendering sizes.<br>
 * See GlTextRender_setOptions to know about rendering options.
 * This will also change the rendering color.
 * \param tr - The text renderer.
 * \param vopt - The definition variable.
 */
void GlTextRender_setOptionsFromVar(GlTextRender tr, Var vopt);

/*!
 * \brief Set the rendering color.
 *
 * This will override the color specified in options read from a variable.
 * \param tr - The text renderer.
 * \param col - New rendering color.
 */
void GlTextRender_setColor(GlTextRender tr, GlColor col);

/*!
 * \brief Set the width limit (allocated width for rendering).
 *
 * This can be changed even between chunks of a same text.<br>
 * The renderer won't draw anything beyond this limit.<br>
 * This limit will be used to break the text in chunks for multi-line rendering.
 * \param tr - The text renderer.
 * \param widthlimit - The width limit, 0 means no limit.
 */
void GlTextRender_setWidthLimit(GlTextRender tr, Gl2DSize widthlimit);

/*!
 * \brief Get the height that would be used to render a text line, given the previouly defined font and options.
 *
 * \param tr - The text renderer.
 * \return Height of a text line.
 */
Uint16 GlTextRender_getLineHeight(GlTextRender tr);

/*!
 * \brief Guess the size that will be required to draw a text, given the previouly defined font and options.
 *
 * This will take care of the width limit previously set, but contrary to GlTextRender_render, there is no
 * surface that will physically limit the text.
 * \param tr - The text renderer.
 * \param text - Text to process.
 * \param r_width - Result width.
 * \param r_height - Result height.
 */
void GlTextRender_guessSize(GlTextRender tr, String text, Gl2DSize* r_width, Gl2DSize* r_height);

/*!
 * \brief Render a chunk of text.
 *
 * This function can be called several times with the same String to render successive chunks
 * (pay attention the comparison is made on the String pointer, not on the text).
 * It returns when a breaking event has been encountered (see \c GlTextRenderEvent enumeration).
 * \param tr - The text renderer.
 * \param text - Text to render.
 * \param surf - Surface where the rendering will be done.
 * \param posx - X starting position.
 * \param posy - Y starting position.
 * \param heightlimit - Height limit beyond which no drawing will be done, 0 for no limit; provided for security purpose only.
 */
GlTextRenderStatus GlTextRender_render(GlTextRender tr, String text, GlSurface surf, Gl2DCoord posx, Gl2DCoord posy, Gl2DSize heightlimit);

/*!
 * \brief Render a text directly inside a rectangle.
 *
 * Ideally, the rectangle used here is of the size given by the \ref GlTextRender_guessSize function.
 * \param tr - The text renderer.
 * \param text - Text to render.
 * \param surf - Surface where the rendering will be done.
 * \param rect - Rectangle of the surface where the rendering will be done, not checked.
 * \param offsetx - X offset from the rectangle's top-left corner.
 * \param offsety - Y offset from the rectangle's top-left corner.
 */
void GlTextRender_directRender(GlTextRender tr, String text, GlSurface surf, GlRect rect, Gl2DCoord offsetx, Gl2DCoord offsety);

#endif
