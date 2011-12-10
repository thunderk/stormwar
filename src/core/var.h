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
 *   Object used to contain any type of variable                              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_VAR_H_
#define _SW_CORE_VAR_H_ 1

/*!
 * \file
 * \brief Object used to contain any type of variable.
 *
 * \par Overview
 * The variable system is very useful to manipulate data.
 * A variable have a name (case sensitive), a type and a value.
 * A variable can be of several types :
 *  - \c VAR_VOID : no value associated.
 *  - \c VAR_INT : signed integer encoded on 32 bits.
 *  - \c VAR_FLOAT : float type.
 *  - \c VAR_STRING : \c String type.
 *  - \c VAR_ARRAY : array of variables, will be explained later.
 *
 * \par Linked variables
 * Variables can be a link to a file, which means the value of this variable is
 * contained in a file. When calling a value access function such as Var_getValueInt,
 * the link will automatically be resolved, returning the value read from the file.
 *
 * \par Arrays
 * Arrays can contain several variables of all types, even other arrays. When adding variables
 * to an array or reading them with a reader, the following rules are applied :
 *  - Unnamed variables are added at the array's end (order preserved)
 *  - Named variables are sorted at the array's beginning.
 *
 * So you need to be careful about the fact that if a named variable is added, order may be lost.
 *
 * \par String image
 * A variable can be represented by a string. It is useful to read or print it.<br>
 * The string will be like this: \#name=value<br>
 * If the name is empty, only the value is given.<br>
 * Spaces are not important, except inside double-quoted expressions.<br>
 * For the value, here are the rules, examples are between parenthesis:
 *  - A \c VAR_INT will be written normally (2) (-12) (0)
 *  - A \c VAR_FLOAT will be written with a dot and from 1 to 8 decimals (0.02563145) (-6.2) (2.0) (0.0)
 *  - A \c VAR_STRING will be written between double-quotes ("hello") ("") ("with space") ("say \"quoted\"") ("&translated string")<br>
 *      As you can see in the example, a string will keep functionalities described in \ref string.h.
 *  - A \c VAR_ARRAY will be listed between brackets, variables being separated with commas ([]) ([1,"test"]) ([1, [\#a = 2.5, \#b = 4]])
 *  - A link will be written with a '\@' character leading a double-quoted path (\@"path/to/file")
 *
 * \par Using a variable as a structure.
 * An array containing named variables can be used as a structure (like a C structure), the variables
 * being the fields of such a structure. To facilitate this, you can use a VarValidator
 * (see \ref varvalidator.h) to make sure of the presence of some fields.
 */
 
/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Types for a variable */
typedef enum
{
    VAR_VOID = 0,              /*!< No type.      */
    VAR_INT = 1,               /*!< Integer type. */
    VAR_FLOAT = 2,             /*!< Float type.   */
    VAR_STRING = 3,            /*!< String type.  */
    VAR_ARRAY = 4,             /*!< Array of variables.  */
    VAR_LINK = 5               /*!< Link to a file containing the value. */
} VarType;

/*! \brief Position in an array. */
typedef Uint16 VarArrayPos;

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a variable.
 *
 * The new variable will have an empty name and a VAR_VOID type.
 * \return The newly allocated variable.
 */
Var Var_new(void);

/*!
 * \brief Create a variable, copy of another.
 *
 * \param v - Original variable.
 * \return The copy of \a v.
 */
Var Var_newByCopy(Var v);

/*!
 * \brief Destroy a variable.
 *
 * \param var - The variable.
 */
void Var_del(Var var);

/*!
 * \brief Compare two variable names.
 *
 * This function can be used to sort an array of variables in PtrArray for example.
 * \param v1 - A pointer to the first variable.
 * \param v2 - A pointer to the second variable.
 * \return 0 if (*v1)==(*v2), a negative number if (*v1)<(*v2), a positive number if (*v1)>(*v2)
 */
int Var_nameCmp(Var* v1, Var* v2);

/*!
 * \brief Change the name of a variable.
 *
 * \param var - The variable.
 * \param name - Name of the variable. This is copied and can be NULL.
 */
void Var_setName(Var var, const char* name);

/*!
 * \brief Change the type of a variable.
 *
 * This will reset the value to the default one.
 * \param var - The variable.
 * \param type - New type.
 */
void Var_setType(Var var, VarType type);

/*!
 * \brief Clear the value of the variable
 *
 * Variable type become VAR_VOID.
 * \param var - The variable
 */
void Var_setVoid(Var var);

/*!
 * \brief Affects an integer value to the variable
 *
 * Variable type become VAR_INT.
 * \param var - The variable
 * \param val - Value
 */
void Var_setInt(Var var, Int val);

/*!
 * \brief Affects an float value to the variable
 *
 * Variable type become VAR_FLOAT.
 * \param var - The variable
 * \param val - Value
 */
void Var_setFloat(Var var, Float val);

/*! 
 * \brief Affects an string value to the variable
 *
 * Variable type become VAR_STRING.
 * \param var - The variable
 * \param val - Value (the string is copied)
 */
void Var_setString(Var var, String val);

/*!
 * \brief Prepare the variable to receive an array of variables.
 *
 * Variable type become VAR_ARRAY, and this array is emptied.
 * \param var - The variable
 */
void Var_setArray(Var var);

/*!
 * \brief Add a variable to an array.
 *
 * elem will be copied and can be freed after.
 * If elem's name isn't NULL, the variable will be inserted in the array, sorted by variable names (and thus may change the order).
 * If var is not an array, it will become one.
 * \param var - The container (array variable, not linked array).
 * \param elem - The element to add.
 */
void Var_addToArray(Var var, Var elem);

/*!
 * \brief Add a variable to an array without copying it.
 *
 * This is the same function as \ref Var_addToArray except that the added
 * variable won't be copied. So don't \ref Var_del free it ! It will be done
 * automatically.
 */
void Var_insertIntoArray(Var var, Var elem);

/*!
 * \brief Give to a variable the value and type of another one
 *
 * Destination's name doesn't change.
 * \param dest - The variable that will be modified
 * \param src - Variable from which take the value
 */
void Var_setFromVar(Var dest, Var src);

/*!
 * \brief Set a variable from a string representation.
 *
 * \param var - Initialized variable to set with read name, type and value.
 * \param s - The string to read.
 * \return TRUE if there was an error.
 */
Bool Var_setFromString(Var var, String s);

/*!
 * \brief Read a variable's value from a file.
 *
 * \param var - Initialized variable to set with type and value (the name will be conserved).
 * \param file - Path to a file containing the variable's value (relative to the mod path).
 * \return TRUE on failure.
 */
Bool Var_readFromFile(Var var, String file);

/*!
 * \brief Save a variable's value to a file.
 *
 * \param var - The var to save.
 * \param file - Absolute path to the file.
 * \return TRUE on failure.
 */
Bool Var_saveToFile(Var var, String file);

/*!
 * \brief Get the current type of a variable.
 *
 * \param var - The variable.
 * \return The variable's type.
 */
VarType Var_getType(Var var);

/*!
 * \brief Get the name of a variable.
 *
 * \param var - The variable.
 * \return The variable's name. \readonly
 */
String Var_getName(Var var);

/*!
 * \brief Get a string image of the variable value.
 *
 * \param var - The variable.
 * \return A string containing the variable representation. \readonly
 */
const char* Var_gets(Var var);

/*! 
 * \brief Get the variable's integer value
 *
 * No type check is performed.
 * \param var - The variable
 * \return Integer value
 */
Int Var_getValueInt(Var var);

/*!
 * \brief Get the variable's float value
 *
 * No type check is performed.
 * \param var - The variable
 * \return Float value
 */
Float Var_getValueFloat(Var var);

/*!
 * \brief Get the variable's string value
 *
 * No type check is performed.
 * \param var - The variable
 * \return String value. \readonly
 */
String Var_getValueString(Var var);

/*!
 * \brief Resolve var links.
 *
 * If the variable isn't a link, it will remain unchanged. So this function
 * can be called when you are not sure if a variable is linked or not.
 * \param var - The variable.
 */
void Var_resolveLink(Var var);

/*!
 * \brief Remove unnamed fields from a named array.
 *
 * \param var - A named array.
 */
void Var_removeUnnamedFields(Var var);

/*!
 * \brief Get an array's element, given its name.
 *
 * No type check is performed.
 * \param var - The array variable.
 * \param name - Name to search for.
 * \return Found array element, NULL if not found.
 */
Var Var_getArrayElemByName(Var var, String name);

/*!
 * \brief Get an array's element, given its name in a C-string.
 *
 * No type check is performed.
 * \param var - The array variable.
 * \param name - Name to search for.
 * \return Found array element, NULL if not found.
 */
Var Var_getArrayElemByCName(Var var, char* name);

/*!
 * \brief Get an array's size.
 *
 * No type check is performed.
 * \param var - The array variable.
 * \return Array size.
 */
VarArrayPos Var_getArraySize(Var var);

/*!
 * \brief Get an array's element, given its position.
 *
 * No type check is performed.
 * Position must be between 0 and (Var_getArraySize(_var_) - 1) , no check performed.
 * \param var - The array variable.
 * \param pos - Position wanted.
 * \return Array element, at the required position.
 */
Var Var_getArrayElemByPos(Var var, VarArrayPos pos);

#endif
