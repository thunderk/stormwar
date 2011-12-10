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
 *   Ground management                                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/ground.h"
#include "graphics/graphics.h"
#include "graphics/gl3dobject.h"
#include "graphics/glmesh.h"
#include "tools/fonct.h"
#include "tools/varvalidator.h"
#include "gui/worldmap.h"

#include "core/core.h"

/******************************************************************************
 *                                 Typedefs                                   *
 ******************************************************************************/
typedef struct
{
    Gl3DObject self;
    Gl3DObject topleft;
    Gl3DObject topright;
    Gl3DObject bottomleft;
    Gl3DObject bottomright;
} GroundUnit;

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static GroundUnit* _ground;
static GroundCoord _sizex;
static GroundCoord _sizey;
static GlMesh _mesh_self;
static GlMesh _mesh_side;
static GlMesh _mesh_cornerint;
static GlMesh _mesh_cornerext;

static CoreID MOD_ID;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
freeMeshes(void)
{
    if (_mesh_self != NULL)
    {
        GlMesh_del(_mesh_self);
        _mesh_self = NULL;
    }
    if (_mesh_side != NULL)
    {
        GlMesh_del(_mesh_side);
        _mesh_side = NULL;
    }
    if (_mesh_cornerint != NULL)
    {
        GlMesh_del(_mesh_cornerint);
        _mesh_cornerint = NULL;
    }
    if (_mesh_cornerext != NULL)
    {
        GlMesh_del(_mesh_cornerext);
        _mesh_cornerext = NULL;
    }
}

/*----------------------------------------------------------------------------*/
static void
datasCallback(Var datas)
{
    VarValidator valid;
    
    valid = VarValidator_new();
    
    VarValidator_declareArrayVar(valid, "mesh_full");
    VarValidator_declareArrayVar(valid, "mesh_side");
    VarValidator_declareArrayVar(valid, "mesh_cornerint");
    VarValidator_declareArrayVar(valid, "mesh_cornerext");
    
    VarValidator_validate(valid, datas);
    VarValidator_del(valid);

    freeMeshes();    
    _mesh_self = GlMesh_new(Var_getArrayElemByCName(datas, "mesh_full"));
    _mesh_side = GlMesh_new(Var_getArrayElemByCName(datas, "mesh_side"));
    _mesh_cornerint = GlMesh_new(Var_getArrayElemByCName(datas, "mesh_cornerint"));
    _mesh_cornerext = GlMesh_new(Var_getArrayElemByCName(datas, "mesh_cornerext"));
}

/*----------------------------------------------------------------------------*/
static void
adjustTile(GroundCoord x, GroundCoord y)
{
    unsigned int i;
    Bool pleft, pright, ptop, pbottom, ptopleft, ptopright, pbottomleft, pbottomright;
    
    ASSERT(x < _sizex, return);
    ASSERT(y < _sizey, return);
    
    i = y * _sizex + x;
    
    /*clear all corners*/
    if (_ground[i].topleft != NULL)
    {
        Gl3DObject_del(_ground[i].topleft);
        _ground[i].topleft = NULL;
    }
    if (_ground[i].topright != NULL)
    {
        Gl3DObject_del(_ground[i].topright);
        _ground[i].topright = NULL;
    }
    if (_ground[i].bottomleft != NULL)
    {
        Gl3DObject_del(_ground[i].bottomleft);
        _ground[i].bottomleft = NULL;
    }
    if (_ground[i].bottomright != NULL)
    {
        Gl3DObject_del(_ground[i].bottomright);
        _ground[i].bottomright = NULL;
    }
    
    if (_ground[i].self != NULL)
    {
        /*this ground unit is full, no room for borders*/
        return;
    }
    
    /*check neighbours*/
    if (x == 0)
    {
        pleft = FALSE;
    }
    else
    {
        pleft = (_ground[i - 1].self != NULL);
    }
    if (x == _sizex - 1)
    {
        pright = FALSE;
    }
    else
    {
        pright = (_ground[i + 1].self != NULL);
    }
    if (y == 0)
    {
        ptop = FALSE;
    }
    else
    {
        ptop = (_ground[i - _sizex].self != NULL);
    }
    if (y == _sizey - 1)
    {
        pbottom = FALSE;
    }
    else
    {
        pbottom = (_ground[i + _sizex].self != NULL);
    }
    if ((x == 0) || (y == 0))
    {
        ptopleft = FALSE;
    }
    else
    {
        ptopleft = (_ground[i - 1 - _sizex].self != NULL);
    }
    if ((x == _sizex - 1) || (y == 0))
    {
        ptopright = FALSE;
    }
    else
    {
        ptopright = (_ground[i + 1 - _sizex].self != NULL);
    }
    if ((x == 0) || (y == _sizey - 1))
    {
        pbottomleft = FALSE;
    }
    else
    {
        pbottomleft = (_ground[i - 1 + _sizex].self != NULL);
    }
    if ((x == _sizex - 1) || (y == _sizey - 1))
    {
        pbottomright = FALSE;
    }
    else
    {
        pbottomright = (_ground[i + 1 + _sizex].self != NULL);
    }
    
    /*topleft quarter*/
    if (ptop && pleft)
    {
        _ground[i].topleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topleft, _mesh_cornerint);
        Gl3DObject_setPos(_ground[i].topleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topleft, 0.0, 0.0, 0);
    }
    else if (ptop)
    {
        _ground[i].topleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topleft, _mesh_side);
        Gl3DObject_setPos(_ground[i].topleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topleft, 0.0, 0.0, 0);
    }
    else if (pleft)
    {
        _ground[i].topleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topleft, _mesh_side);
        Gl3DObject_setPos(_ground[i].topleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y + 0.5, 0);
        Gl3DObject_setAngle(_ground[i].topleft, -M_PI_2, 0.0, 0);
    }
    else if (ptopleft)
    {
        _ground[i].topleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topleft, _mesh_cornerext);
        Gl3DObject_setPos(_ground[i].topleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topleft, 0.0, 0.0, 0);
    }
    
    /*topright quarter*/
    if (ptop && pright)
    {
        _ground[i].topright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topright, _mesh_cornerint);
        Gl3DObject_setPos(_ground[i].topright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topright, M_PI_2, 0.0, 0);
    }
    else if (ptop)
    {
        _ground[i].topright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topright, _mesh_side);
        Gl3DObject_setPos(_ground[i].topright, (Gl3DCoord)x + 0.5, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topright, 0.0, 0.0, 0);
    }
    else if (pright)
    {
        _ground[i].topright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topright, _mesh_side);
        Gl3DObject_setPos(_ground[i].topright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topright, M_PI_2, 0.0, 0);
    }
    else if (ptopright)
    {
        _ground[i].topright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].topright, _mesh_cornerext);
        Gl3DObject_setPos(_ground[i].topright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y, 0);
        Gl3DObject_setAngle(_ground[i].topright, M_PI_2, 0.0, 0);
    }
    
    /*bottomleft quarter*/
    if (pbottom && pleft)
    {
        _ground[i].bottomleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomleft, _mesh_cornerint);
        Gl3DObject_setPos(_ground[i].bottomleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomleft, -M_PI_2, 0.0, 0);
    }
    else if (pbottom)
    {
        _ground[i].bottomleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomleft, _mesh_side);
        Gl3DObject_setPos(_ground[i].bottomleft, (Gl3DCoord)x + 0.5, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomleft, M_PI, 0.0, 0);
    }
    else if (pleft)
    {
        _ground[i].bottomleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomleft, _mesh_side);
        Gl3DObject_setPos(_ground[i].bottomleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomleft, -M_PI_2, 0.0, 0);
    }
    else if (pbottomleft)
    {
        _ground[i].bottomleft = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomleft, _mesh_cornerext);
        Gl3DObject_setPos(_ground[i].bottomleft, (Gl3DCoord)x, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomleft, -M_PI_2, 0.0, 0);
    }
    
    /*bottomright quarter*/
    if (pbottom && pright)
    {
        _ground[i].bottomright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomright, _mesh_cornerint);
        Gl3DObject_setPos(_ground[i].bottomright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomright, -M_PI, 0.0, 0);
    }
    else if (pbottom)
    {
        _ground[i].bottomright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomright, _mesh_side);
        Gl3DObject_setPos(_ground[i].bottomright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomright, M_PI, 0.0, 0);
    }
    else if (pright)
    {
        _ground[i].bottomright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomright, _mesh_side);
        Gl3DObject_setPos(_ground[i].bottomright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y + 0.5, 0);
        Gl3DObject_setAngle(_ground[i].bottomright, M_PI_2, 0.0, 0);
    }
    else if (pbottomright)
    {
        _ground[i].bottomright = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].bottomright, _mesh_cornerext);
        Gl3DObject_setPos(_ground[i].bottomright, (Gl3DCoord)x + 1.0, 0.0, (Gl3DCoord)y + 1.0, 0);
        Gl3DObject_setAngle(_ground[i].bottomright, -M_PI, 0.0, 0);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
groundInit()
{
    long i;
    
    _sizex = 10;
    _sizey = 10;
    _ground = MALLOC(sizeof(GroundUnit) * _sizex * _sizey);
    _mesh_self = NULL;
    _mesh_side = NULL;
    _mesh_cornerint = NULL;
    _mesh_cornerext = NULL;
    for (i = 0; i < _sizex * _sizey; i++)
    {
        _ground[i].self = NULL;
        _ground[i].topleft = NULL;
        _ground[i].topright = NULL;
        _ground[i].bottomleft = NULL;
        _ground[i].bottomright = NULL;
    }
    
    MOD_ID = coreDeclareModule("ground", NULL, datasCallback, NULL, NULL, NULL, NULL);
}

/*----------------------------------------------------------------------------*/
void
groundUninit()
{
    groundClear();
    freeMeshes();
    FREE(_ground);
    shellPrint(LEVEL_INFO, "Ground module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
groundWorldSizeChanged(GroundCoord nbx, GroundCoord nby)
{
    long i;
    
    groundClear();
    _sizex = nbx;
    _sizey = nby;
    _ground = REALLOC(_ground, sizeof(GroundUnit) * _sizex * _sizey);
    for (i = 0; i < _sizex * _sizey; i++)
    {
        _ground[i].self = NULL;
        _ground[i].topleft = NULL;
        _ground[i].topright = NULL;
        _ground[i].bottomleft = NULL;
        _ground[i].bottomright = NULL;
    }
}

/*----------------------------------------------------------------------------*/
void
groundClear()
{
    long i;
    
    /*TODO: unset map plots*/
    for (i = 0; i < _sizex * _sizey; i++)
    {
        if (_ground[i].self != NULL)
        {
            Gl3DObject_del(_ground[i].self);
            _ground[i].self = NULL;
        }
        if (_ground[i].topleft != NULL)
        {
            Gl3DObject_del(_ground[i].topleft);
            _ground[i].topleft = NULL;
        }
        if (_ground[i].topright != NULL)
        {
            Gl3DObject_del(_ground[i].topright);
            _ground[i].topright = NULL;
        }
        if (_ground[i].bottomleft != NULL)
        {
            Gl3DObject_del(_ground[i].bottomleft);
            _ground[i].bottomleft = NULL;
        }
        if (_ground[i].bottomright != NULL)
        {
            Gl3DObject_del(_ground[i].bottomright);
            _ground[i].bottomright = NULL;
        }
    }
}

/*----------------------------------------------------------------------------*/
Bool
groundAddPlayerArea(Uint16 minw, Uint16 minh, GroundCoord* rx, GroundCoord* ry)
{
    Uint8 areatested = 0x00;
    Uint8 areachosen = 0;
    GlRect rct;
    int x, y;
    int added, total, loop;
    Bool* buf;
    
    minw = MAX(minw, 1);
    minh = MAX(minh, 1);
    
    rct.w = _sizex / 2;
    rct.h = _sizey / 2;
    
    if ((minw > rct.w) | (minh > rct.h))
    {
        shellPrint(LEVEL_ERROR, "Unable to create player area; too big area required.");
        return TRUE;
    }
    
    /*choose an area*/
    rct.x = 0;
    rct.y = 0;
    do
    {
        areachosen = rnd(1, 4);
        if (((0x01 << (areachosen - 1)) & areatested) == 0x00)
        {
            switch (areachosen)
            {
                case 1:
                    rct.x = 0;
                    rct.y = 0;
                    break;
                case 2:
                    rct.x = _sizex - rct.w;
                    rct.y = 0;
                    break;
                case 3:
                    rct.x = 0;
                    rct.y = _sizey - rct.h;
                    break;
                case 4:
                    rct.x = _sizex - rct.w;
                    rct.y = _sizey - rct.h;
                    break;
                default:
                    /*should not happen*/
                    ;
            }
            if (groundGetState(rct.x + rct.w / 2, rct.y + rct.h / 2))
            {
                /*this area is already taken*/
                areachosen = 0;
                areatested &= (0x01 << (areachosen - 1));
                if (areatested == 0xFF)
                {
                    return TRUE;
                }
            }
        }
    } while (areachosen == 0);
   
    /*create a virtual growing area*/
    buf = MALLOC(sizeof(Bool) * rct.w * rct.h);
    for (x = 0; x < rct.w; x++)
    {
        for (y = 0; y < rct.h; y++)
        {
            buf[y * rct.w + x] = FALSE;
        }
    }
    
    /*place the minimal area*/
    total = 0;
    *rx = rct.x + (rct.w - minw) / 2;
    *ry = rct.y + (rct.h - minh) / 2;
    for (x = *rx; x < *rx + minw; x++)
    {
        for (y = *ry; y < *ry + minh; y++)
        {
            buf[(y - rct.y) * rct.w + (x - rct.x)] = FALSE;
            groundSetState(x, y, TRUE);
            total++;
        }
    }

    /*make it grow*/
    loop = 0;
    do
    {
        loop++;
        added = 0;
        /*seed propagation*/
        for (x = rct.x; x < rct.x + rct.w; x++)
        {
            for (y = rct.y; y < rct.y + rct.h; y++)
            {
                if (! buf[(y - rct.y) * rct.w + (x - rct.x)])
                {
                    int f;
                    int dx, dy;
                    
                    f = 0;
                    
                    for (dx = -1; dx <= 1; dx++)
                    {
                        for (dy = -1; dy <= 1; dy++)
                        {
                            if (((dx != 0) | (dy != 0)) && groundGetState(x + dx, y + dy))
                            {
                                f += (((dx == 0) | (dy == 0)) ? 2 : 1);    /*diagonal or not*/
                            }
                        }
                    }
                    
                    /*maximal f is 12*/
                    if (rnd(0, total) < f)
                    {
                        buf[(y - rct.y) * rct.w + (x - rct.x)] = TRUE;
                        added++;
                        total++;
                    }
                }
            }
        }
        /*build the new seeds*/
        for (x = 0; x < rct.w; x++)
        {
            for (y = 0; y < rct.h; y++)
            {
                if (buf[y * rct.w + x])
                {
                    groundSetState(x + rct.x, y + rct.y, TRUE);
                }
            }
        }
    } while ((loop < 50) && ((total < (rct.w * rct.h / 4)) | (added >= total / 8)));
    
    FREE(buf);
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
void
groundSetState(GroundCoord x, GroundCoord y, Bool ground)
{
    unsigned int i;
    
    if ((x >= _sizex) || (y >= _sizey))
    {
        return;
    }
    i = y * _sizex + x;
    
    if (ground)
    {
        if (_ground[i].self != NULL)
        {
            return;
        }
        
        /*set the tile itself*/
        _ground[i].self = Gl3DObject_new(NULL, global_groupnormal, NULL);
        Gl3DObject_setMesh(_ground[i].self, _mesh_self);
        Gl3DObject_setPos(_ground[i].self, (Gl3DCoord)x, 0.0, (Gl3DCoord)y, 0);
        
        /*change map plot*/
        worldmapSetPlot(x, y, MapPlot_GROUND);
    }
    else
    {
        /*delete the tile*/
        if (_ground[i].self != NULL)
        {
            Gl3DObject_del(_ground[i].self);
            _ground[i].self = NULL;
        
            /*change map plot*/
            worldmapUnsetPlot(x, y, MapPlot_GROUND);
        }
        else
        {
            return;
        }
    }
    
    /*correct the tile and its neighbours*/
    adjustTile(x, y);
    if (x > 0)
    {
        adjustTile(x - 1, y);
    }
    if (x < _sizex - 1)
    {
        adjustTile(x + 1, y);
    }
    if (y > 0)
    {
        adjustTile(x, y - 1);
    }
    if (y < _sizey - 1)
    {
        adjustTile(x, y + 1);
    }
    if ((x > 0) && (y > 0))
    {
        adjustTile(x - 1, y - 1);
    }
    if ((x < _sizex - 1) && (y > 0))
    {
        adjustTile(x + 1, y - 1);
    }
    if ((x > 0) && (y < _sizey - 1))
    {
        adjustTile(x - 1, y + 1);
    }
    if ((x < _sizex - 1) && (y < _sizey - 1))
    {
        adjustTile(x + 1, y + 1);
    }
}

/*----------------------------------------------------------------------------*/
Bool
groundGetState(GroundCoord x, GroundCoord y)
{
    unsigned int i;
    
    if ((x >= _sizex) || (y >= _sizey))
    {
        return FALSE;
    }
    i = y * _sizex + x;
    
    return (_ground[i].self != NULL);
}
