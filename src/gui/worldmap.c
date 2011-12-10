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
 *   Graphical map to represent the whole world                               *
 *                                                                            *
  ***************************************************************************/


/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "gui/worldmap.h"

#include "graphics/color.h"
#include "graphics/graphics.h"
#include "graphics/glsurface.h"
#include "graphics/camera.h"
#include "graphics/glrect.h"
#include "gui/gui.h"
#include "gui/internal/guiwidget.h"
#include "gui/internal/guitooltip.h"

#include "core/core.h"
#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static WorldCoord map_w;
static WorldCoord map_h;
static MapPlot* map_plot;
static GlSurface map_surf;
static GuiLayout map_layout;
static GlColor map_color[7];
static GuiWidget _widget;
static GlRect rct_src;      /*rectangle inside the whole map*/
static GlRect rct_dest;     /*rectangle inside the gui surface*/

static Sint16 camposx;
static Sint16 camposy;
static Sint16 camlookx;
static Sint16 camlooky;

static CoreTime movetime;
static Bool dontmove;

static CoreID MOD_ID;
static CoreID FUNC_REDRAW;
static CoreID RESEXT_WORLDWIDTH;
static CoreID RESEXT_WORLDHEIGHT;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static GlColor
getCol(MapPlot plot)
{
    if (plot & MapPlot_CAMERA)
    {
        return map_color[6];
    }
    else if (plot & MapPlot_MAINUNIT)
    {
        return map_color[5];
    }
    else if (plot & MapPlot_DYNUNIT)
    {
        return map_color[4];
    }
    else if (plot & MapPlot_STATICUNIT)
    {
        return map_color[3];
    }
    else if (plot & MapPlot_PATH)
    {
        return map_color[2];
    }
    else if (plot & MapPlot_GROUND)
    {
        return map_color[1];
    }
    else
    {
        return map_color[0];
    }
}

/*----------------------------------------------------------------------------*/
/*Compute rectangles parts that will remain the same.
  Call this when the world size or the gui object size changed.*/
static void
precompRects()
{
    rct_src.x = 0;
    rct_src.y = 0;
    rct_src.w = rct_dest.w = MIN(map_w, GlSurface_getWidth(map_surf));
    rct_src.h = rct_dest.h = MIN(map_h, GlSurface_getHeight(map_surf));
    if (rct_dest.w < map_w)
    {
        rct_dest.x = 0;
    }
    else
    {
        rct_dest.x = (GlSurface_getWidth(map_surf) - map_w) / 2;
    }
    if (rct_dest.h < map_h)
    {
        rct_dest.y = 0;
    }
    else
    {
        rct_dest.y = (GlSurface_getHeight(map_surf) - map_h) / 2;
    }
}
    
/*----------------------------------------------------------------------------*/
static void
worldmapRedraw()
{
    WorldCoord x;
    WorldCoord y;
    float ang;
    float dist;
    
    GlSurface_clear(map_surf, map_color[0]);
    for (y = 0; y < rct_src.h; y++)
    {
        for (x = 0; x < rct_src.w; x++)
        {
            GlSurface_drawPixel(map_surf, rct_dest.x + x, rct_dest.y + y, getCol(map_plot[(rct_src.y + y) * map_w + (rct_src.x + x)]));
        }
    }
    
    /*TODO: maybe store ang and dist*/
    ang = angle2d((float)(camlookx - camposx), (float)(camlooky - camposy));
    dist = dist2d((float)camlookx, (float)camlooky, (float)camposx, (float)camposy);
    GlSurface_drawLineCut(map_surf, camposx - rct_src.x + rct_dest.x, camposy - rct_src.y + rct_dest.y, camposx + cos(ang + M_PI_4) * dist - rct_src.x + rct_dest.x, camposy + sin(ang + M_PI_4) * dist - rct_src.y + rct_dest.y, map_color[6]);
    GlSurface_drawLineCut(map_surf, camposx - rct_src.x + rct_dest.x, camposy - rct_src.y + rct_dest.y, camposx + cos(ang - M_PI_4) * dist - rct_src.x + rct_dest.x, camposy + sin(ang - M_PI_4) * dist - rct_src.y + rct_dest.y, map_color[6]);
    
    GuiWidget_redraw(_widget);
}

/*----------------------------------------------------------------------------*/
static Bool
gleventCallback(GlExtID data, GlEvent* event)
{
    (void)data;
    
    if (event->type == GLEVENT_CAMERA)
    {
        camposx = (Sint16)event->event.camevent.newcam.posx;
        camposy = (Sint16)event->event.camevent.newcam.posz;
        camlookx = (Sint16)event->event.camevent.newcam.lookx;
        camlooky = (Sint16)event->event.camevent.newcam.lookz;
        worldmapRedraw();
    }
    else if (event->type == GLEVENT_RESIZE)
    {
        map_layout.parentrect.w = event->event.resizeevent.width;
        map_layout.parentrect.h = event->event.resizeevent.height;
        GuiLayout_update(&map_layout);
        GuiWidget_topLevelSet(_widget, map_layout.layer, map_layout.rect);
    }
    else if (event->type == GLEVENT_MOUSE)
    {
        if (event->event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
        {
            dontmove = TRUE;
        }
        else if (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
        {
            dontmove = FALSE;
        }
        if (event->event.mouseevent.button == MOUSEBUTTON_LEFT)
        {
            cameraSetPos((Gl3DCoord)(event->event.mouseevent.x - rct_dest.x + rct_src.x),
                         0.0,
                         (Gl3DCoord)(event->event.mouseevent.y - rct_dest.y + rct_src.y));
        }
        else if (event->event.mouseevent.button == MOUSEBUTTON_RIGHT)
        {
            /*TODO*/
        }
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNC_REDRAW)
    {
        worldmapRedraw();
        Var_setVoid(func->ret);
    }
}

/*----------------------------------------------------------------------------*/
static void
resourceCallback(CoreID id, Var value)
{
    if (id == RESEXT_WORLDWIDTH)
    {
        ASSERT(Var_getType(value) == VAR_INT, return);
        
        map_w = Var_getValueInt(value);
        map_plot = REALLOC(map_plot, sizeof(MapPlot) * map_w * map_h);
        precompRects();
        worldmapClear();
    }
    else if (id == RESEXT_WORLDHEIGHT)
    {
        ASSERT(Var_getType(value) == VAR_INT, return);
        
        map_h = Var_getValueInt(value);
        map_plot = REALLOC(map_plot, sizeof(MapPlot) * map_w * map_h);
        precompRects();
        worldmapClear();
    }
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime duration)
{
    (void)thread;
    
    movetime += duration;
    if (movetime > 1000)
    {
        movetime = 50;
    }
    if (movetime >= 50)
    {
        Sint16 mx, my;
        Bool redraw;
        
        redraw = FALSE;
        movetime -= 50;
        
        if (dontmove)
        {
            return;
        }
        
        mx = rct_src.x + rct_src.w / 2;
        my = rct_src.y + rct_src.h / 2;
        if ((mx < camlookx) && (rct_src.x + rct_src.w < map_w))
        {
            rct_src.x++;
            redraw = TRUE;
        }
        if ((mx > camlookx) && (rct_src.x > 0))
        {
            rct_src.x--;
            redraw = TRUE;
        }
        if ((my < camlooky) && (rct_src.y + rct_src.h < map_h))
        {
            rct_src.y++;
            redraw = TRUE;
        }
        if ((my > camlooky) && (rct_src.y > 0))
        {
            rct_src.y--;
            redraw = TRUE;
        }
        if (redraw)
        {
            worldmapRedraw();
        }
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
worldmapInit()
{
    map_w = 10;
    map_h = 10;
    map_plot = MALLOC(sizeof(map_plot) * map_w * map_h);
    _widget = GuiWidget_new(NULL, NULL, _("World map (click to move the camera)"), FALSE, NULL, gleventCallback);
    map_surf = GuiWidget_getDrawingSurface(_widget);
    map_layout.dock = 5;
    map_layout.xoffset = 0;
    map_layout.yoffset = 0;
    map_layout.layer = 0;
    map_layout.parentrect.x = 0;
    map_layout.parentrect.y = 0;
    map_layout.rect.w = 10;
    map_layout.rect.h = 10;
    /*worldmapClear();*/
    
    graphicsAddEventCollector(GLEVENT_CAMERA, gleventCallback);
    
    camposx = 0;
    camposy = 0;
    camlookx = 0;
    camlooky = 0;
    
    movetime = 0;
    dontmove = FALSE;
    
    precompRects();
    
    MOD_ID = coreDeclareModule("map", NULL, NULL, shellCallback, NULL, resourceCallback, threadCallback);
    FUNC_REDRAW = coreDeclareShellFunction(MOD_ID, "redraw", VAR_VOID, 0);
    RESEXT_WORLDWIDTH = coreAddResourceWatcher(MOD_ID, "world_width");
    RESEXT_WORLDHEIGHT = coreAddResourceWatcher(MOD_ID, "world_height");
    coreRequireThreadSlot(MOD_ID, coreGetThreadID(NULL));
}

/*----------------------------------------------------------------------------*/
void
worldmapUninit()
{
    FREE(map_plot);
    GuiWidget_del(_widget);
    shellPrint(LEVEL_INFO, "Map unloaded.");
}

/*----------------------------------------------------------------------------*/
void
worldmapSet(Var varset)
{
    VarValidator valid;
    GlColorRGBA col;
    GlRect rct;
    
    valid = VarValidator_new();
    
    VarValidator_declareIntVar(valid, "width", 10);
    VarValidator_declareIntVar(valid, "height", 10);
    VarValidator_declareArrayVar(valid, "layout");
    VarValidator_declareArrayVar(valid, "col_void");
    VarValidator_declareArrayVar(valid, "col_ground");
    VarValidator_declareArrayVar(valid, "col_staticunit");
    VarValidator_declareArrayVar(valid, "col_dynunit");
    VarValidator_declareArrayVar(valid, "col_mainunit");
    VarValidator_declareArrayVar(valid, "col_camera");
    VarValidator_declareArrayVar(valid, "col_path");
    
    VarValidator_validate(valid, varset);
    VarValidator_del(valid);

    /*TODO: only change the tooltip instead of recreating the widget*/
    GuiWidget_del(_widget);    
    _widget = GuiWidget_new(NULL, NULL, _("World map (click to move the camera)"), FALSE, NULL, gleventCallback);
    map_surf = GuiWidget_getDrawingSurface(_widget);
    GlRect_MAKE(rct, 0, 0, Var_getValueInt(Var_getArrayElemByCName(varset, "width")), Var_getValueInt(Var_getArrayElemByCName(varset, "height")));
    GuiWidget_topLevelSet(_widget, 0, rct);
    map_surf = GuiWidget_getDrawingSurface(_widget);
    precompRects();
    
    GuiLayout_paramFromVar(&map_layout, Var_getArrayElemByCName(varset, "layout"));
    map_layout.rect.w = GlSurface_getWidth(map_surf);
    map_layout.rect.h = GlSurface_getHeight(map_surf);

    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_void"));
    map_color[0] = GlColorRGBA_to_GlColor(col);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_ground"));
    map_color[1] = GlColorRGBA_to_GlColor(col);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_path"));
    map_color[2] = GlColorRGBA_to_GlColor(col);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_staticunit"));
    map_color[3] = GlColorRGBA_to_GlColor(col);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_dynunit"));
    map_color[4] = GlColorRGBA_to_GlColor(col);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_mainunit"));
    map_color[5] = GlColorRGBA_to_GlColor(col);
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(varset, "col_camera"));
    map_color[6] = GlColorRGBA_to_GlColor(col);
    
    worldmapRedraw();
}

/*----------------------------------------------------------------------------*/
void
worldmapClear()
{
    WorldCoord x, y;
    
    for (y = 0; y < map_h; y++)
    {
        for (x = 0; x < map_w; x++)
        {
            map_plot[y * map_w + x] = MapPlot_VOID;
        }
    }
    
    worldmapRedraw();
}

/*----------------------------------------------------------------------------*/
void
worldmapSetPlot(WorldCoord x, WorldCoord y, MapPlot plot)
{
    ASSERT(x < map_w, return);
    ASSERT(y < map_h, return);
    
    map_plot[y * map_w + x] |= plot;
    if (isInRect(&rct_src, x, y))
    {
        GlSurface_drawPixel(map_surf, rct_dest.x + x - rct_src.x, rct_dest.y + y - rct_src.y, getCol(map_plot[y * map_w + x]));
        GuiWidget_redraw(_widget);
    }
}

/*----------------------------------------------------------------------------*/
void
worldmapUnsetPlot(WorldCoord x, WorldCoord y, MapPlot plot)
{
    ASSERT(x < map_w, return);
    ASSERT(y < map_h, return);
    
    map_plot[y * map_w + x] &= (~plot);
    if (isInRect(&rct_src, x, y))
    {
        GlSurface_drawPixel(map_surf, rct_dest.x + x, rct_dest.y + y, getCol(map_plot[(rct_src.y + y) * map_w + (rct_src.x + x)]));
        GuiWidget_redraw(_widget);
    }
}
