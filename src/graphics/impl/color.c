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
 *   Colours functions                                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/color.h"

#include "graphics/impl/impl.h"
#include "graphics/glsurface.h"
#include "tools/varvalidator.h"
#include "core/var.h"

/******************************************************************************
 *                                  Globals                                   *
 ******************************************************************************/
Uint32 GlColor_Rmask;
Uint32 GlColor_Rshift;
Uint32 GlColor_Gmask;
Uint32 GlColor_Gshift;
Uint32 GlColor_Bmask;
Uint32 GlColor_Bshift;
Uint32 GlColor_Amask;
Uint32 GlColor_Ashift;

/******************************************************************************
 *############################################################################*
 *#                            ColorRGBA functions                           #*
 *############################################################################*
 ******************************************************************************/
void
colorInit()
{
    GlSurface surf;
    SDL_Surface* sdlsurf;
    
    surf = GlSurface_new(1, 1, TRUE);
    sdlsurf = GlSurface_getSDLSurface(surf);
    GlColor_Rmask = sdlsurf->format->Rmask;
    GlColor_Gmask = sdlsurf->format->Gmask;
    GlColor_Bmask = sdlsurf->format->Bmask;
    GlColor_Amask = sdlsurf->format->Amask;
    GlColor_Rshift = sdlsurf->format->Rshift;
    GlColor_Gshift = sdlsurf->format->Gshift;
    GlColor_Bshift = sdlsurf->format->Bshift;
    GlColor_Ashift = sdlsurf->format->Ashift;
    GlSurface_del(surf);
}

/*----------------------------------------------------------------------------*/
void
GlColorRGBA_makeFromVar(GlColorRGBA* col, Var vcol)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "r", 0);
    VarValidator_declareIntVar(valid, "g", 0);
    VarValidator_declareIntVar(valid, "b", 0);
    VarValidator_declareIntVar(valid, "a", 0);
    VarValidator_validate(valid, vcol);
    VarValidator_del(valid);
    
    col->r = Var_getValueInt(Var_getArrayElemByCName(vcol, "r"));
    col->g = Var_getValueInt(Var_getArrayElemByCName(vcol, "g"));
    col->b = Var_getValueInt(Var_getArrayElemByCName(vcol, "b"));
    col->a = Var_getValueInt(Var_getArrayElemByCName(vcol, "a"));
}

/*----------------------------------------------------------------------------*/
void
GlColorRGB_makeFromVar(GlColorRGB* col, Var vcol)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "r", 0);
    VarValidator_declareIntVar(valid, "g", 0);
    VarValidator_declareIntVar(valid, "b", 0);
    VarValidator_validate(valid, vcol);
    VarValidator_del(valid);
    
    col->r = Var_getValueInt(Var_getArrayElemByCName(vcol, "r"));
    col->g = Var_getValueInt(Var_getArrayElemByCName(vcol, "g"));
    col->b = Var_getValueInt(Var_getArrayElemByCName(vcol, "b"));
}
