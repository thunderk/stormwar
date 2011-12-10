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
 *   Graphical 3d object                                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GL3DOBJECT_H_
#define _SW_GRAPHICS_GL3DOBJECT_H_ 1

/*!
 * \file
 * \brief Graphical 3D objects.
 *
 * \todo Animation speed.
 *
 * A 3D object has two identifiers :
 *  \li An internal ID (\ref Gl3DObject) that is used by the engine.
 *  \li An external ID (\ref GlExtID) that can be used by callback receivers to identify
 *      the object and/or event.
 *
 * \par Events catched.
 *  (\ref GlEventType)
 *  \li GLEVENT_DELETE
 *  \li GLEVENT_DRAW
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                            Gl3DObject functions                          #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a graphical 3D object.
 *
 * \param extid - External pointer to identify the object (will be used in event callbacks).
 * \param group - Group in which the object will be added (the group must exist).
 * \param callback - Callback function for catched events.
 * \return The newly allocated object.
 */
Gl3DObject Gl3DObject_new(GlExtID extid, Gl3DGroup group, GlEventCallback callback);

/*!
 * \brief Destroy a Gl3DObject.
 *
 * WARNING: This is not immediate. A GLEVENT_DELETE event will be sent through the callback
 * just before the object is really deleted. So don't delete the mesh before receiving
 * this event.<br>
 * This will NOT destroy associated data (external ID and mesh).
 * \param obj - The object.
 */
void Gl3DObject_del(Gl3DObject obj);

/*!
 * \brief Set the external id associated with an object.
 *
 * The caller must ensure the management of such external IDs. The object pointer itself (internal ID)
 * can be used as external ID (e.g Gl3DObject_setExtID(obj, obj)).
 * \param obj - The object.
 * \param extid - External ID to help identify the object.
 */
void Gl3DObject_setExtID(Gl3DObject obj, GlExtID extid);

/*!
 * \brief Set an object's visibility.
 *
 * \param obj - The object.
 * \param visible - TRUE for visible, FALSE for hidden.
 */
void Gl3DObject_show(Gl3DObject obj, Bool visible);

/*!
 * \brief Set the 3D aspect of an object.
 *
 * Do this whenever the mesh changed.
 * \param obj - The object.
 * \param mesh - The mesh.
 */
void Gl3DObject_setMesh(Gl3DObject obj, GlMesh mesh);

/*!
 * \brief Set the global color tone of an object.
 *
 * Alpha value will be used only in blending mode. RGB components will be used only in ghost mode.
 * \param obj - The object.
 * \param col - The new color.
 * \param duration - The duration of the transition effect in milliseconds.
 */
void Gl3DObject_setColor(Gl3DObject obj, GlColorRGBA col, CoreTime duration);

/*!
 * \brief Choose an animation for an object.
 *
 * \param obj - The object.
 * \param anim - Animation name.
 * \param time - Offset time inside the animation.
 * \param repeats - Number of times to repeat the animation, 0 for an infinite loop.
 */
void Gl3DObject_setAnim(Gl3DObject obj, String anim, CoreTime time, unsigned int repeats);

/*!
 * \brief Set the position of an object.
 *
 * Position is given relatively to the world.
 * \param obj - The object.
 * \param x - New X position.
 * \param y - New Y position (altitude).
 * \param z - New Z position.
 * \param duration - Duration of the movement in milliseconds.
 */
void Gl3DObject_setPos(Gl3DObject obj, Gl3DCoord x, Gl3DCoord y, Gl3DCoord z, CoreTime duration);

/*!
 * \brief Set the orientation of an object.
 *
 * This will pass (returned external ID, GLEVENT_TIMER) to the callback when the move ends.
 * \param obj - The object.
 * \param angh - New horizontal angle.
 * \param angv - New vertical angle.
 * \param duration - Duration of the rotation in milliseconds.
 */
void Gl3DObject_setAngle(Gl3DObject obj, Gl3DCoord angh, Gl3DCoord angv, CoreTime duration);

/*!
 * \brief Tell if an object is visible or hidden.
 *
 * \param obj - The object.
 * \return Object's visibility.
 */
Bool Gl3DObject_isVisible(Gl3DObject obj);

/*!
 * \brief Get the mesh associated with an object.
 *
 * \param obj - The object.
 * \return Object's mesh.
 */
GlMesh Gl3DObject_getMesh(Gl3DObject obj);

/*!
 * \brief Get the group of an object.
 *
 * \param obj - The object.
 * \return Object's group.
 */
Gl3DGroup Gl3DObject_getGroup(Gl3DObject obj);

/*!
 * \brief Get the position of an object.
 *
 * \param obj - The object.
 * \param x - Pointer to a Gl3DCoord to fill with a X coordinate.
 * \param y - Pointer to a Gl3DCoord to fill with a Y coordinate.
 * \param z - Pointer to a Gl3DCoord to fill with a Z coordinate.
 */
void Gl3DObject_getPos(Gl3DObject obj, Gl3DCoord* x, Gl3DCoord* y, Gl3DCoord* z);

/*!
 * \brief Get the screen rectangle occupied by the object's control points.
 *
 * \param obj - The object.
 * \return The occupied rectangle, in screen coordinates.
 */
GlRect Gl3DObject_getRect(Gl3DObject obj);

/*!
 * \brief Get the X position of an object.
 *
 * \param obj - The object.
 * \return The X coordinate.
 */
Gl3DCoord Gl3DObject_getX(Gl3DObject obj);

/*!
 * \brief Get the Y position of an object.
 *
 * \param obj - The object.
 * \return The Y coordinate.
 */
Gl3DCoord Gl3DObject_getY(Gl3DObject obj);

/*!
 * \brief Get the Z position of an object.
 *
 * \param obj - The object.
 * \return The Z coordinate.
 */
Gl3DCoord Gl3DObject_getZ(Gl3DObject obj);

/*!
 * \brief Get the horizontal angle of an object.
 *
 * \param obj - The object.
 * \return The horizontal angle.
 */
Gl3DCoord Gl3DObject_getAngh(Gl3DObject obj);

/*!
 * \brief Get the vertical angle of an object.
 *
 * \param obj - The object.
 * \return The vertical angle.
 */
Gl3DCoord Gl3DObject_getAngv(Gl3DObject obj);

#endif
