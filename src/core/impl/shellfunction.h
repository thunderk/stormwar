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
 *   Shell function object                                                    *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_IMPL_SHELLFUNCTION_H_
#define _SW_CORE_IMPL_SHELLFUNCTION_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"
#include "core/var.h"

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a function definition.
 *
 * \param func - Pointer to a valid function structure.
 * \param name - Function name (will be copied).
 * \param id - Function identifier.
 * \param nbparams - Number of parameters that will be set.
 * \param rettype - Type of the returned variable.
 */
void ShellFunction_new(ShellFunction* func, String name, CoreID id, int nbparams, VarType rettype);

/*!
 * \brief Create a new function by copying another.
 *
 * This will only copy the types, not the values of parameters and returned variable.
 * \param src - The function to copy.
 * \param dest - Pointer to a valid but not initialized structure.
 */
void ShellFunction_copy(ShellFunction* src, ShellFunction* dest);

/*!
 * \brief Destroy a function definition.
 *
 * \param func - The function.
 */
void ShellFunction_del(ShellFunction* func);

/*!
 * \brief Compare two functions' names.
 *
 * Use this as a sort function.
 * \param func1 - Pointer to a valid function.
 * \param func2 - Pointer to a valid function.
 * \return 0 if functions are equal, 1 if the first function is greater, -1 otherwise.
 */
int ShellFunction_cmp(ShellFunction** func1, ShellFunction** func2);

/*!
 * \brief Defines a parameter type
 *
 * \param func - The function
 * \param pos - Position of the parameter (must be in [0..nbparams-1])
 * \param type - Requested type
 */
void ShellFunction_setParamType(ShellFunction* func, int pos, VarType type);

/*!
 * \brief Get the function prototype string
 *
 * \param func - The function
 * \param prototype - The string to replace by the prototype
 */
void ShellFunction_getPrototype(ShellFunction* func, String prototype);

#endif
