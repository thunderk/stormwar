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
 *   Part of a GlMesh                                                         *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_IMPL_GLMESHPART_H_
#define _SW_GRAPHICS_IMPL_GLMESHPART_H_ 1

/*!
 * \file
 * \brief Part of a GlMesh.
 *
 * A GlMeshPart is a set of triangles or quads that share the same properties
 * (texture, material...).
 *
 * \todo Replace use of the shell error stack.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "graphics/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure for a GlMeshPart.
 */
typedef struct pv_GlMeshPart pv_GlMeshPart;

/*!
 * \brief Abstract type for sub-element of a GlMesh.
 */
typedef pv_GlMeshPart* GlMeshPart;

/******************************************************************************
 *############################################################################*
 *#                              GlMesh functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new mesh part from a definition variable.
 *
 * \return The newly allocated mesh part.
 */
GlMeshPart GlMeshPart_new(Var var);

/*!
 * \brief Destroy a mesh part.
 *
 * \param part - The mesh part.
 */
void GlMeshPart_del(GlMeshPart part);

/*!
 * \brief Draw a mesh part in OpenGL context.
 *
 * The camera must already be set for this part.
 * \mainthread
 * \param part - The mesh part.
 */
void GlMeshPart_draw(GlMeshPart part);

#endif
