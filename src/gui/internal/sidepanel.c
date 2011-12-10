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
 *   Panel that stands on the left of the screen.                             *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/internal/sidepanel.h"

#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

#include "gui/guitexture.h"
#include "core/core.h"

#include "graphics/gl2dobject.h"
#include "graphics/glrect.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static GuiTexture tex;

static GlSurface draw_surf;

static Gl2DObject globj;

static GlRect _entities_rect;
static Uint16 _entities_bbottom;
static Uint16 _entity_h;
static Uint16 _entity_w;

static int _nbentities;             /*number of available entities*/
static Var _entities;               /*names of available entities*/
static Var _entities_assoc;         /*association between name and pictures*/
static GlSurface* _entities_pict;   /*scaled pictures*/
static int _entity_selected;        /*entity currently selected, -1 for none*/

/*core things*/
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID RES_ENTITIES = CORE_INVALID_ID;
static CoreID RES_ENTITY_SELECTED = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static void
sidepanelDraw()
{
    GlRect src, dest;
    int i;
    
    /*background*/
    GlSurface_getRect(draw_surf, &dest);
    GuiTexture_draw(tex, draw_surf, dest);
    
    /*entities*/
    src.x = src.y = 0;
    src.w = dest.w = _entities_rect.w;
    dest.y = _entities_rect.y;
    dest.x = _entities_rect.x;
    for (i = 0; i < _nbentities; i++)
    {
        if (dest.y > _entities_rect.y + _entities_rect.h)
        {
            break;
        }
        src.h = dest.h = imin(_entity_h, _entities_rect.y + _entities_rect.h - dest.y);
        GlSurface_doBlit(_entities_pict[i], draw_surf, &src, &dest);
        dest.y += _entity_h;
    }
    
    Gl2DObject_redraw(globj);
}

/*----------------------------------------------------------------------------*/
/*correct _entities_pict to match _entities*/
static void correctEntities()
{
    int i;
    String s;
    Var v;
    
    if (_nbentities != 0)
    {
        for (i = 0; i < _nbentities; i++)
        {
            GlSurface_del(_entities_pict[i]);
        }
        FREE(_entities_pict);
        _entities_pict = NULL;
    }
    
    _nbentities = Var_getArraySize(_entities);
    if (_nbentities != 0)
    {
        _entities_pict = MALLOC(sizeof(GlSurface) * _nbentities);
        for (i = 0; i < _nbentities; i++)
        {
            v = Var_getArrayElemByPos(_entities, i);
            if (Var_getType(v) == VAR_STRING)
            {
                s = Var_getValueString(v);
                v = Var_getArrayElemByName(_entities_assoc, s);
                if ((v == NULL) || (Var_getType(v) != VAR_STRING))
                {
                    shellPrintf(LEVEL_ERROR, "Entity '%s' not found (or bad path) by the sidepanel.", String_get(s));
                    _entities_pict[i] = GlSurface_new(_entities_rect.w, _entity_h, FALSE);
                }
                else
                {
                    _entities_pict[i] = GlSurface_newFromFile(Var_getValueString(v));
                    if ((GlSurface_getWidth(_entities_pict[i]) != _entities_rect.w)
                      | (GlSurface_getHeight(_entities_pict[i]) != _entity_h))
                    {
                        GlSurface_resize(_entities_pict[i], _entities_rect.w, _entity_h, SURFACE_RESAMPLE);
                    }
                }
            }
            else
            {
                _entities_pict[i] = GlSurface_new(_entities_rect.w, _entity_h, FALSE);
                /*TODO: error*/
            }
        }
    }
    
    sidepanelDraw();
}

/*----------------------------------------------------------------------------*/
static void
resourceCallback(CoreID id, Var value)
{
    if (id == RES_ENTITIES)
    {
        if (Var_getType(value) == VAR_ARRAY)
        {
            Var_setFromVar(_entities, value);
            correctEntities();
        }
        else
        {
            /*TODO: error*/
        }
    }
    else if (id == RES_ENTITY_SELECTED)
    {
        ASSERT(Var_getType(value) == VAR_INT, return);
        
        _entity_selected = Var_getValueInt(value);
        /*TODO: graphical acknowledgement*/
    }
}

/*----------------------------------------------------------------------------*/
static Bool
gleventCallback(GlExtID data, GlEvent* event)
{
    Sint16 dy;
    
    (void)data;
    
    if ((event->type == GLEVENT_MOUSE) && (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED))
    {
        dy = event->event.mouseevent.y - Gl2DObject_getY(globj) - _entities_rect.y;
        if ((dy >= 0) & (dy < _entities_rect.h) & (dy < _nbentities * _entity_h) & (_entity_selected != dy / _entity_h))
        {
            Var_setInt(temp_corevar, dy / _entity_h);
        }
        else
        {
            Var_setInt(temp_corevar, -1);
        }
        coreSetResourceValue(MOD_ID, RES_ENTITY_SELECTED, temp_corevar);
        return TRUE;
    }
    
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                            Sidepanel functions                           #*
 *############################################################################*
 ******************************************************************************/
void
sidepanelInit()
{
    /*Arbitrary values*/
    draw_surf = GlSurface_new(10, 10, TRUE);
    globj = Gl2DObject_new(draw_surf, NULL, gleventCallback);
    Gl2DObject_setAlt(globj, GL2D_LOWEST);
    tex = GuiTexture_new();
    
    _nbentities = 0;
    _entities = Var_new();
    Var_setArray(_entities);
    _entities_assoc = Var_new();
    Var_setArray(_entities_assoc);
    _entities_pict = NULL;
    
    GlRect_MAKE(_entities_rect, 0, 0, 10, 10);
    _entities_bbottom = 0;
    _entity_w = 10;
    _entity_h = 1;
    
    _entity_selected = -1;
    
    MOD_ID = coreDeclareModule("sidepanel", NULL, NULL, NULL, NULL, resourceCallback, NULL);
    RES_ENTITIES = coreAddResourceWatcher(MOD_ID, "entities_available");
    RES_ENTITY_SELECTED = coreAddResourceWatcher(MOD_ID, "entity_selected");
}

/*----------------------------------------------------------------------------*/
void
sidepanelUninit()
{
    int i;
    
    if (_nbentities != 0)
    {
        for (i = 0; i < _nbentities; i++)
        {
            GlSurface_del(_entities_pict[i]);
        }
        FREE(_entities_pict);
        _nbentities = 0;
    }
    Var_del(_entities);
    Var_del(_entities_assoc);
    
    GuiTexture_del(tex);
    Gl2DObject_del(globj);
    GlSurface_del(draw_surf);
    shellPrint(LEVEL_INFO, "Sidepanel unloaded.");
}

/*----------------------------------------------------------------------------*/
void
sidepanelSet(Var varset)
{
    VarValidator valid;
    Uint16 w;
    
    valid = VarValidator_new();

    VarValidator_declareIntVar(valid, "width", 10);
    VarValidator_declareArrayVar(valid, "texture");
    VarValidator_declareArrayVar(valid, "entities_pict");
    VarValidator_declareIntVar(valid, "entities_bleft", 0);
    VarValidator_declareIntVar(valid, "entities_btop", 0);
    VarValidator_declareIntVar(valid, "entities_bbottom", 0);
    VarValidator_declareIntVar(valid, "entity_h", 1);
    VarValidator_declareIntVar(valid, "entity_w", 1);
    
    VarValidator_validate(valid, varset);
    VarValidator_del(valid);

    w = Var_getValueInt(Var_getArrayElemByCName(varset, "width"));

    GlSurface_resize(draw_surf, w, GlSurface_getHeight(draw_surf), SURFACE_UNDEFINED);
    Gl2DObject_relink(globj, draw_surf);

    GuiTexture_set(tex, Var_getArrayElemByCName(varset, "texture"));
    
    _entity_h = Var_getValueInt(Var_getArrayElemByCName(varset, "entity_h"));
    _entity_w = Var_getValueInt(Var_getArrayElemByCName(varset, "entity_w"));
    
    /*TODO: check bleft against entity_w */
    _entities_rect.x = Var_getValueInt(Var_getArrayElemByCName(varset, "entities_bleft"));
    _entities_rect.y = Var_getValueInt(Var_getArrayElemByCName(varset, "entities_btop"));
    _entities_bbottom = Var_getValueInt(Var_getArrayElemByCName(varset, "entities_bbottom"));
    _entities_rect.w = _entity_w;
    
    Var_setFromVar(_entities_assoc, Var_getArrayElemByCName(varset, "entities_pict"));
    
    if (_entities_rect.y + _entities_bbottom > GlSurface_getHeight(draw_surf))
    {
        _entities_rect.h = 0;
    }
    else
    {
        _entities_rect.h = GlSurface_getHeight(draw_surf) - _entities_bbottom - _entities_rect.y;
    }
    
    correctEntities();
}

/*----------------------------------------------------------------------------*/
void
sidepanelSetHeight(Uint16 h)
{
    GlSurface_resize(draw_surf, GlSurface_getWidth(draw_surf), h, SURFACE_UNDEFINED);
    Gl2DObject_relink(globj, draw_surf);
    
    if (_entities_rect.y + _entities_bbottom > GlSurface_getHeight(draw_surf))
    {
        _entities_rect.h = 0;
    }
    else
    {
        _entities_rect.h = GlSurface_getHeight(draw_surf) - _entities_bbottom - _entities_rect.y;
    }
    
    sidepanelDraw();
}

/*----------------------------------------------------------------------------*/
void
sidepanelSetYPos(Sint16 y)
{
    Gl2DObject_setPos(globj, 0, y, 0);
}
