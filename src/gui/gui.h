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
 *   Graphical user interface management                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GUI_H_
#define _SW_GUI_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "gui/guidialog.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Altitude of a gui object. */
typedef Uint16 GuiLayer;

/*! \brief Dock style of a gui object inside another. */
typedef enum
{
    GUIDOCK_TOPLEFT = 7,        /*!< No dock, absolute screen coordinates. */
    GUIDOCK_TOP = 8,            /*!< Dock at top center. */
    GUIDOCK_TOPRIGHT = 9,       /*!< Dock at top right corner. */
    GUIDOCK_LEFT = 4,           /*!< Dock at left center. */
    GUIDOCK_CENTER = 5,         /*!< Dock in the center. */
    GUIDOCK_RIGHT = 6,          /*!< Dock at right center. */
    GUIDOCK_BOTTOMLEFT = 1,     /*!< Dock at bottom left corner. */
    GUIDOCK_BOTTOM = 2,         /*!< Dock at bottom center. */
    GUIDOCK_BOTTOMRIGHT = 3     /*!< Dock at bottom right corner. */
} GuiDock;

/*! \brief Placement of a gui object inside a rectangle. */
typedef struct
{
    GuiDock dock;
    Sint16 xoffset;
    Sint16 yoffset;
    GlRect parentrect;
    GlRect rect;
    GuiLayer layer;
} GuiLayout;

/******************************************************************************
 *############################################################################*
 *#                             Engine functions                             #*
 *############################################################################*
 ******************************************************************************/
/*! \brief Initialize the module */
void guiInit(void);

/*! \brief Uninitialize the module */
void guiUninit(void);

/*!
 * \brief Mark a dialog to delete it later.
 */
void guiDelDialog(GuiDialog dia);

/*!
 * \brief Call this when the screen size changed.
 *
 * Interface elements will be resized or moved if needed.
 * \param w - New width.
 * \param h - New height.
 */
void guiScreenSizeChanged(Uint16 w, Uint16 h);

/******************************************************************************
 *############################################################################*
 *#                            Layout functions                              #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Fill a \ref GuiLayout structure's parameters from a definition variable.
 *
 * This will fill: dock, xoffset, yoffset, layer.
 * \param layout - Pointer to a layout structure.
 * \param v - Definition variable.
 */
void GuiLayout_paramFromVar(GuiLayout* layout, Var v);

/*!
 * \brief Apply a layout.
 *
 * This will compute rect.x, rect.y fields. This assumes parentrect, rect.w and rect.h have already been filled.
 * \param layout - Layout structure.
 */
void GuiLayout_update(GuiLayout* layout);

#endif
