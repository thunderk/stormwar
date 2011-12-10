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
 *   Graphical interface for shell input/output                               *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GUISHELL_H_
#define _SW_GUISHELL_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                             Engine functions                             #*
 *############################################################################*
 ******************************************************************************/

/*! \brief Initialize the module */
void guishellInit(void);

/*! \brief Uninitialize the module */
void guishellUninit(void);

/*!
 * \brief Set the graphical shell from a definition variable.
 *
 * \param v - The definition variable.
 */
void guishellSet(Var v);

/*!
 * \brief Set graphical shell visibility.
 *
 * \param show - TRUE to show the shell, FALSE to hide it.
 */
void guishellShow(Bool show);

/*!
 * \brief Toggle graphical shell visibility.
 */
void guishellToggle();

#endif
