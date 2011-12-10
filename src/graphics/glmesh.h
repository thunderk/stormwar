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
 *   Graphical appearance and behavior of a 3d object                         *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GLMESH_H_
#define _SW_GRAPHICS_GLMESH_H_ 1

/*!
 * \file
 * \brief Graphical datas for a 3D object.
 *
 * A mesh contains:
 *  \li One or several parts (that contains surfaces definition).
 *  \li Animation datas to control these parts.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                              GlMesh functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new mesh from a definition variable.
 *
 * \param v - The variable, can be NULL to clear the mesh.
 * \return The newly allocated mesh.
 */
GlMesh GlMesh_new(Var v);

/*!
 * \brief Destroy a mesh.
 *
 * \param mesh - The mesh.
 */
void GlMesh_del(GlMesh mesh);

#endif
