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
 *   Object for menus that popup on the screen and menus manager              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GUIPOPUPMENU_H_
#define _SW_GUIPOPUPMENU_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "graphics/glrect.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Specifies where a menu is placed, relatively to its parent.
 */
typedef enum
{
    MENU_RIGHT,         /*!< Child menu placed on its parent's right side (or left if no room) */
    MENU_BELOW          /*!< Child menu placed below its parent (or above if no room) */
} MenuLayout;

/*!
 * \brief Private structure of a GuiPopupMenu
 */
typedef struct pv_GuiPopupMenu pv_GuiPopupMenu;

/*!
 * \brief Abstract type for a popup menu
 */
typedef pv_GuiPopupMenu* GuiPopupMenu;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the menus manager.
 */
void menusInit(void);

/*!
 * \brief Destroy the menus manager.
 */
void menusUninit(void);

/*!
 * \brief Set the menus from a definition variable.
 *
 * \param v - The definition variable (must be a resolved array).
 */
void menusSet(Var v);

/*!
 * \brief Create a new GuiPopupMenu
 * 
 * This will add the menu to the manager.
 * Drawing is disable by default.
 * \param menuname - Name of the menu template.
 * \return The newly allocated menu.
 */
GuiPopupMenu GuiPopupMenu_new(String menuname);

/*!
 * \brief Destroy a menu
 *
 * This will destroy the menu's children then remove the menu from the manager.
 * \param menu - The menu.
 */
void GuiPopupMenu_del(GuiPopupMenu menu);

/*!
 * \brief Define the position of a menu
 *
 * The menu will be automatically placed relatively to its parent.
 * \param menu - The menu.
 * \param parent - Rectangle of the menu's creator.
 * \param layout - Where to place the menu, given its parent's rectangle.
 */
void GuiPopupMenu_setPos(GuiPopupMenu menu, GlRect parent, MenuLayout layout);

#endif
