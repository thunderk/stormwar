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
 *   Graphical appearance and behavior of a 3d object                         *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/glmesh.h"

#include "graphics/impl/glmeshpart.h"
#include "graphics/impl/opengl.h"
#include "graphics/impl/glscreen.h"
#include "graphics/glrect.h"
#include "graphics/impl/impl.h"
#include "tools/anim.h"

#include "core/string.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    Gl3DCoord x;
    Gl3DCoord y;
    Gl3DCoord z;
    Gl3DCoord angh;
    Gl3DCoord angv;
} PartPlace;

struct pv_GlMesh
{
    Uint16 nbparts;         /*number of parts*/
    GlMeshPart* parts;      /*NULL terminated array of parts*/
    Uint16 nbctrlpoints;    /*number of control points*/
    Gl3DCoord* ctrlpoints;  /*control points (used for visibility and selection)*/
    Gl3DCoord* ctrlfback;   /*feedback of control points*/
    PtrArray anims;         /*animation data*/
};

struct pv_GlMeshControl
{
    AnimControl animcontrol;
    PartPlace* parts_place; /*array of parts position and direction (match 'parts' size, except the ending NULL)*/
    int nbloops;
    CoreTime totaltime;
    CoreTime curtime;
};

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
void
GlMesh_draw(GlMesh mesh, GlMeshControl control, GlMeshInfo* info)
{
    GlMeshPart* it_part;
    PartPlace* it_place;
    Uint16 i, n;
    Gl3DCoord xmin, xmax, ymin, ymax, zmin, zmax;
    
    ASSERT(mesh != NULL, return);
    ASSERT(control != NULL, return);

    if ((!global_pause) && (control->nbloops >=0))
    {
        /*Update animation controller*/
        ASSERT(control->animcontrol != NULL, return);
        control->curtime += global_frametime;
        if (control->curtime >= control->totaltime)
        {
            if (control->nbloops == 0)
            {
                /*infinite loop*/
                if (control->totaltime == 0)
                {
                    control->curtime = 0;
                }
                else
                {
                    control->curtime %= control->totaltime;
                }
            }
            else
            {
                /*do loops*/
                while ((control->nbloops > 0) && (control->curtime > control->totaltime))
                {
                    control->curtime -= control->totaltime;
                    control->nbloops--;
                }
                if (control->nbloops == 0)
                {
                    /*last loop reached*/
                    control->curtime = control->totaltime;
                    control->nbloops = -1;
                }
            }
        }
        AnimControl_update(control->animcontrol, control->curtime);
    }

    /*Check control points*/
    if (mesh->nbctrlpoints != 0)
    {
        if (info->check && (info->forcecheck || (global_checkcount++ < 50)))
        {
            info->check = FALSE;
            glFeedbackBuffer(mesh->nbctrlpoints * 4, GL_3D, mesh->ctrlfback);
            glRenderMode(GL_FEEDBACK);
            
            glBegin(GL_POINTS);
            for (i = 0; i < mesh->nbctrlpoints; i++)
            {
                glVertex3fv(mesh->ctrlpoints + i * 3);
            }
            glEnd();
            
            n = glRenderMode(GL_RENDER);
            if (n != 0)
            {
                xmax = -1.0;
                xmin = (Gl3DCoord)global_screenwidth;
                ymax = -1.0;
                ymin = (Gl3DCoord)global_screenheight;
                zmax = -1000000.0f;
                zmin = 1000000.0f;
                for (i = 0; i < n; i += 4)
                {
                    ASSERT_WARN(floatEqual(mesh->ctrlfback[i], GL_POINT_TOKEN));
                    xmax = MAX(mesh->ctrlfback[i + 1], xmax);
                    xmin = MIN(mesh->ctrlfback[i + 1], xmin);
                    ymax = MAX(mesh->ctrlfback[i + 2], ymax);
                    ymin = MIN(mesh->ctrlfback[i + 2], ymin);
                    zmax = MAX(mesh->ctrlfback[i + 3], zmax);
                    zmin = MIN(mesh->ctrlfback[i + 3], zmin);
                }
                /*TODO: maybe rounds*/
                info->rct.x = (Gl2DCoord)xmin;
                info->rct.w = (Gl2DSize)(xmax - xmin) + 1;
                info->rct.y = global_screenheight - 1 - (Gl2DCoord)ymax;
                info->rct.h = (Gl2DSize)(ymax - ymin) + 1;
                info->z = (zmax + zmin) / 2.0;
                info->drawn = TRUE;
            }
            else
            {
                GlRect_MAKE(info->rct, -1, -1, 1, 1);
                info->drawn = FALSE;
                return;
            }
        }
        else if (!info->drawn)
        {
            return;
        }
    }
    else
    {
        GlRect_MAKE(info->rct, -1, -1, 1, 1);
    }
    
    /*Render*/
    it_part = mesh->parts;
    it_place = control->parts_place;
    while (*it_part != NULL)
    {
        cameraPushObject(it_place->x, it_place->y, it_place->z, it_place->angh, it_place->angv);
        GlMeshPart_draw(*it_part);
        cameraPopObject();
        it_part++;
        it_place++;
    }
}

/*----------------------------------------------------------------------------*/
void
GlMesh_makeControl(GlMesh mesh, GlMeshControl* control_p)
{
    AnimTrack i;
    
    if (mesh == NULL)
    {
        if (*control_p != NULL)
        {
            AnimControl_del((*control_p)->animcontrol);
            FREE((*control_p)->parts_place);
            FREE(*control_p);
            *control_p = NULL;
        }
    }
    else
    {
        if (*control_p == NULL)
        {
            *control_p = MALLOC(sizeof(pv_GlMeshControl));
            (*control_p)->animcontrol = AnimControl_new(mesh->nbparts * 5);
            (*control_p)->parts_place = MALLOC(sizeof(PartPlace) * mesh->nbparts * 5);
        }
        else
        {
            AnimControl_del((*control_p)->animcontrol);
            (*control_p)->animcontrol = AnimControl_new(mesh->nbparts * 5);
            (*control_p)->parts_place = REALLOC((*control_p)->parts_place, sizeof(PartPlace) * mesh->nbparts * 5);
        }
        
        for (i = 0; i < mesh->nbparts; i++)
        {
            AnimControl_setFloatControl((*control_p)->animcontrol, i * 5, &(*control_p)->parts_place[i].x);
            AnimControl_setFloatControl((*control_p)->animcontrol, i * 5 + 1, &(*control_p)->parts_place[i].y);
            AnimControl_setFloatControl((*control_p)->animcontrol, i * 5 + 2, &(*control_p)->parts_place[i].z);
            AnimControl_setFloatControl((*control_p)->animcontrol, i * 5 + 3, &(*control_p)->parts_place[i].angh);
            AnimControl_setFloatControl((*control_p)->animcontrol, i * 5 + 4, &(*control_p)->parts_place[i].angv);
            (*control_p)->parts_place[i].x = 0.0f;
            (*control_p)->parts_place[i].y = 0.0f;
            (*control_p)->parts_place[i].z = 0.0f;
            (*control_p)->parts_place[i].angh = 0.0f;
            (*control_p)->parts_place[i].angv = 0.0f;
        }
        (*control_p)->nbloops = -1;
    }
}

/*----------------------------------------------------------------------------*/
void
GlMesh_linkControl(GlMesh mesh, GlMeshControl control, String anim, CoreTime time, unsigned int repeats)
{
    Anim as;
    PtrArrayIterator ai;
    Anim a;
    
    ASSERT(control != NULL, return);
    
    /*search anim*/
    as = Anim_new(anim, 0);
    ai = PtrArray_findSorted(mesh->anims, as);
    
    /*link the control to it*/
    if (ai != NULL)
    {
        a = (Anim)*ai;
        control->totaltime = AnimControl_linkToAnim(control->animcontrol, a);
        control->nbloops = repeats;
        control->curtime = time;
    }
    
    Anim_del(as);
}

/*----------------------------------------------------------------------------*/
static void
GlMesh_setFromVar(GlMesh mesh, Var var)
{
    VarValidator valid, validpart;
    Var vanims, v, vv;
    unsigned int i, j;
    unsigned int n;
    
    /*validate variable*/
    valid = VarValidator_new();
    VarValidator_declareArrayVar(valid, "parts");
    VarValidator_declareArrayVar(valid, "anim");
    VarValidator_declareArrayVar(valid, "controlpoints");
    VarValidator_validate(valid, var);
    VarValidator_del(valid);
    
    validpart = VarValidator_new();
    VarValidator_declareArrayVar(validpart, "x");
    VarValidator_declareArrayVar(validpart, "y");
    VarValidator_declareArrayVar(validpart, "z");
    VarValidator_declareArrayVar(validpart, "angh");
    VarValidator_declareArrayVar(validpart, "angv");
    
    /*create control points*/
    v = Var_getArrayElemByCName(var, "controlpoints");
    n = Var_getArraySize(v);
    mesh->nbctrlpoints = n / 3;
    /*TODO: warning if number is incorrect*/
    if (mesh->nbctrlpoints != 0)
    {
        mesh->ctrlpoints = MALLOC(sizeof(Gl3DCoord) * mesh->nbctrlpoints * 3);
        mesh->ctrlfback = MALLOC(sizeof(Gl3DCoord) * mesh->nbctrlpoints * 4);
        for (i = 0; (int)i < mesh->nbctrlpoints * 3; i++)
        {
            vv = Var_getArrayElemByPos(v, i);
            if (Var_getType(vv) != VAR_FLOAT)
            {
                /*TODO: error*/
            }
            else
            {
                mesh->ctrlpoints[i] = Var_getValueFloat(vv);
            }
        }
    }
    
    /*create new mesh parts*/
    n = Var_getArraySize(Var_getArrayElemByCName(var, "parts"));
    mesh->nbparts = n;
    mesh->parts = MALLOC(sizeof(GlMeshPart) * (n + 1));
    mesh->parts[n] = NULL;
    for (i = 0; i < n; i++)
    {
        mesh->parts[i] = GlMeshPart_new(Var_getArrayElemByPos(Var_getArrayElemByCName(var, "parts"), i));
    }
    
    /*create animations*/
    PtrArray_clear(mesh->anims);
    vanims = Var_getArrayElemByCName(var, "anim");
    for (i = 0; i < Var_getArraySize(vanims); i++)
    {
        Anim a;
        Var vanim;
        Var vpart;
        
        vanim = Var_getArrayElemByPos(vanims, i);
        if (Var_getType(vanim) != VAR_ARRAY)
        {
            /*TODO: error*/
        }
        else
        {
            a = Anim_new(Var_getName(vanim), n * 5);
            
            j = 0;
            while (j < n)
            {
                if (j >= Var_getArraySize(vanim))
                {
                    shellPrintf(LEVEL_ERROR, "No animation data found for part %d of '%s' mesh for '%s' anim.", j, String_get(Var_getName(var)), String_get(Var_getName(vanim)));
                }
                else
                {
                    vpart = Var_getArrayElemByPos(vanim, j);
                    VarValidator_validate(validpart, vpart);
                    Anim_setTrackFromVar(a, j * 5, Var_getArrayElemByCName(vpart, "x"));
                    Anim_setTrackFromVar(a, j * 5 + 1, Var_getArrayElemByCName(vpart, "y"));
                    Anim_setTrackFromVar(a, j * 5 + 2, Var_getArrayElemByCName(vpart, "z"));
                    Anim_setTrackFromVar(a, j * 5 + 3, Var_getArrayElemByCName(vpart, "angh"));
                    Anim_setTrackFromVar(a, j * 5 + 4, Var_getArrayElemByCName(vpart, "angv"));
                }
                j++;
            }
            
            PtrArray_insertSorted(mesh->anims, a);
        }
    }
    
    VarValidator_del(validpart);
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
GlMesh
GlMesh_new(Var var)
{
    GlMesh ret;
    
    ret = (GlMesh)MALLOC(sizeof(pv_GlMesh));
    ret->anims = PtrArray_newFull(5, 3, (PtrFunc)Anim_del, (PtrCmpFunc)Anim_cmp);
    GlMesh_setFromVar(ret, var);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GlMesh_del(GlMesh mesh)
{
    GlMeshPart* i;
    
    if (mesh->nbctrlpoints != 0)
    {
        FREE(mesh->ctrlpoints);
        FREE(mesh->ctrlfback);
    }
    PtrArray_del(mesh->anims);
    for (i = mesh->parts; *i != NULL; i++)
    {
        GlMeshPart_del(*i);
    }
    FREE(mesh->parts);
    FREE(mesh);
}
