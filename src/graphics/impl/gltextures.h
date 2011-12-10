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
 *   OpenGL texture management                                                *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_IMPL_GLTEXTURES_H_
#define _SW_GRAPHICS_IMPL_GLTEXTURES_H_ 1

/*!
 * \file
 * \brief Management of OpenGL textures.
 *
 * These functions should only be called by main thread.
 *
 * There are two kinds of textures:
 *  \li Static textures, that are loaded from the mod.
 *  \li Linked textures, that reflect a GlSurface content.
 *
 * Static textures can be retrived by their name.
 *
 * Linked textures can need to be split in parts (because some OpenGL
 * implementation limit the texture size) for exactness. They are
 * used in 2d objects.
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
/*! \brief Private static texture structure. */
typedef struct pv_GlStaticTexture pv_GlStaticTexture;

/*! \brief Abstract type for a static texture. */
typedef pv_GlStaticTexture* GlStaticTexture;

/*! \brief Private linked texture structure. */
typedef struct pv_GlLinkedTexture pv_GlLinkedTexture;

/*! \brief Abstract type for a linked texture. */
typedef pv_GlLinkedTexture* GlLinkedTexture;

/*! \brief Standard texture coordinates. */
typedef float TexCoord;

/*! \brief Part rendering information. */
typedef struct
{
    GlRect rct;         /* Rectangle of the part inside the whole texture. */
    TexCoord* texcoords;/* Array of 8 texture coordinates (4 times {x,y}) given in clockwise order. */
} TexRenderInfo;

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
/*!
 * \brief Usable void static texture.
 */
extern GlStaticTexture GlStaticTexture_NULL;

/******************************************************************************
 *############################################################################*
 *#                              Module functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the textures manager.
 */
void gltexturesInit(void);

/*!
 * \brief Destroy the textures manager.
 */
void gltexturesUninit(void);

/*!
 * \brief Load a set of static textures from a definition variable.
 *
 * \param v - The definition variable.
 */
void gltexturesLoadSet(Var v);

/*!
 * \brief Get a static texture by its name.
 *
 * \param name - The texture name.
 * \return The texture, GlStaticTexture_NULL if not found.
 */
GlStaticTexture gltexturesGet(String name);

/*!
 * \brief Check that OpenGL hasn't discarded its textures.
 *
 * Call this when the screen mode or the window state changed. This will reload
 * the textures if needed.
 */
void gltexturesCheck(void);

/*!
 * \brief Change the texture filter.
 *
 * \param filter - The new texture filter.
 */
void gltexturesSetFilter(GlTextureFilter filter);

/******************************************************************************
 *############################################################################*
 *#                         Static textures functions                        #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Place a static texture in OpenGL context.
 *
 * \param tex - The texture.
 */
void GlStaticTexture_use(GlStaticTexture tex);

/******************************************************************************
 *############################################################################*
 *#                         Linked textures functions                        #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new linked texture.
 *
 * \param surf - The surface to link.
 * \param nbparts - Returned number of parts.
 * \return The new texture.
 */
GlLinkedTexture GlLinkedTexture_new(GlSurface surf, Uint16* nbparts);

/*!
 * \brief Destroy a linked texture.
 *
 * \param tex - The texture.
 */
void GlLinkedTexture_del(GlLinkedTexture tex);

/*!
 * \brief Place a linked texture in OpenGL context.
 *
 * Call this once before drawing each part (part counting is done internally).
 * \param tex - The texture.
 * \return Rendering information for the part. \readonly
 */
TexRenderInfo* GlLinkedTexture_use(GlLinkedTexture tex);

/*!
 * \brief Link a surface to a texture.
 *
 * The previous link will be replaced by this one.
 * Recall this whenever the surface's size changed.
 * \param tex - The texture.
 * \param surf - The surface.
 * \return The number of parts.
 */
Uint16 GlLinkedTexture_linkToSurf(GlLinkedTexture tex, GlSurface surf);

/*!
 * \brief Update a link with the surface.
 *
 * Call this when the surface content has changed and need to be updated in OpenGL memory.
 * \param tex - The linked texture.
 */
void GlLinkedTexture_updateLink(GlLinkedTexture tex);

#endif
