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
 *   Lights                                                                   *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_LIGHT_H_
#define _SW_GRAPHICS_LIGHT_H_ 1

/*!
 * \file
 * \brief Lights manager.
 *
 * \todo Tree structure with meta-lights for optimized light choice.
 * \todo Check the orientation.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/types.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure for a Light.
 */
typedef struct pv_Light pv_Light;

/*!
 * \brief Abstract type for a light.
 */
typedef pv_Light* Light;

/*!
 * \brief Standard type for light factors (parameters).
 */
typedef float LightFactor;

/*!
 * \brief Attenuation modes.
 */
typedef enum
{
    LIGHT_ATTCONSTANT,  /*!< Constant attenuation. */
    LIGHT_ATTLINEAR,    /*!< Linear attenuation. */
    LIGHT_ATTQUADRATIC  /*!< Quadratic attenuation. */
} LightAttMode;

/******************************************************************************
 *############################################################################*
 *#                             Light functions                              #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Add a light to the scene.
 *
 * The light needs to be toggled with \ref lightToggle .
 * \return The new light.
 */
Light lightAdd();

/*!
 * \brief Remove a light from the scene.
 *
 * \param li - The light.
 */
void lightDel(Light li);

/*!
 * \brief Remove all lights from the scene.
 */
void lightDelAll(void);

/*!
 * \brief Toggle a light.
 *
 * \param li - The light.
 * \param lit - Specifies if the light should be lit (TRUE) or not (FALSE).
 */
void lightToggle(Light li, Bool lit);

/*!
 * \brief Set a light's parameters from a definition variable.
 *
 * \param li - The light.
 * \param v - The definition variable.
 */
void lightSetFromVar(Light li, Var v);

/*!
 * \brief Set a light's colours.
 *
 * \param li - The light.
 * \param col_ambient - Ambient colour.
 * \param col_diffuse - Diffuse colour.
 * \param col_specular - Specular colour.
 */
void lightSetColor(Light li, GlColorRGB col_ambient, GlColorRGB col_diffuse, GlColorRGB col_specular);

/*!
 * \brief Set a light's position inside the scene.
 *
 * \param li - The light.
 * \param x - New X position.
 * \param y - New Y position.
 * \param z - New Z position.
 */
void lightSetPos(Light li, Gl3DCoord x, Gl3DCoord y, Gl3DCoord z);

/*!
 * \brief Set a light's direction.
 *
 * \param li - The light.
 * \param angh - Horizontal angle.
 * \param angv - Vertical angle.
 */
void lightSetDirection(Light li, Gl3DCoord angh, Gl3DCoord angv);

/*!
 * \brief Set a light's parameters.
 *
 * \param li - The light.
 * \param exponent - Exponent factor [0.0 - 128.0].
 * \param cutoff - Cutoff factor [0.0 - 90.0] or 128.0.
 * \param attenuation - Attenuation factor [0.0 - +inf].
 * \param attmode - Attenuation mode.
 */
void lightSetParams(Light li, LightFactor exponent, LightFactor cutoff, LightFactor attenuation, LightAttMode attmode);

#endif
