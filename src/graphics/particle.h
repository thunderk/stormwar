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
 *   Graphical particles                                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_PARTICLE_H_
#define _SW_GRAPHICS_PARTICLE_H_ 1

/*!
 * \file
 * \brief 3D particle objects.
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
 * \brief Private structure of a particle.
 */
typedef struct pv_Particle pv_Particle;

/*!
 * \brief Abstract type for a graphical particle.
 */
typedef pv_Particle* Particle;

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new particle.
 *
 * \return The newly allocated particle.
 */
Particle Particle_new(void);

/*!
 * \brief Delete a particle.
 *
 * \param particle - The particle.
 */
void Particle_del(Particle particle);

/*!
 * \brief Set a particle visibility.
 *
 * \param particle - The particle.
 * \param show - Show the particle or not.
 */
void Particle_show(Particle particle, Bool show);

/*!
 * \brief Set the texture of a particle
 *
 * \param particle - The particle.
 * \param texname - Name of the texture to use.
 */
void Particle_setTex(Particle particle, String texname);

/*!
 * \brief Set the size of a particle
 *
 * \param particle - The particle.
 * \param sizex - X size of the particle.
 * \param sizey - Y size of the particle.
 */
void Particle_setSize(Particle particle, Gl3DCoord sizex, Gl3DCoord sizey);

/*!
 * \brief Set the position of a particle.
 *
 * \param particle - The particle.
 * \param x - New X coordinate.
 * \param y - New Y coordinate.
 * \param z - New Z coordinate.
 */
void Particle_setPos(Particle particle, Gl3DCoord x, Gl3DCoord y, Gl3DCoord z);

#endif
