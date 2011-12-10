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
 *   Basic type functions                                                     *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "graphics/types.h"
#include "graphics/glrect.h"
#include "graphics/color.h"

#include "core/var.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

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
 *#                             Internal functions                           #*
 *############################################################################*
 ******************************************************************************/
void
typesInit()
{
    
}

/*----------------------------------------------------------------------------*/
void
typesUninit()
{
    
}

/******************************************************************************
 *############################################################################*
 *#                              Public functions                            #*
 *############################################################################*
 ******************************************************************************/
void
ColorRGBA_makeFromVar(GlColorRGBA* col, Var v)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "r", 0);
    VarValidator_declareIntVar(valid, "g", 0);
    VarValidator_declareIntVar(valid, "b", 0);
    VarValidator_declareIntVar(valid, "a", 0);
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    col->r = Var_getValueInt(Var_getArrayElemByCName(v, "r"));
    col->g = Var_getValueInt(Var_getArrayElemByCName(v, "g"));
    col->b = Var_getValueInt(Var_getArrayElemByCName(v, "b"));
    col->a = Var_getValueInt(Var_getArrayElemByCName(v, "a"));
}

/*----------------------------------------------------------------------------*/
void
returnInsideRect(GlRect* rct, Gl2DCoord* x, Gl2DCoord* y)
{
    (*x) = imin(imax(rct->x, (*x)), rct->x + rct->w - 1);
    (*y) = imin(imax(rct->y, (*y)), rct->y + rct->h - 1);
}

/*----------------------------------------------------------------------------*/
Bool
isInRect(GlRect* rct, Gl2DCoord x, Gl2DCoord y)
{
    return ((x >= rct->x) && (x < rct->x + rct->w) && (y >= rct->y) && (y < rct->y + rct->h));
}

/*----------------------------------------------------------------------------*/
Bool
GlRect_clip(GlRect* rct, GlRect* clip)
{
    Sint16 d;
    Bool ret = FALSE;
    
    d = clip->x - rct->x;
    if (d > 0)
    {
        rct->w -= MIN(d, rct->w);
        rct->x = clip->x;
        ret = TRUE;
    }
    d = clip->y - rct->y;
    if (d > 0)
    {
        rct->h -= MIN(d, rct->h);
        rct->y = clip->y;
        ret = TRUE;
    }
    d = (rct->x + rct->w - clip->x - clip->w);
    if (d > 0)
    {
        rct->w -= MIN(d, rct->w);
        ret = TRUE;
    }
    d = (rct->y + rct->h - clip->y - clip->h);
    if (d > 0)
    {
        rct->h -= MIN(d, rct->h);
        ret = TRUE;
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
Bool
GlRect_equal(GlRect rct1, GlRect rct2)
{
    return ((rct1.x == rct2.x) && (rct1.y == rct2.y) && (rct1.w == rct2.w) && (rct1.h == rct2.h));
}

/*----------------------------------------------------------------------------*/
void
GlRect_cropBorders(GlRect* rct, Gl2DSize bleft, Gl2DSize bright, Gl2DSize btop, Gl2DSize bbottom)
{
    Gl2DSize dw, dh;
    
    rct->x += bleft;
    rct->y += btop;
    dw = bleft + bright;
    rct->w = ((dw >= rct->w) ? 0 : (rct->w - dw));
    dh = btop + bbottom;
    rct->h = ((dh >= rct->h) ? 0 : (rct->h - dh));
}

/*----------------------------------------------------------------------------*/
void
GlRect_makeFromVar(GlRect* rct, Var vrct)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "x", 0);
    VarValidator_declareIntVar(valid, "y", 0);
    VarValidator_declareIntVar(valid, "w", 1);
    VarValidator_declareIntVar(valid, "h", 1);
    VarValidator_validate(valid, vrct);
    VarValidator_del(valid);
    
    rct->x = (Gl2DCoord)Var_getValueInt(Var_getArrayElemByCName(vrct, "x"));
    rct->y = (Gl2DCoord)Var_getValueInt(Var_getArrayElemByCName(vrct, "y"));
    rct->w = (Gl2DSize)Var_getValueInt(Var_getArrayElemByCName(vrct, "w"));
    rct->h = (Gl2DSize)Var_getValueInt(Var_getArrayElemByCName(vrct, "h"));
}
