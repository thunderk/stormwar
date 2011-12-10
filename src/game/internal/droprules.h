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
 *   Rules to drop entities                                                   *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_DROPRULES_H_
#define _SW_DROPRULES_H_ 1

/*!
 * \file
 * \brief Rules to decide whether or not a player can drop a piece.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "world/world.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private type for drop rules.
 */
typedef struct pv_DropRules pv_DropRules;
    
/*!
 * \brief Abstract type for drop rules.
 */
typedef pv_DropRules* DropRules;

/******************************************************************************
 *############################################################################*
 *#                            DropRules functions                           #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create new drop rules from a definition variable.
 *
 * \param v - The definition variable.
 * \return Newly allocated rules.
 */
DropRules   DropRules_new(Var v);

/*!
 * \brief Delete drop rules.
 *
 * \param rules - The rules to delete.
 */
void        DropRules_del(DropRules rules);

/*!
 * \brief Check drop rules to know if a dropping is allowed, given certain conditions.
 *
 * \param rules - The rules to apply.
 * \param posx - X dropping position.
 * \param posy - Y dropping position.
 * \return TRUE if the dropping is allowed, FALSE if it is denied.
 */
Bool        DropRules_check(DropRules rules, WorldCoord posx, WorldCoord posy);

#endif
