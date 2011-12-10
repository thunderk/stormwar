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
 *   Internationalization                                                     *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_I18N_H_
#define _SW_CORE_I18N_H_ 1

/*!
 * \file
 * \brief Internationalization module.
 *
 * This module is useful to translate strings.
 *
 * All translations are performed at run-time.
 * There are three kinds of translation:
 *  \li C strings can be translated by using the macro _("...").
 *  \li Core strings (see \ref string.h) can be translated by using the macro _s(string).
 *  \li Data strings (loaded in variables, see \ref var.h) are translated if they begin
 *      with a '&'.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"

/******************************************************************************
 *                                   Macros                                   *
 ******************************************************************************/
#ifdef USE_I18N
    #define _(_string_) pv_i18nTranslate(_string_)
    #define _s(_string_) pv_i18nTranslateString(_string_)
#else
    /*!
     * \brief Pass a char* string to the internationalisation system.
     *
     * \param _string_ - (char*) The original string.
     * \return (char*) Translated (or not) string. \readonly
     */
    #define _(_string_) _string_
    /*!
     * \brief Pass a string to the internationalisation system.
     *
     * \param _string_ - (String) The original string.
     * \return (String) Translated (or not) string. \readonly
     */
    #define _s(_string_) _string_
#endif

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Translate a C string.
 *
 * Don't use this function directly, use the _(...) macro instead.
 * \param st - Original string.
 * \return The translated string. \a st if not found or if the module is not initialized. \readonly
 */
char* pv_i18nTranslate(char* st);

/*!
 * \brief Translate a String.
 *
 * Don't use this function directly, use the _s(...) macro instead.
 * \param st - Original string.
 * \return The translated string. \a st if not found or if the module is not initialized. \readonly
 */
String pv_i18nTranslateString(String st);

#endif
