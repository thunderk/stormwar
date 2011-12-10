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
 *   Graphical map to represent the whole world                               *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_WORLDMAP_H_
#define _SW_WORLDMAP_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "world/world.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
/*!
 * \brief Type for map plots.
 *
 * Can be a combination of MapPlot flags, such as MapPlot_GROUND | MapPlot_STATICUNIT.
 */
typedef Uint8 MapPlot;

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define MapPlot_VOID        0x00
#define MapPlot_GROUND      0x01
#define MapPlot_PATH        0x02
#define MapPlot_STATICUNIT  0x04
#define MapPlot_DYNUNIT     0x08
#define MapPlot_MAINUNIT    0x10
#define MapPlot_CAMERA      0x20

/******************************************************************************
 *############################################################################*
 *#                               Map functions                              #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the map.
 */
void worldmapInit(void);

/*!
 * \brief Destroy the map.
 */
void worldmapUninit(void);

/*!
 * \brief Set the map from a definition variable.
 *
 * This will redraw the map to match the definition.
 * \param v - The definition variable.
 */
void worldmapSet(Var v);

/*!
 * \brief Clear the map of all plots.
 */
void worldmapClear(void);

/*!
 * \brief Set a plot at a given position.
 *
 * Don't use a combination of plot types here.
 * \param x - X position.
 * \param y - Y position.
 * \param plot - Type of the plot to set.
 */
void worldmapSetPlot(WorldCoord x, WorldCoord y, MapPlot plot);

/*!
 * \brief Unset a previously set plot.
 *
 * Don't use a combination of plot types here.
 * If the plot wasn't set at the given position, it will be silently ignored.
 * \param x - X position.
 * \param y - Y position.
 * \param plot - Type to unset.
 */
void worldmapUnsetPlot(WorldCoord x, WorldCoord y, MapPlot plot);

#endif
