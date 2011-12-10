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
 *   Engine that handles the graphical part of the game                       *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_GRAPHICS_H_
#define _SW_GRAPHICS_GRAPHICS_H_ 1

/*!
 * \file
 * \brief Graphical engine.
 *
 * \par Events received by event collectors
 *  \li GLEVENT_MOUSE not captured by another object.
 *  \li GLEVENT_KEYBOARD not captured by another object.
 *  \li GLEVENT_RESIZE
 *
 * \todo Detailed doc.
 * \todo Better event processing.
 * \todo Thread-safe strategy :
 *          - use asynchronous queues instead of pointer arrays for deletes)
 *          - protect 3d groups with mutex
 * \todo Send state events to objects that have just been inserted.
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
 * \brief Private structure for a GlEventCollector.
 */
typedef struct pv_GlEventCollector pv_GlEventCollector;

/*!
 * \brief Abstract type for an event collector.
 */
typedef pv_GlEventCollector* GlEventCollector;

/*!
 * \brief Rendering modes for 3D groups.
 */
typedef enum
{
    GL3DRENDER_BACKGROUND = 0,    /*!< Background objects. */
    GL3DRENDER_NORMAL = 1,        /*!< Normal objects. */
    GL3DRENDER_RAWBLENDED = 2,    /*!< Transparent objects but not sorted. */
    GL3DRENDER_BLENDED = 3,       /*!< Transparent objects. Objects of such a group will be distance-sorted. */
    GL3DRENDER_GHOST = 4          /*!< Ghost objects. */
} Gl3DRenderMode;

/******************************************************************************
 *                                  Globals                                   *
 ******************************************************************************/
/*! \brief Standard GL3DRENDER_BACKGROUND group. */
extern Gl3DGroup global_groupbackground;
/*! \brief Standard GL3DRENDER_NORMAL group. */
extern Gl3DGroup global_groupnormal;
/*! \brief Standard GL3DRENDER_BLENDED group. */
extern Gl3DGroup global_grouptransparent;
/*! \brief Standard GL3DRENDER_GHOST group. */
extern Gl3DGroup global_groupghost;

/******************************************************************************
 *############################################################################*
 *#                             Engine functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the graphical module (and its sub-modules).
 */
void graphicsInit(void);

/*!
 * \brief Destroy the graphical module (and its sub-modules).
 */
void graphicsUninit(void);

/*!
 * \brief Set the screen video mode.
 *
 * Do this at least one time to be sure everything is set up well.
 * \param width - Hozizontal size required.
 * \param height - Vertical size required.
 * \param fullscreen - Specifies if the video area must be in fullscreen mode or not.
 */
void graphicsSetVideoMode(Gl2DSize width, Gl2DSize height, Bool fullscreen);

/*!
 * \brief Add a new event collector.
 *
 * An event collector will receive all events, except mouse and keyboard ones that
 * were catched by another object.<br>
 * The returned pointer will be passed as external ID to the callback.<br>
 * \param events - Combinaison of events to receive (for example, GLEVENT_MOUSE | GLEVENT_RESIZE).
 * \param callback - Callback to send events through.
 * \return The event collector identifier.
 */
GlEventCollector graphicsAddEventCollector(GlEventType events, GlEventCallback callback);

/*!
 * \brief Delete an event collector.
 *
 * WARNING: This is not immediate. A GLEVENT_DELETE event will be sent through the callback
 * just before the object is really deleted.<br>
 * \param collector - The event collector.
 */
void graphicsDelEventCollector(GlEventCollector collector);

/*!
 * \brief Create a new 3D group.
 *
 * \param mode - Render mode for this group.
 * \return The group identifier.
 */
Gl3DGroup graphicsCreate3DGroup(Gl3DRenderMode mode);

#endif
