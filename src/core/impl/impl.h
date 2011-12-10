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
 *   Core engine internal functions                                           *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_IMPL_IMPL_H_
#define _SW_CORE_IMPL_IMPL_H_ 1

/*!
 * \file
 * \brief Core engine internal functions.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/core.h"
#include "core/reader.h"

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Retrive a function definition and a callback.
 *
 * This is used by the shell.
 * \param modname - Module name; if NULL, the search will be performed in all modules.
 * \param funcname - Function name.
 * \param r_callback - Callback associated returned.
 * \return The function found, NULL if not found. \readonly
 */
ShellFunction* coreFindShellFunction(String modname, String funcname, CoreShellCallback* r_callback);

/*!
 * \brief Execute a shell command parsed by a reader.
 *
 * \param ret - Returned variable.
 * \param reader - Positioned reader.
 * \return TRUE in case of error.
 */
Bool shellExecFromReader(Var ret, Reader reader);

/*!
 * \brief Set a variable from a Reader.
 *
 * \param var - The var to set.
 * \param reader - The reader.
 */
void Var_setFromReader(Var var, Reader reader);

void shellInit(void);
void shellUninit(void);
void i18nInit(void);
void i18nUninit(void);

#endif
