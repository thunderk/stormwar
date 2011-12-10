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
 *   Textures for the interface.                                              *
 *   These textures can be tiled and have a border.                           *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GUITEXTURE_H_
#define _SW_GUITEXTURE_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/glsurface.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 *  \brief Private structure of a GuiTexture
 */
typedef struct
{
    GlSurface surf;         /*!< Surface containing the borders and tile for the texture */
    Uint16 border_left;     /*!< Left border size */
    Uint16 border_right;    /*!< Right border size */
    Uint16 border_top;      /*!< Top border size */
    Uint16 border_bottom;   /*!< Bottom border size */
} pv_GuiTexture;

/*!
 * \brief Abstract type for a GUI texture
 *
 * A GUI texture is a texture that can be tiled and have a border
 *  (that can be tiled too)
 */
typedef pv_GuiTexture* GuiTexture;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new GuiTexture
 * 
 * \return The newly allocated texture
 */
GuiTexture GuiTexture_new(void);

/*!
 * \brief Destroy a texture
 *
 * \param tex - The texture
 */
void GuiTexture_del(GuiTexture tex);

/*!
 * \brief Set a texture from a definition variable
 *
 * \param tex  - The texture.
 * \param v - The definition variable.
 */
void GuiTexture_set(GuiTexture tex, Var v);

/*!
 * \brief Fill a rectangle with the texture (borders are drawn and tiling is done)
 * 
 * \param tex - The texture
 * \param dest - Destination surface
 * \param destrect - Rectangle to fill inside the destination surface
 */
void GuiTexture_draw(GuiTexture tex, GlSurface dest, GlRect destrect);

/*!
 * \brief Get the total width of a GuiTexture border (left+right border)
 *
 * \param _tex_ - (GuiTexture) The texture
 * \return (Uint16) Total border width
 */
#define GuiTexture_GETBORDERWIDTH(_tex_) ((_tex_)->border_left+(_tex_)->border_right)

/*!
 * \brief Get the total height of a GuiTexture border (top+bottom border)
 *
 * \param _tex_ - (GuiTexture) The texture
 * \return (Uint16) Total border height
 */
#define GuiTexture_GETBORDERHEIGHT(_tex_) ((_tex_)->border_top+(_tex_)->border_bottom)

/*!
 * \brief Get the top height of a GuiTexture border
 *
 * \param _tex_ - (GuiTexture) The texture
 * \return (Uint16) Top border height
 */
#define GuiTexture_GETBORDERTOP(_tex_) ((_tex_)->border_top)

/*!
 * \brief Get the bottom height of a GuiTexture border
 *
 * \param _tex_ - (GuiTexture) The texture
 * \return (Uint16) Bottom border height
 */
#define GuiTexture_GETBORDERBOTTOM(_tex_) ((_tex_)->border_bottom)

/*!
 * \brief Get the left width of a GuiTexture border
 *
 * \param _tex_ - (GuiTexture) The texture
 * \return (Uint16) Left border width
 */
#define GuiTexture_GETBORDERLEFT(_tex_) ((_tex_)->border_left)

/*!
 * \brief Get the right width of a GuiTexture border
 *
 * \param _tex_ - (GuiTexture) The texture
 * \return (Uint16) Right border width
 */
#define GuiTexture_GETBORDERRIGHT(_tex_) ((_tex_)->border_right)

#endif
