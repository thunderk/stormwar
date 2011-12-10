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
 *   Platform dependent compatibility                                         *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_COMP_H_
#define _SW_CORE_COMP_H_ 1

/*!
 * \file
 * \brief This file contains some functions that will use platform dependent code.
 */

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Guess the local language code.
 *
 * Will default to 'en' if not found.
 * \return A buffer at least two characters long, containing the local language code.
 */
const char* compGuessLocalLanguage(void);

/*!
 * \brief Show an error message.
 *
 * \param title - Title of the message (often the application name).
 * \param message - Error message to display.
 */
void compErrorMessage(const char* title, const char* message);

#endif
