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
 *   Buffered reader                                                          *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_READER_H_
#define _SW_CORE_READER_H_ 1

/*!
 * \file
 * \brief Buffered reader for files and strings.
 *
 * The reader is useful to parse an input and collect tokens from it.
 * \todo Error message facilities.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"

/******************************************************************************
 *                                   Types                                    *
 ******************************************************************************/
/*! \brief Private structure for a Reader. */
typedef struct pv_Reader pv_Reader;

/*! \brief Abstract type of a buffered reader. */
typedef pv_Reader* Reader;

/*! \brief Types of token. */
typedef enum
{
    READER_END,     /*!< End of stream was encountered. */
    READER_CHAR,    /*!< A special character was read, stored in value.c */
    READER_STRING,  /*!< A double-quoted string was read, stored in value.s (without double-quotes) */
    READER_INT,     /*!< An integer was read, stored in value.i */
    READER_FLOAT,   /*!< A float was read, stored in value.f */
    READER_NAME     /*!< A name was read, stored in value.s */
} ReaderTokenType;

/*! \brief Readed token. */
typedef struct
{
    ReaderTokenType type;   /*!< Type of the token. */
    union
    {
        char c;         /*!< Character value. */
        String s;       /*!< String value. */
        Int i;          /*!< Integer value. */
        Float f;        /*!< Float value. */
    } value;                /*!< Value of the token. */
} ReaderToken;

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new file reader.
 *
 * This will perform an initial step 'forward'.
 * \param path - Full path of the file.
 * \return The new reader.
 */
Reader Reader_newFromFile(const char* path);

/*!
 * \brief Create a new string reader.
 *
 * This will perform an initial step 'forward'.
 * \param s - The '\\0'-terminated string (will be copied).
 * \return The new reader.
 */
Reader Reader_newFromString(const char* s);

/*!
 * \brief Destroy a reader.
 *
 * \param reader - The reader.
 */
void Reader_del(Reader reader);

/*!
 * \brief Get the current token.
 *
 * \param reader - The reader.
 * \return A pointer to the current token.
 */
ReaderToken* Reader_getCurrent(Reader reader);

/*!
 * \brief Make the reader progress by one token.
 *
 * \param reader - The reader.
 * \return A pointer to the newly read token. \readonly
 */
ReaderToken* Reader_forward(Reader reader);

#endif
