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
 *   Graphical theme manager                                                  *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This file handles graphical themes used by the user interface.
 *
 * TODO...
 */

#ifndef _SW_GUITHEME_H_
#define _SW_GUITHEME_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "gui/guitexture.h"
#include "graphics/gltextrender.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Standard themes.
 */
typedef enum
{
    GUITHEME_DIALOG = 0,
    GUITHEME_BUTTON = 1,
    GUITHEME_BUTTONPUSHED = 2,
    GUITHEME_MENU = 3,
    GUITHEME_INPUT = 4,
    GUITHEME_TOOLTIP = 5,
    GUITHEME_SCROLLBAR = 6
} GuiThemeStd;

/*!
 * \brief Structure of a graphical theme.
 */
typedef struct
{
    GuiTexture bgtex;           /*!< Background texture, \readonly */
    GlTextRender textrender;    /*!< Text renderer. Only rendering limits can be changed and it is preferable to set them each time (to prevent from concurrent accesses). */
    Uint16 borderleft;          /*!< Left border to the content. */
    Uint16 borderright;         /*!< Right border to the content. */
    Uint16 bordertop;           /*!< Top border to the content. */
    Uint16 borderbottom;        /*!< Bottom border to the content. */
} GuiTheme;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the theme manager.
 */
void guithemeInit(void);

/*!
 * \brief Destroy the theme manager.
 */
void guithemeUninit(void);

/*!
 * \brief Set themes from a definition variable.
 *
 * \param v - The definition variable.
 */
void guithemeSet(Var v);

/*!
 * \brief Get a standard theme.
 *
 * \param type - Type of standard element.
 */
GuiTheme* guithemeGetStd(GuiThemeStd type);

#endif
