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
 *   String type                                                              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_STRING_H_
#define _SW_CORE_STRING_H_ 1

/*!
 * \file
 * \brief This file handles strings of characters.
 *
 * A String is a convenience type that manages a '\\0'-terminated character array.
 * It will automatically grow to contain requested data.
 *
 * If a string starts with a '\&' character, it will be translated by the i18n module.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include <stdarg.h>

#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new string.
 *
 * \param st - Initialization string (will be copied, can be NULL).
 * \return Newly allocated string.
 */
String String_new(const char* st);

/*!
 * \brief Create a new string by copying the beginning of another.
 *
 * The size of \a st will NOT be checked.
 * \param st - Initialization string.
 * \param nbc - Number of characters to be copied from \a st.
 * \return Newly allocated string.
 */
String String_newBySizedCopy(const char* st, unsigned int nbc);

/*!
 * \brief Create a string by copying the content of an other one.
 *
 * \param string - Source string.
 * \return Newly allocated string.
 */
String String_newByCopy(String string);

/*!
 * \brief Delete a string.
 *
 * \param s - The string.
 */
void String_del(String s);

/*!
 * \brief Get a pointer to the content of the string.
 *
 * \param string - The string.
 * \return A pointer to the string's content. \readonly
 */
char* String_get(String string);

/*!
 * \brief Check if a string is empty.
 *
 * \param string - The string.
 * \return TRUE if the string is empty, FALSE otherwise.
 */
Bool String_isEmpty(String string);

/*!
 * \brief Get the string's length.
 *
 * \param string - The string.
 * \return The string's length.
 */
unsigned int String_getLength(String string);

/*!
 * \brief Replace the content of a string by a C-string.
 *
 * \param string - The string to replace
 * \param cstring - Replacing string (will be copied, must end with a '\\0').
 */
void String_replace(String string, const char* cstring);

/*!
 * \brief Clear the content of a string.
 *
 * \param string - The string to clear.
 */
void String_clear(String string);

/*!
 * \brief Copy a string to replace another one.
 *
 * This replace the first string by the content of the second one.
 * \param dest - Destination string
 * \param src - Source string
 */
void String_copy(String dest, String src);

/*!
 * \brief Removes \a size characters from \a string, starting at position \a pos.
 *
 * The string content will be shifted down to fill the gap.
 * \param string - The string.
 * \param pos - Starting position.
 * \param size - Number of characters to erase.
 */
void String_erase(String string, unsigned int pos, unsigned int size);

/*!
 * \brief Append the content of the second string to the first one.
 *
 * The first string will grow, the second one won't be modified.
 * \param string1 - Base string
 * \param string2 - String to append
 */
void String_appendString(String string1, String string2);

/*!
 * \brief Append the content of the C-style string to the first String.
 *
 * The first string will grow, the second one won't be modified.
 * \param string - Base string.
 * \param st - String to append (must end with a '\\0').
 */
void String_append(String string, const char* st);

/*!
 * \brief Append a character to the String.
 *
 * The first string will grow.
 * \param string - Base string.
 * \param ch - Character to append.
 */
void String_appendChar(String string, char ch);

/*!
 * \brief Function to compare two strings.
 *
 * \param s1 - First string.
 * \param s2 - Second string.
 * \return TRUE if the strings are equals, else FALSE.
 */
Bool String_equal(String s1, String s2);

/*!
 * \brief Function that can be used in custom containers (such as a PtrArray) to compare two strings.
 *
 * \param s1 - Pointer to the first string.
 * \param s2 - Pointer to the second string.
 * \return 0 if (*s1)==(*s2), a negative number if (*s1)<(*s2), a positive number if (*s1)>(*s2)
 */
int String_cmp(String* s1, String* s2);

/*!
 * \brief Replace a string by the result of a formatted print.
 *
 * \param s - The string.
 * \param format - Format string.
 * \param ... - Variable arguments.
 */
void String_printf(String s, const char* format, ...);

/*!
 * \brief Replace a string by the result of a formatted print.
 *
 * \param s - The string.
 * \param format - Format string.
 * \param list - List of arguments.
 */
void String_vprintf(String s, const char* format, va_list list);

#endif
