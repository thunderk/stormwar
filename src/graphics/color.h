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
 *   Colour functions                                                         *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_COLOR_H_
#define _SW_GRAPHICS_COLOR_H_ 1

/*!
 * \file
 * \brief This file brings some basic functions to handle color types.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "graphics/types.h"

/******************************************************************************
 *                                 Constants                                  *
 ******************************************************************************/
/*! \brief Black and fully transparent color. */
#define GlColor_NULL 0x00000000
/*! \brief Mask for R component. Internal use. */
extern Uint32 GlColor_Rmask;
/*! \brief Shift for R component. Internal use. */
extern Uint32 GlColor_Rshift;
/*! \brief Mask for G component. Internal use. */
extern Uint32 GlColor_Gmask;
/*! \brief Shift for G component. Internal use. */
extern Uint32 GlColor_Gshift;
/*! \brief Mask for B component. Internal use. */
extern Uint32 GlColor_Bmask;
/*! \brief Shift for B component. Internal use. */
extern Uint32 GlColor_Bshift;
/*! \brief Mask for A component. Internal use. */
extern Uint32 GlColor_Amask;
/*! \brief Shift for A component. Internal use. */
extern Uint32 GlColor_Ashift;

/******************************************************************************
 *############################################################################*
 *#                           Conversion functions                           #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Convert a GlColorRGBA structure to a GlColor type.
 *
 * \param _colrgba_ - (\ref GlColorRGBA) Source color.
 * \return (\ref GlColor) Resulting color.
 */
#define GlColorRGBA_to_GlColor(_colrgba_) ((GlColor)(((_colrgba_).r << GlColor_Rshift) | ((_colrgba_).g << GlColor_Gshift) | ((_colrgba_).b << GlColor_Bshift) | ((_colrgba_).a << GlColor_Ashift)))

/******************************************************************************
 *############################################################################*
 *#                            ColorRGB? functions                           #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Fill a \ref GlColorRGBA structure with values.
 * 
 * \param _col_ - (\ref GlColorRGBA) The color to fill.
 * \param _r_ - (Uint8) Red value.
 * \param _g_ - (Uint8) Green value.
 * \param _b_ - (Uint8) Blue value.
 * \param _a_ - (Uint8) Alpha value.
 */
#define GlColorRGBA_MAKE(_col_,_r_,_g_,_b_,_a_) (_col_).r=(_r_);(_col_).g=(_g_);(_col_).b=(_b_);(_col_).a=(_a_)

/*!
 * \brief Fill a GlColorRGBA structure from a named array variable.
 *
 * \param col - Color to fill.
 * \param v - Variable to read values from.
 */
void GlColorRGBA_makeFromVar(GlColorRGBA* col, Var v);

/*!
 * \brief Fill a \ref GlColorRGB structure with values.
 * 
 * \param _col_ - (\ref GlColorRGB) The color to fill.
 * \param _r_ - (Uint8) Red value.
 * \param _g_ - (Uint8) Green value.
 * \param _b_ - (Uint8) Blue value.
 */
#define GlColorRGB_MAKE(_col_,_r_,_g_,_b_) (_col_).r=(_r_);(_col_).g=(_g_);(_col_).b=(_b_)

/*!
 * \brief Fill a GlColorRGB structure from a named array variable.
 *
 * \param col - Color to fill.
 * \param v - Variable to read values from.
 */
void GlColorRGB_makeFromVar(GlColorRGB* col, Var v);

/******************************************************************************
 *############################################################################*
 *#                             GlColor functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Make a GlColor from color and alpha components.
 *
 * \param _r_ - (Uint8) Red value.
 * \param _g_ - (Uint8) Green value.
 * \param _b_ - (Uint8) Blue value.
 * \param _a_ - (Uint8) Alpha value.
 * \return (\ref GlColor) The associated color.
 */
#define GlColor_MAKE(_r_,_g_,_b_,_a_) ((GlColor)(((_r_) << GlColor_Rshift) | ((_g_) << GlColor_Gshift) | ((_b_) << GlColor_Bshift) | ((_a_) << GlColor_Ashift)))

/*!
 * \brief Return the alpha component of a GlColor.
 *
 * \param _color_ - (\ref GlColor) Color from which we extract alpha value.
 * \return (Uint8) The alpha component of the color.
 */
#define GlColor_GETA(_color_) (Uint8)(((_color_) & GlColor_Amask) >> GlColor_Ashift)

/*!
 * \brief Return the red component of a GlColor.
 *
 * \param _color_ - (\ref GlColor) Color from which we extract red value.
 * \return (Uint8) The red component of the color.
 */
#define GlColor_GETR(_color_) (Uint8)(((_color_) & GlColor_Rmask) >> GlColor_Rshift)

/*!
 * \brief Return the green component of a GlColor.
 *
 * \param _color_ - (\ref GlColor) Color from which we extract green value.
 * \return (Uint8) The green component of the color.
 */
#define GlColor_GETG(_color_) (Uint8)(((_color_) & GlColor_Gmask) >> GlColor_Gshift)

/*!
 * \brief Return the blue component of a GlColor.
 *
 * \param _color_ - (\ref GlColor) Color from which we extract blue value.
 * \return (Uint8) The blue component of the color.
 */
#define GlColor_GETB(_color_) (Uint8)(((_color_) & GlColor_Bmask) >> GlColor_Bshift)

/*!
 * \brief Set the alpha value of a GlColor.
 *
 * \param _color_ - (\ref GlColor) The color we want to change.
 * \param _a_     - (Uint8) Alpha value.
 */
#define GlColor_SETA(_color_,_a_) (_color_) &= ~GlColor_Amask; (_color_) |= (_a_) << GlColor_Ashift

/*!
 * \brief Set the red value of a GlColor.
 *
 * \param _color_ - (\ref GlColor) The color we want to change.
 * \param _r_     - (Uint8) Red value.
 */
#define GlColor_SETR(_color_,_r_) (_color_) &= ~GlColor_Rmask; (_color_) |= (_r_) << GlColor_Rshift

/*!
 * \brief Set the green value of a GlColor.
 *
 * \param _color_ - (\ref GlColor) The color we want to change.
 * \param _g_     - (Uint8) Green value.
 */
#define GlColor_SETG(_color_,_g_) (_color_) &= ~GlColor_Gmask; (_color_) |= (_g_) << GlColor_Gshift

/*!
 * \brief Set the blue value of a GlColor.
 *
 * \param _color_ - (\ref GlColor) The color we want to change.
 * \param _b_     - (Uint8) Blue value.
 */
#define GlColor_SETB(_color_,_b_) (_color_) &= ~GlColor_Bmask; (_color_) |= (_b_) << GlColor_Bshift

#endif
