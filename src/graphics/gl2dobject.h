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
 *   Graphical 2d object                                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GL2DOBJECT_H_
#define _SW_GRAPHICS_GL2DOBJECT_H_ 1

/*!
 * \file
 * \brief Graphical 2D objects.
 *
 * A 2D object has two identifiers :
 *  \li An internal ID (\ref Gl2DObject) that is used by the engine.
 *  \li An external ID (\ref GlExtID) that can be used by callback receivers to identify
 *      the object.
 *
 * \par Events catched.
 *  (\ref GlEventType)
 *  \li GLEVENT_MOUSE
 *  \li GLEVENT_KEYBOARD
 *  \li GLEVENT_RESIZE
 *  \li GLEVENT_DELETE
 *  \li GLEVENT_DRAW
 *
 * \todo Non-squared OpenGL textures.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
/*! \brief (\ref Gl2DCoord) Maximal altitude for a \ref Gl2DObject. */
#define GL2D_HIGHEST 30000

/*! \brief (\ref Gl2DCoord) Minimal altitude for a \ref Gl2DObject. */
#define GL2D_LOWEST 0

/******************************************************************************
 *############################################################################*
 *#                            Gl2DObject functions                          #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a graphical 2d object.
 *
 * \param surf - Surface containing the graphical data for this object.
 * \param extid - External ID.
 * \param callback - Callback function for catched events.
 * \return The newly allocated object.
 */
Gl2DObject Gl2DObject_new(GlSurface surf, GlExtID extid, GlEventCallback callback);

/*!
 * \brief Destroy a Gl2DObject.
 *
 * WARNING: This is not immediate. A GLEVENT_DELETE event will be sent through the callback
 * just before the object is really deleted. So don't delete the linked surface before receiving
 * this event.<br>
 * This will NOT destroy the linked surface.
 * \param obj - The object.
 */
void Gl2DObject_del(Gl2DObject obj);

/*!
 * \brief Set the external id associated with an object.
 *
 * The caller must ensure the management of such external IDs. The object pointer itself (internal ID)
 * can be used as external ID (e.g Gl2DObject_setExtID(obj, obj)).
 * \param obj - The object.
 * \param extid - External ID to help identify the object.
 */
void Gl2DObject_setExtID(Gl2DObject obj, GlExtID extid);

/*!
 * \brief Set an object's visibility.
 *
 * \param obj - The object.
 * \param visible - TRUE for visible, FALSE for hidden.
 */
void Gl2DObject_show(Gl2DObject obj, Bool visible);

/*!
 * \brief Redraw an object.
 *
 * Call this when the graphical data of an object changed.
 * \param obj - The object.
 */
void Gl2DObject_redraw(Gl2DObject obj);

/*!
 * \brief Relink an object to a graphical surface.
 *
 * Call this when graphical data's location changed or when the linked surface was resized.
 * \param obj - The object.
 * \param surf - Surface containing graphical data for this object.
 */
void Gl2DObject_relink(Gl2DObject obj, GlSurface surf);

/*!
 * \brief Set the global color tone of an object.
 *
 * This can be used for transparent fadings.
 * \param obj - The object.
 * \param col - The new color.
 * \param duration - The duration of the transition effect in milliseconds.
 */
void Gl2DObject_setColor(Gl2DObject obj, GlColorRGBA col, CoreTime duration);

/*!
 * \brief Set the position of an object.
 *
 * Position is given relatively to the screen.
 * \param obj - The object.
 * \param x - Horizontal position.
 * \param y - Vertical position.
 * \param duration - Duration of the move in milliseconds.
 */
void Gl2DObject_setPos(Gl2DObject obj, Gl2DCoord x, Gl2DCoord y, CoreTime duration);

/*!
 * \brief Set the altitude of an object.
 *
 * \param obj - The object.
 * \param z - New altitude.
 */
void Gl2DObject_setAlt(Gl2DObject obj, Gl2DSize z);

/*!
 * \brief Get the surface where the drawing have to be done.
 *
 * \param obj - The object.
 * \return The surface containing the graphical data for the object.
 */
GlSurface Gl2DObject_getDrawingSurface(Gl2DObject obj);

/*!
 * \brief Tell if an object is visible or hidden.
 *
 * \param obj - The object.
 * \return Object's visibility.
 */
Bool Gl2DObject_isVisible(Gl2DObject obj);

/*!
 * \brief Get the position and size of an object.
 *
 * \param obj - The object.
 * \param rect - Pointer to a valid rectangle structure that will be filled.
 */
void Gl2DObject_getRect(Gl2DObject obj, GlRect* rect);

/*!
 * \brief Get the X position of an object.
 *
 * \param obj - The object.
 * \return The X coordinate.
 */
Gl2DCoord Gl2DObject_getX(Gl2DObject obj);

/*!
 * \brief Get the Y position of an object.
 *
 * \param obj - The object.
 * \return The Y coordinate.
 */
Gl2DCoord Gl2DObject_getY(Gl2DObject obj);

/*!
 * \brief Get the altitude of an object.
 *
 * \param obj - The object.
 * \return The altitude.
 */
Gl2DCoord Gl2DObject_getAlt(Gl2DObject obj);

/*!
 * \brief Get an object's width.
 *
 * \param obj - The object.
 * \return The width.
 */
Gl2DSize Gl2DObject_getWidth(Gl2DObject obj);

/*!
 * \brief Get an object's height.
 *
 * \param obj - The object.
 * \return The height.
 */
Gl2DSize Gl2DObject_getHeight(Gl2DObject obj);

#endif
