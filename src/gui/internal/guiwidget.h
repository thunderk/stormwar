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
 *   GUI object                                                               *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This file defines common GUI object's behavior.
 *
 * A GuiWidget is an object used to manage graphical interface's hierarchy
 * and events.
 *
 * Each GUI object will have an associated GuiWidget. The widget will manage
 * the hierarchy tree by linking itself to its children. The real GUI object will
 * be called the 'holder' of this widget.
 *
 * There are 2 kinds of events that can run through the widget tree:
 * \li GlEvent (graphical events), that can be passed 'downwards' from parent to children (the top-parent is the screen).
 * \li GuiEvent (GUI events), that can be passed 'upwards' from a child to its parent.
 *
 * GlEvents that can be forwarded to the holder are:
 * \li EVENT_MOUSE (forwarding to children and button holding are automatically performed by the widget)
 * \li EVENT_KEY (forwarding to children is automatically performed by the widget)
 * \li EVENT_REALLOC (when the drawing surface changed or when the screen size changed for a top-level widget)
 *
 * When an event is received by the widget, it warns its holder by the mean of the defined callbacks then treat the event itself.
 *
 * A top-level widget is a widget whose parent is the screen. It will receive screen events (inputs, resize...).
 *
 * \warning In a down event callback, throwing an event upwards is the last thing that sould be done
 * because such an event can lead to the widget's destruction by a parent.
 */

#ifndef _SW_GUIWIDGET_H_
#define _SW_GUIWIDGET_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief GUI events that can be produced by a child for its parent. */
typedef enum
{
    GUIEVENT_NONE,      /*!< No event. Only used internally. */
    GUIEVENT_CLICKED,   /*!< Event produced by a child when it is clicked on. */
    GUIEVENT_CHANGED,   /*!< Event produced by a child when its value has been modified. */
    GUIEVENT_VALIDATE   /*!< Event produced by a child when its value has been validated. */
} GuiEvent;

/*! \brief Private structure for a GuiWidget. */
typedef struct pv_GuiWidget pv_GuiWidget;

/*! \brief Abstract type containing common GUI properties for any object. */
typedef pv_GuiWidget* GuiWidget;

/*! \brief Function called for a GUI event that goes up (received from a child). */
typedef void (*GuiEventCallback) (GuiWidget widget, unsigned int childnb, GuiEvent event);

/******************************************************************************
 *############################################################################*
 *#                             Widget functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the widget system.
 */
void guiwidgetInit(void);

/*!
 * \brief Destroy the widget system.
 */
void guiwidgetUninit(void);

/*!
 * \brief Create a new GUI widget.
 *
 * Pay attention: don't throw upwards events (such as redraw) before linking this widget to its parent.
 * \param parent - Parent widget, NULL if the created widget will be a top-level one (the parent is the screen, the widget will be a Gl2DObject).
 * \param holder - The holder.
 * \param tooltip - Tooltip to display, will be copied, NULL if not.
 * \param container - Specifies if the widget is just a container (no drawing surface); ignored for a top-level widget.
 * \param upcallback - Function that will be called when an event is received from a child, can be NULL if the widget has no child.
 * \param downcallback - Function that will be called when an event is received from the parent.
 * \return The newly allocated widget.
 */
GuiWidget GuiWidget_new(GuiWidget parent, void* holder, const char* tooltip, Bool container, GuiEventCallback upcallback, GlEventCallback downcallback);

/*!
 * \brief Delete a widget.
 *
 * This doesn't delete children widget (holders are supposed to do those things themselves).
 * \param widget - The widget.
 */
void GuiWidget_del(GuiWidget widget);

/*!
 * \brief Set a top-level widget's altitude, position and size.
 *
 * \param widget - A top-level widget.
 * \param alt - New altitude (between \ref GL2D_LOWEST and \ref GL2D_HIGHEST, or -1 for automatic management).
 * \param rct - New position and size.
 */
void GuiWidget_topLevelSet(GuiWidget widget, Sint16 alt, GlRect rct);

/*!
 * \brief Get a widget's holder.
 *
 * \param widget - A widget.
 * \return The holder.
 */
void* GuiWidget_getHolder(GuiWidget widget);

/*!
 * \brief Show/hide a widget.
 *
 * \param widget - The widget.
 * \param show - TRUE to show the widget, FALSE to hide it.
 */
void GuiWidget_show(GuiWidget widget, Bool show);

/*!
 * \brief Set the number of children of a widget.
 *
 * A child can stay 'undefined' (not set by the function GuiWidget_setChild) without error.
 * A defined child must remain available through the parent's lifetime.
 * As many children as possible will remain the same.
 * \param widget - The widget.
 * \param nb - Number of children, can be 0.
 */
void GuiWidget_setNbChild(GuiWidget widget, unsigned int nb);

/*!
 * \brief Define a child.
 *
 * \param widget - The widget.
 * \param childnb - Child number.
 * \param child - Child to attach, NULL to delete the link.
 */
void GuiWidget_setChild(GuiWidget widget, unsigned int childnb, GuiWidget child);

/*!
 * \brief Allocate a graphical space for a child inside the widget's rectangle.
 *
 * \param widget - The widget.
 * \param childnb - Child number.
 * \param rect - Allocated rectangle, relative to the widget's own rectangle.
 */
void GuiWidget_setChildRect(GuiWidget widget, unsigned int childnb, GlRect rect);

/*!
 * \brief Throw a GuiEvent.
 *
 * This will handle automatically those things:
 * \li Forwarding event to the parent if needed.
 * \li Forwarding event to the children and redrawing for GUIEVENT_REDRAW.
 * \param widget - The widget.
 * \param event - The event.
 */
void GuiWidget_throwEvent(GuiWidget widget, GuiEvent event);

/*!
 * \brief Redraw a widget inside its parent.
 *
 * Only call this when the drawing surface has been modified.
 * This function only changes a state of the widget ; the real redrawing is performed on graphical frame drawing.
 * \param widget - The widget to redraw.
 */
void GuiWidget_redraw(GuiWidget widget);

/*!
 * \brief Get the surface into which the graphical data must be drawn.
 *
 * Call this when the allocated area changed (on the corresponding GlEvent).
 * \param widget - The widget.
 * \return The drawing surface, NULL if the widget is a virtual container.
 */
GlSurface GuiWidget_getDrawingSurface(GuiWidget widget);

#endif
