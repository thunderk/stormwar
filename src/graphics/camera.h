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
 *   Camera for the 3D part of the scene                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_CAMERA_H_
#define _SW_GRAPHICS_CAMERA_H_ 1

/*!
 * \file
 * \brief 3D camera control.
 *
 * \todo Core module with data, ressources...
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"

/******************************************************************************
 *############################################################################*
 *#                              Camera functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Set the position looked at.
 *
 * \param x - X coordinate.
 * \param y - Y coordinate.
 * \param z - Z coordinate.
 */
void cameraSetPos(Gl3DCoord x, Gl3DCoord y, Gl3DCoord z);

/*!
 * \brief Scroll the camera.
 *
 * This will actually move the position looked at and so the camera's virtual position.
 * The scrolling will be performed relatively to the current orientation.
 * \param right - Lateral movement.
 * \param front - Forward-backward movement.
 */
void cameraScroll(Gl3DCoord right, Gl3DCoord front);

/*!
 * \brief Set the angles made by the viewing line.
 *
 * \param angh - Horizontal angle made by the camera position with the looked-at position.
 * \param angv - Vectical angle made by the camera position with the ground.
 */
void cameraSetAngle(Gl3DCoord angh, Gl3DCoord angv);

/*!
 * \brief Make the camera turn on itself.
 *
 * The camera do a panoramic turn, changing its horizontal angle and the looked-at position.
 * \param ang - Panoramic angle modification.
 */
void cameraPan(Gl3DCoord ang);

/*!
 * \brief Rotate the camera around the position looked at.
 *
 * \param angh - Horizontal rotation angle.
 * \param angv - Vertical rotation angle.
 */
void cameraRotateAround(Gl3DCoord angh, Gl3DCoord angv);

/*!
 * \brief Set the zoom factor.
 *
 * The zoom factor is used to determine the distance between the camera and the looked-at position.
 * A zoom of 0.0 would mean the camera being at the looked-at position.
 * \param zoom - Zoom factor.
 */
void cameraSetZoom(Gl3DCoord zoom);

/*!
 * \brief Modify the zoom factor.
 *
 * \param dzoom - Zoom factor modification.
 */
void cameraZoom(Gl3DCoord dzoom);

/*!
 * \brief Get the distance between the camera and the focused point.
 *
 * \return Camera distance.
 */
Gl3DCoord cameraGetDist(void);

/*!
 * \brief Get the camera's real position.
 */
void cameraGetRealPos(Gl3DCoord* x, Gl3DCoord* y, Gl3DCoord* z);

#endif
