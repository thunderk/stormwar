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
 *   Functions to make tileable textures from pictures                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_TEXTURIZER_H_
#define _SW_TEXTURIZER_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a tileable texture from a picture.
 *
 * \param src - Source picture (must be mod-relative).
 * \param dest - Destination picture (absolute or in the current directory).
 */
void texturize(String src, String dest);

/*!
 * \brief Create a tileable texture from a picture by symetric means.
 *
 * \param src - Source picture (must be mod-relative).
 * \param dest - Destination picture (absolute or in the current directory).
 */
void texturesym(String src, String dest);

#endif
