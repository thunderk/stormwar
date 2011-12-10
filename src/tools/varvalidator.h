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
 *   Validator to check the fields of a named array                           *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_VARVALIDATOR_H_
#define _SW_VARVALIDATOR_H_ 1

/*!
 * \file
 * \brief Object used to validate a named array variable.
 * 
 * A VarValidator is able to validate the fields of a named array variable (Var).
 * A named array is like:
 * \code
 *  #namedarray = [#field1=value_of_field1, #field2=..., ...]
 * \endcode
 *
 * These named array are useful to contain a structure, for example a color:
 * \code
 *  #col = [#r = 255, #g = 0, #b = 120, #comment = "test", #altern = [...]]
 * \endcode
 *
 * The problem is, it is fastidious to check every field of an array. The VarValidator
 * comes here. It checks both the presence of a field and its type.
 * Here is how to use a validator to validate the last example
 * (we suppose the variable to validate is called \a var) :
 * \code
 *  //Previously declared: Var var;
 *  VarValidator valid;
 *
 *  VarValidator_declareStringVar(valid, "comment", "");
 *  VarValidator_declareIntVar(valid, "r", 0);
 *  VarValidator_declareIntVar(valid, "g", 0);
 *  VarValidator_declareIntVar(valid, "b", 0);
 *  VarValidator_declareArrayVar(valid, "altern");
 *
 *  VarValidator_validate(valid, var);
 *  VarValidator_del(valid);
 * \endcode
 *
 * After this, the variable \a var is ensured to contain the expected fields with the good types so that these
 * fields can be got without risking an error.
 * Field order isn't important. Moreover, a default value is assigned in case the field isn't
 * found (except for arrays, the default being an empty array).
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/var.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Abstract type for a named array fields validator. */
typedef Var VarValidator;

/******************************************************************************
 *############################################################################*
 *#                                  Functions                               #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a validator
 *
 * \return The newly allocated validator.
 */
VarValidator VarValidator_new(void);

/*!
 * \brief Delete a validator.
 *
 * \param val - The validator.
 */
void VarValidator_del(VarValidator val);

/*!
 * \brief Declare an integer variable.
 *
 * If the variable already exists in the validator, it is modified.
 * \param val      - The validator
 * \param varname  - Name of the variable
 * \param varvalue - Default value
 */
void VarValidator_declareIntVar(VarValidator val, const char* varname, Int varvalue);

/*!
 * \brief Declare a float variable.
 *
 * If the variable already exists in the validator, it is modified.
 * \param val      - The validator
 * \param varname  - Name of the variable
 * \param varvalue - Default value
 */
void VarValidator_declareFloatVar(VarValidator val, const char* varname, Float varvalue);

/*!
 * \brief Declare a string variable.
 *
 * If the variable already exists in the validator, it is modified.
 * \param val      - The validator
 * \param varname  - Name of the variable
 * \param varvalue - Default value
 */
void VarValidator_declareStringVar(VarValidator val, const char* varname, char* varvalue);

/*!
 * \brief Declare an array variable.
 *
 * Unlike int, float and string variables, the types and names of the elements inside the array
 * are not checked (just the name and the fact that it's an array are checked).
 * Default value is an empty array (it would be too complicated to define it by function parameters).
 * \param val      - The validator
 * \param varname  - Name of the variable
 */
void VarValidator_declareArrayVar(VarValidator val, const char* varname);

/*!
 * \brief Validate a named array with the previously declared variables.
 *
 * After this, v is considered as validated. This ensures that v is a named array and
 * the declared variables are actually in the named array and of the good type.
 * Unexpected variables are removed.
 * Linked arrays are resolved.
 * \param val - The validator with full declarations done.
 * \param v - The variable to validate (type and value might be modified to match expected declarations).
 */
void VarValidator_validate(VarValidator val, Var v);

#endif
