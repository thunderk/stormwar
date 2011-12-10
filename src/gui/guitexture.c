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
 *   Textures for the interface.                                              *
 *   These textures can be tiled and have a border.                           *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/guitexture.h"
#include "graphics/glsurface.h"
#include "tools/fonct.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *############################################################################*
 *#                              Public functions                            #*
 *############################################################################*
 ******************************************************************************/
GuiTexture
GuiTexture_new()
{
    GuiTexture ret;
    
    ret = (GuiTexture)MALLOC(sizeof(pv_GuiTexture));
    ret->surf = GlSurface_new(10, 10, FALSE);
    ret->border_left = 0;
    ret->border_right = 0;
    ret->border_top = 0;
    ret->border_bottom = 0;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GuiTexture_del(GuiTexture tex)
{
    GlSurface_del(tex->surf);
    FREE(tex);
}

/*----------------------------------------------------------------------------*/
void
GuiTexture_set(GuiTexture tex, Var v)
{
    VarValidator valid;
    
    /*init*/
    valid = VarValidator_new();
    
    /*default*/
    VarValidator_declareStringVar(valid, "picture", "");
    VarValidator_declareIntVar(valid, "border_left", 0);
    VarValidator_declareIntVar(valid, "border_right", 0);
    VarValidator_declareIntVar(valid, "border_top", 0);
    VarValidator_declareIntVar(valid, "border_bottom", 0);
    
    /*load*/
    VarValidator_validate(valid, v);
    VarValidator_del(valid);
    
    /*set*/
    GlSurface_del(tex->surf);
    tex->surf = GlSurface_newFromFile(Var_getValueString(Var_getArrayElemByCName(v, "picture")));
    tex->border_left = Var_getValueInt(Var_getArrayElemByCName(v, "border_left"));
    tex->border_right = Var_getValueInt(Var_getArrayElemByCName(v, "border_right"));
    tex->border_top = Var_getValueInt(Var_getArrayElemByCName(v, "border_top"));
    tex->border_bottom = Var_getValueInt(Var_getArrayElemByCName(v, "border_bottom"));
}

/*----------------------------------------------------------------------------*/
void
GuiTexture_draw(GuiTexture tex, GlSurface dest, GlRect destrect)
{
    GlRect rct_src;
    GlRect rct_dest;
    
    if ((tex->border_left + tex->border_right > destrect.w)
        || (tex->border_top + tex->border_bottom > destrect.h))
    {
        /*tiled copy of the background*/
        rct_src.w = GlSurface_getWidth(tex->surf);
        rct_dest.w = destrect.w;
        rct_src.h = GlSurface_getHeight(tex->surf);
        rct_dest.h = destrect.h;
        rct_src.x = 0;
        rct_dest.x = destrect.x;
        rct_src.y = 0;
        rct_dest.y = destrect.y;
        GlSurface_copyRectTiled(tex->surf, dest, &rct_src, &rct_dest);
        return;
    }
    
    /*copy the corners*/
    if ((tex->border_left != 0) && (tex->border_top != 0))
    {
        rct_src.w = rct_dest.w = tex->border_left;
        rct_src.h = rct_dest.h = tex->border_top;
        rct_src.x = rct_src.y = 0;
        rct_dest.x = destrect.x;
        rct_dest.y = destrect.y;
        GlSurface_copyRect(tex->surf, dest, &rct_src, &rct_dest);
    }
    if ((tex->border_left != 0) && (tex->border_bottom != 0))
    {
        rct_src.w = rct_dest.w = tex->border_left;
        rct_src.h = rct_dest.h = tex->border_bottom;
        rct_src.x = 0;
        rct_src.y = GlSurface_getHeight(tex->surf) - tex->border_bottom;
        rct_dest.x = destrect.x;
        rct_dest.y = destrect.y + destrect.h - rct_dest.h;
        GlSurface_copyRect(tex->surf, dest, &rct_src, &rct_dest);
    }
    if ((tex->border_right != 0) && (tex->border_top != 0))
    {
        rct_src.w = rct_dest.w = tex->border_right;
        rct_src.h = rct_dest.h = tex->border_top;
        rct_src.x = GlSurface_getWidth(tex->surf) - tex->border_right;
        rct_src.y = 0;
        rct_dest.x = destrect.x + destrect.w - rct_dest.w;
        rct_dest.y = destrect.y;
        GlSurface_copyRect(tex->surf, dest, &rct_src, &rct_dest);
    }
    if ((tex->border_right != 0) && (tex->border_bottom != 0))
    {
        rct_src.w = rct_dest.w = tex->border_right;
        rct_src.h = rct_dest.h = tex->border_bottom;
        rct_src.x = GlSurface_getWidth(tex->surf) - tex->border_right;
        rct_src.y = GlSurface_getHeight(tex->surf) - tex->border_bottom;
        rct_dest.x = destrect.x + destrect.w - rct_dest.w;
        rct_dest.y = destrect.y + destrect.h - rct_dest.h;
        GlSurface_copyRect(tex->surf, dest, &rct_src, &rct_dest);
    }
    
    /*tiled copy of borders (without the corners)*/
    if (tex->border_top != 0)
    {
        rct_src.w = GlSurface_getWidth(tex->surf) - tex->border_left - tex->border_right;
        rct_dest.w = destrect.w - tex->border_left - tex->border_right;
        rct_src.h = rct_dest.h = tex->border_top;
        rct_src.x = tex->border_left;
        rct_src.y = 0;
        rct_dest.x = destrect.x + rct_src.x;
        rct_dest.y = destrect.y;
        GlSurface_copyRectTiled(tex->surf, dest, &rct_src, &rct_dest);
    }
    if (tex->border_bottom != 0)
    {
        rct_src.w = GlSurface_getWidth(tex->surf) - tex->border_left - tex->border_right;
        rct_dest.w = destrect.w - tex->border_left - tex->border_right;
        rct_src.h = rct_dest.h = tex->border_bottom;
        rct_src.x = tex->border_left;
        rct_src.y = GlSurface_getHeight(tex->surf) - rct_src.h;
        rct_dest.x = destrect.x + rct_src.x;
        rct_dest.y = destrect.y + destrect.h - rct_dest.h;
        GlSurface_copyRectTiled(tex->surf, dest, &rct_src, &rct_dest);
    }
    if (tex->border_left != 0)
    {
        rct_src.h = GlSurface_getHeight(tex->surf) - tex->border_top - tex->border_bottom;
        rct_dest.h = destrect.h - tex->border_top - tex->border_bottom;
        rct_src.w = rct_dest.w = tex->border_left;
        rct_src.x = 0;
        rct_src.y = tex->border_top;
        rct_dest.x = destrect.x;
        rct_dest.y = destrect.y + tex->border_top;
        GlSurface_copyRectTiled(tex->surf, dest, &rct_src, &rct_dest);
    }
    if (tex->border_right != 0)
    {
        rct_src.h = GlSurface_getHeight(tex->surf) - tex->border_top - tex->border_bottom;
        rct_dest.h = destrect.h - tex->border_top - tex->border_bottom;
        rct_src.w = rct_dest.w = tex->border_right;
        rct_src.x = GlSurface_getWidth(tex->surf) - rct_src.w;
        rct_src.y = tex->border_top;
        rct_dest.x = destrect.x + destrect.w - rct_dest.w;
        rct_dest.y = destrect.y + tex->border_top;
        GlSurface_copyRectTiled(tex->surf, dest, &rct_src, &rct_dest);
    }
    
    /*tiled copy of the background*/
    rct_src.w = GlSurface_getWidth(tex->surf) - tex->border_left - tex->border_right;
    rct_dest.w = destrect.w - tex->border_left - tex->border_right;
    rct_src.h = GlSurface_getHeight(tex->surf) - tex->border_top - tex->border_bottom;
    rct_dest.h = destrect.h - tex->border_top - tex->border_bottom;
    rct_src.x = tex->border_left;
    rct_dest.x = destrect.x + tex->border_left;
    rct_src.y = tex->border_top;
    rct_dest.y = destrect.y + tex->border_top;
    GlSurface_copyRectTiled(tex->surf, dest, &rct_src, &rct_dest);
}
