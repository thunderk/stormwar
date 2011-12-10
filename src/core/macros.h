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
 *   Core macros                                                              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_MACROS_H_
#define _SW_CORE_MACROS_H_ 1

/*!
 * \file
 * \brief Core engine miscellaneous macros.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

/******************************************************************************
 *                                   Macros                                   *
 ******************************************************************************/
/*!
 * \brief Check that variable arguments match a printf style format.
 *
 * Indexes start from 1.
 * Add this after the prototype of a printf-like function (just before the semicolon).
 * \param format_idx - Index of the format string in the arguments.
 * \param arg_idx - Index of the first variable argument (the "...").
 */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define CHECK_ARGS_PRINTF(format_idx, arg_idx) __attribute__((__format__(__printf__, format_idx, arg_idx)))
#else
#define CHECK_ARGS_PRINTF(format_idx, arg_idx)
#endif

#endif
