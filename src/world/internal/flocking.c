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
 *   Module that controls the flocking of entities (such as fishes, birds...) *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/internal/flocking.h"

#include "core/core.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"

#include "graphics/graphics.h"
#include "graphics/gl3dobject.h"
#include "graphics/glmesh.h"
#include "graphics/types.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Boid (single element of a flocking system) */
typedef struct
{
    Gl3DObject obj;
    Gl3DCoord dx;
    Gl3DCoord dy;
    Gl3DCoord dz;
} Boid;

struct pv_BoidGroup
{
    int xrel:1;         /*!< Specifies if xmin and xmax field are relative to world's xmin and xmax */
    int zrel:1;         /*!< Specifies if zmin and zmax field are relative to world's zmin and zmax */
    float response;     /*!< Global responsiveness of a boid to the flocking rules */
    Gl3DCoord xmin;     /*!< X lower limit */
    Gl3DCoord xmax;     /*!< X upper limit */
    Gl3DCoord ymin;     /*!< Y lower limit */
    Gl3DCoord ymax;     /*!< Y upper limit */
    Gl3DCoord zmin;     /*!< Z lower limit */
    Gl3DCoord zmax;     /*!< Z upper limit */
    Gl3DCoord xmul;     /*!< X multiplier */
    Gl3DCoord ymul;     /*!< Y multiplier */
    Gl3DCoord zmul;     /*!< Z multiplier */
    GlMesh mesh;        /*!< Mesh for an entity of this group */
    Uint32 nbboids;     /*!< Number of boids in this group */
    Boid* boids;        /*!< Vector of boids */
};

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static PtrArray _groups;

static WorldCoord world_w;
static WorldCoord world_h;

static CoreID MOD_ID = CORE_INVALID_ID;
static volatile CoreID THREAD_ID = CORE_INVALID_ID;

#define THREAD_TIMER 30

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
BoidGroup_del(BoidGroup group)
{
    Uint32 i;
    
    if (group->nbboids > 0)
    {
        for (i = 0; i < group->nbboids; i++)
        {
            Gl3DObject_del((group->boids)[i].obj);
        }
        FREE(group->boids);
    }
    if (group->mesh != NULL)
    {
        GlMesh_del(group->mesh);
    }
    FREE(group);
}
/*----------------------------------------------------------------------------*/
static void
applyFlock(Boid* boid, BoidGroup group, Uint32 pos, Gl3DCoord f)
{
    Gl3DCoord ndx, ndy, ndz;
    Gl3DCoord mx, my, mz;
    Gl3DCoord mdx, mdy, mdz;
    Gl3DCoord ox, oy, oz;
    Uint32 i, n;
    Boid* b;
    Gl3DCoord fa;
    Bool avoid = FALSE;
    
    Gl3DObject_getPos(boid->obj, &ox, &oy, &oz);
    
    /*avoidance*/
    if (ox < group->xmin + 1.0)
    {
        boid->dx += (group->xmin + 1.0 - ox) * 0.01;
        avoid = TRUE;
    }
    if (ox > group->xmax - 1.0)
    {
        boid->dx += (group->xmax - 1.0 - ox) * 0.01;
        avoid = TRUE;
    }
    if (oy < group->ymin + 1.0)
    {
        boid->dy += (group->ymin + 1.0 - oy) * 0.01;
        avoid = TRUE;
    }
    if (oy > group->ymax - 1.0)
    {
        boid->dy += (group->ymax - 1.0 - oy) * 0.01;
        avoid = TRUE;
    }
    if (oz < group->zmin + 1.0)
    {
        boid->dz += (group->zmin + 1.0 - oz) * 0.01;
        avoid = TRUE;
    }
    if (oz > group->zmax - 1.0)
    {
        boid->dz += (group->zmax - 1.0 - oz) * 0.01;
        avoid = TRUE;
    }
    if (avoid)
    {
        Gl3DObject_setPos(boid->obj, ox + boid->dx * f, oy + boid->dy * f, oz + boid->dz * f, 0);
        return;
    }

    /*make sum of neighborhood factors*/
    mx = 0.0;
    my = 0.0;
    mz = 0.0;
    mdx = 0.0;
    mdy = 0.0;
    mdz = 0.0;
    n = 0;
    for (i = 0; i < group->nbboids; i++)
    {
        if (i != pos)
        {
            b = (group->boids) + i;
            if ((fabs(ox - Gl3DObject_getX(b->obj)) < 5.0)
             && (fabs(oy - Gl3DObject_getY(b->obj)) < 5.0)
             && (fabs(oz - Gl3DObject_getZ(b->obj)) < 5.0))
            {
                /*b is in the neighborhood*/
                mx += Gl3DObject_getX(b->obj);
                my += Gl3DObject_getY(b->obj);
                mz += Gl3DObject_getZ(b->obj);
                mdx += b->dx;
                mdy += b->dy;
                mdz += b->dz;
                n++;
            }
        }
    }
    if (n != 0)
    {
        mx /= (Gl3DCoord)n;
        my /= (Gl3DCoord)n;
        mz /= (Gl3DCoord)n;
        mdx /= (Gl3DCoord)n;
        mdy /= (Gl3DCoord)n;
        mdz /= (Gl3DCoord)n;
        
        fa = 1.0;
        
        ndx = boid->dx;
        ndy = boid->dy;
        ndz = boid->dz;
        /*fa += dist3d(0.0, 0.0, 0.0, boid->dx, boid->dy, boid->dz);*/

        /*cohesion*/
        mx -= ox;
        my -= oy;
        mz -= oz;
        ndx += 0.001 * mx;
        ndy += 0.001 * my;
        ndz += 0.001 * mz;
        fa += dist3d(0.0, 0.0, 0.0, 0.001 * mx, 0.001 * my, 0.001 * mz);
        
        /*alignement*/
        ndx += mdx;
        ndy += mdy;
        ndz += mdz;
        fa += dist3d(0.0, 0.0, 0.0, mdx, mdy, mdz);
        
        /*multiplier*/
        ndx *= group->xmul;
        ndy *= group->ymul;
        ndz *= group->zmul;
        
        /*normalize new direction*/
        ndx /= fa;
        ndy /= fa;
        ndz /= fa;
        ndx = MIN(ndx, 0.1);
        ndx = MAX(ndx, -0.1);
        ndy = MIN(ndy, 0.1);
        ndy = MAX(ndy, -0.1);
        ndz = MIN(ndz, 0.1);
        ndz = MAX(ndz, -0.1);
    
        /*steer towards this direction*/
        boid->dx += (ndx - boid->dx) * group->response;
        boid->dy += (ndy - boid->dy) * group->response;
        boid->dz += (ndz - boid->dz) * group->response;
    }
    
    Gl3DObject_setPos(boid->obj, ox + boid->dx * f, oy + boid->dy * f, oz + boid->dz * f, 0);
}

/*----------------------------------------------------------------------------*/
static void
boidRandom(Boid* boid, BoidGroup group)
{
    /*TODO: better random*/
    Gl3DObject_setPos(boid->obj, (Gl3DCoord)rnd(group->xmin, group->xmax - group->xmin), (Gl3DCoord)rnd(group->ymin, group->ymax - group->ymin), (Gl3DCoord)rnd(group->zmin, group->zmax - group->zmin), 0);
    boid->dx = (Gl3DCoord)(rnd(0, 100) - 50) / 500.0;
    boid->dy = (Gl3DCoord)(rnd(0, 100) - 50) / 500.0;
    boid->dz = (Gl3DCoord)(rnd(0, 100) - 50) / 500.0;
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime steptime)
{
    PtrArrayIterator i;
    BoidGroup group;
    Uint32 j;
    Boid* boid;
    Gl3DCoord f;
    Gl3DCoord ah, av;
    
    ASSERT(thread == THREAD_ID, return);
    (void)thread;
    
    f = (Gl3DCoord)steptime / THREAD_TIMER;
    f = MIN(f, 3.0);
    
    for (i = PtrArray_START(_groups); i != PtrArray_STOP(_groups); i++)
    {
        group = (BoidGroup)*i;
        for (j = 0; j < group->nbboids; j++)
        {
            boid = group->boids + j;
            applyFlock(boid, group, j, f);
            angle3d(boid->dx, boid->dy, boid->dz, &ah, &av);
            Gl3DObject_setAngle(boid->obj, ah, av, 0);
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
coreCallback(CoreEvent event)
{
    if (event == CORE_PAUSE)
    {
        corePauseThread(MOD_ID, THREAD_ID);
    }
    else if (event == CORE_RESUME)
    {
        coreResumeThread(MOD_ID, THREAD_ID);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
flockingInit()
{
    _groups = PtrArray_newFull(2, 2, (PtrFunc)BoidGroup_del, NULL);

    world_w = 10;
    world_h = 10;
    
    MOD_ID = coreDeclareModule("flocking", coreCallback, NULL, NULL, NULL, NULL, threadCallback);
    coreCreateThread(MOD_ID, "flocks", FALSE, &THREAD_ID);
    coreSetThreadTimer(MOD_ID, THREAD_ID, THREAD_TIMER);
}

/*----------------------------------------------------------------------------*/
void
flockingUninit()
{
    PtrArray_del(_groups);
    
    shellPrint(LEVEL_INFO, "Flocking module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
flockingClear()
{
    PtrArray_clear(_groups);
}

/*----------------------------------------------------------------------------*/
BoidGroup
flockingAddGroup(Var params)
{
    VarValidator valid;
    BoidGroup ret;
    Uint32 i;
    Boid* b;
    
    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "number", 0);
    VarValidator_declareArrayVar(valid, "mesh");
    VarValidator_declareFloatVar(valid, "xmin", -10.0);
    VarValidator_declareFloatVar(valid, "xmax", 10.0);
    VarValidator_declareFloatVar(valid, "ymin", 4.0);
    VarValidator_declareFloatVar(valid, "ymax", 20.0);
    VarValidator_declareFloatVar(valid, "zmin", -10.0);
    VarValidator_declareFloatVar(valid, "zmax", 10.0);
    VarValidator_declareFloatVar(valid, "xmul", 1.0);
    VarValidator_declareFloatVar(valid, "ymul", 1.0);
    VarValidator_declareFloatVar(valid, "zmul", 1.0);
    VarValidator_declareIntVar(valid, "xrel", 1);
    VarValidator_declareIntVar(valid, "zrel", 1);
    VarValidator_declareFloatVar(valid, "response", 0.1);
    VarValidator_declareStringVar(valid, "anim", "");
    VarValidator_validate(valid, params);
    VarValidator_del(valid);
    
    ret = MALLOC(sizeof(pv_BoidGroup));
    
    ret->mesh = NULL;
    
    ret->nbboids = Var_getValueInt(Var_getArrayElemByCName(params, "number"));

    if (ret->nbboids > 0)
    {
        ret->boids = MALLOC(sizeof(Boid) * ret->nbboids);
        
        /*group parameters*/
        ret->mesh = GlMesh_new(Var_getArrayElemByCName(params, "mesh"));
        ret->xmin = Var_getValueFloat(Var_getArrayElemByCName(params, "xmin"));
        ret->xmax = Var_getValueFloat(Var_getArrayElemByCName(params, "xmax"));
        ret->ymin = Var_getValueFloat(Var_getArrayElemByCName(params, "ymin"));
        ret->ymax = Var_getValueFloat(Var_getArrayElemByCName(params, "ymax"));
        ret->zmin = Var_getValueFloat(Var_getArrayElemByCName(params, "zmin"));
        ret->zmax = Var_getValueFloat(Var_getArrayElemByCName(params, "zmax"));
        ret->xmul = Var_getValueFloat(Var_getArrayElemByCName(params, "xmul"));
        ret->ymul = Var_getValueFloat(Var_getArrayElemByCName(params, "ymul"));
        ret->zmul = Var_getValueFloat(Var_getArrayElemByCName(params, "zmul"));
        ret->response = Var_getValueFloat(Var_getArrayElemByCName(params, "response"));
        ret->xrel = Var_getValueInt(Var_getArrayElemByCName(params, "xrel"));
        ret->zrel = Var_getValueInt(Var_getArrayElemByCName(params, "zrel"));
        
        /*correct X and Z limits if they are relative*/
        if (ret->xrel)
        {
            ret->xmax += (Gl3DCoord)world_w;
        }
        if (ret->zrel)
        {
            ret->zmax += (Gl3DCoord)world_h;
        }
        
        /*generate boids*/
        for (i = 0; i < ret->nbboids; i++)
        {
            b = ret->boids + i;
            b->obj = Gl3DObject_new(NULL, global_groupnormal, NULL);
            Gl3DObject_setMesh(b->obj, ret->mesh);
            Gl3DObject_setAnim(b->obj, Var_getValueString(Var_getArrayElemByCName(params, "anim")), rnd(0, 1000), 0);
            boidRandom(b, ret);
        }
    }
    
    PtrArray_append(_groups, ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
flockingDelGroup(BoidGroup group)
{
    PtrArray_removeFast(_groups, group);
}

/*----------------------------------------------------------------------------*/
void
flockingWorldSizeChanged(WorldCoord w, WorldCoord h)
{
    PtrArrayIterator i;
    BoidGroup group;
    unsigned int j;
    
    /*correct relative limits for each group*/
    for (i = PtrArray_START(_groups); i != PtrArray_STOP(_groups); i++)
    {
        group = (BoidGroup)*i;
        if (group->xrel)
        {
            group->xmax = group->xmax - (Gl3DCoord)world_w + (Gl3DCoord)w;
        }
        if (group->zrel)
        {
            group->zmax = group->zmax - (Gl3DCoord)world_h + (Gl3DCoord)h;
        }
        for (j = 0; j < group->nbboids; j++)
        {
            boidRandom(group->boids + j, group);
        }
    }

    world_w = w;
    world_h = h;
}
