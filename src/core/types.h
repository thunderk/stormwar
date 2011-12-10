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
 *   Core engine types                                                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_TYPES_H_
#define _SW_CORE_TYPES_H_ 1

/*!
 * \file
 * \brief Core engine types.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

/*#include <SDL_types.h>*/

/******************************************************************************
 *                                   Types                                    *
 ******************************************************************************/
/*! \brief Memory pointer to any type. */
typedef void* Ptr;

/*! \brief Core integer type. */
typedef Sint32 Int;

/*! \brief Core float type. */
typedef float Float;

/*!
 * \brief Private structure for a String.
 *
 * See \ref string.h for more details.
 */
typedef struct pv_String pv_String;

/*!
 * \brief Abstract type for a string.
 *
 * See \ref string.h for more details.
 */
typedef pv_String* String;

/*!
 * \brief Private structure for a Var.
 *
 * See \ref var.h for more details.
 */
typedef struct pv_Var pv_Var;

/*!
 * \brief Abstract type for a variable.
 *
 * See \ref var.h for more details.
 */
typedef pv_Var* Var;

/*! \brief Multiple usage identifier. */
typedef Sint16 CoreID;

/*! \brief Type to store a time in milliseconds. */
typedef Uint32 CoreTime;

/*!
 * \brief Shell function structure.
 *
 * This structure is used both for declaration (prototype) and for calling.
 */
typedef struct
{
    String name;        /*!< Function name. */
    CoreID id;          /*!< Identifier used by the core. */
    int nbparam;        /*!< Number of parameters. */
    Var* params;        /*!< Parameters. */
    Var ret;            /*!< Return variable. */
} ShellFunction;

/******************************************************************************
 *                                  Defines                                   *
 ******************************************************************************/
#ifndef Bool
/*! \brief Simple boolean type. */
#define Bool Uint8
#endif

#ifndef FALSE
/*! \brief Constant 'false' for the Bool type */
#define FALSE 0
#endif

#ifndef TRUE
/*! \brief Constant 'true' for the Bool type */
#define TRUE 1
#endif

#endif
