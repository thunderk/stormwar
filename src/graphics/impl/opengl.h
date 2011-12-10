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
 *   OpenGL management                                                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_IMPL_OPENGL_H_
#define _SW_GRAPHICS_IMPL_OPENGL_H_ 1

/*!
 * \file
 * \brief OpenGL management.
 *
 * \todo Rewrite this.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "SDL_opengl.h"
#include "core/types.h"

/******************************************************************************
 *                              Global variables                              *
 ******************************************************************************/
/*!
 * \brief Screen ratio.
 *
 * Pay attention, this value will change very quickly (twice a frame) to adapt
 * itself to the current drawing mode (2d or 3d).

 */
extern float global_screenratio;

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef GLuint OpenGLTexture;

/******************************************************************************
 *############################################################################*
 *#                             OpenGL functions                             #*
 *############################################################################*
 ******************************************************************************/

/*!
 * \brief Initialize the openGL module.
 */
void openglInit(void);

/*!
 * \brief Uninitialize openGL module.
 */
void openglUninit(void);

/*!
 * \brief Set OpenGL parameters from a definition file.
 */
void openglSet(Var v);

/*!
 * \brief Adjust the OpenGL viewport to match given screen size.
 *
 * Call this whenever the screen size has been modified.
 * \param width - Screen's new width
 * \param height - Screen's new height
 */
void openglScreenSize(Uint16 width, Uint16 height);

/*!
 * \brief Prepare for background rendering.
 */
void openglStep3DBackground(void);

/*!
 * \brief Prepare for 3D objects rendering.
 */
void openglStep3DObjects(void);

/*!
 * \brief Prepare for ghost 3D objects rendering.
 */
void openglStep3DObjectsGhost(void);

/*!
 * \brief Prepare for particles rendering.
 */
void openglStepParticles(void);

/*!
 * \brief Prepare for 2D objects rendering.
 */
void openglStep2D(void);

#ifdef DEBUG_OPENGL
    void pv_openglResetCount();
    void pv_openglCount(unsigned int i);
    #define openglResetCount() pv_openglResetCount()
    #define openglCount(_i_) pv_openglCount(_i_)
#else
    #define openglResetCount()
    #define openglCount(_i_)
#endif

#endif
