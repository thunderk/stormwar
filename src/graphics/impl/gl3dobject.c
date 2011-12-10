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
 *   Graphical 3d object                                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/gl3dobject.h"

#include "graphics/graphics.h"
#include "graphics/glmesh.h"
#include "graphics/color.h"
#include "graphics/glrect.h"
#include "graphics/impl/opengl.h"
#include "graphics/impl/impl.h"
#include "graphics/camera.h"

#include "tools/anim.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define ANIM_POS 0
#define ANIM_ANG 1
#define ANIM_COL 2
#define ANIM_NB 3

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure for a Gl3DObject.
 */
struct pv_Gl3DObject
{
    GlExtID extid;              /*!< External identifier. */
    Gl3DGroup group;            /*!< Object's group. */
    Bool visible;               /*!< Visible or not. */
    Bool zsorted;               /*!< Z-sorting is needed or not. */
    GlMesh mesh;                /*!< Link to the mesh containing graphical data for this object. */
    GlMeshControl meshcontrol;  /*!< Mesh controller. */
    GlMeshInfo info;
    
    Anim anim[ANIM_NB];                 /*!< Animations for internal values. */
    AnimControl animcontrol[ANIM_NB];   /*!< Animation controllers for internal values. */
    CoreTime animtotaltime[ANIM_NB];    /*!< Total animation time for internal anims. */
    CoreTime animcurtime[ANIM_NB];      /*!< Current animation time for internal anims. */
    
    Gl3DCoord x;                /*!< Current X position in the world. */
    Gl3DCoord y;                /*!< Current Y position in the world. */
    Gl3DCoord z;                /*!< Current Z position in the world. */
    Gl3DCoord angh;             /*!< Horizontal angle. */
    Gl3DCoord angv;             /*!< Vertical angle. */
    Float color[4];             /*!< Color. */
    GlEventCallback eventcb;    /*!< Callback function to execute for catched events. */
};

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
Bool
Gl3DObject_processEvent(Gl3DObject obj, GlEvent* event)
{
    GlEvent ev;
    int i;
    Bool ret;
    
    switch (event->type)
    {
        case GLEVENT_DRAW:
            /*do animations*/
            for (i = 0; i < ANIM_NB; i++)
            {
                if (obj->anim[i] != NULL)
                {
                    obj->animcurtime[i] += event->event.frameduration;
                    AnimControl_update(obj->animcontrol[i], obj->animcurtime[i]);
                    if (i < 2)
                    {
                        obj->info.drawn = TRUE;
                        obj->info.check = TRUE;
                    }
                    if (obj->animcurtime[i] >= obj->animtotaltime[i])
                    {
                        /*end animation*/
                        Anim_del(obj->anim[i]);
                        obj->anim[i] = NULL;
                    }
                }
            }

            /*draw*/
            ret = FALSE;
            if ((obj->mesh != NULL) && (obj->visible))
            {
                cameraPushObject(obj->x, obj->y, obj->z, obj->angh, obj->angv);
                glColor4fv(obj->color);
                if (global_cammoved)
                {
                    obj->info.drawn = TRUE;
                    obj->info.check = TRUE;
                }
                ret = obj->info.check;
                GlMesh_draw(obj->mesh, obj->meshcontrol, &obj->info);
                obj->info.forcecheck = obj->zsorted;
                cameraPopObject();
            }
            else
            {
                /*TODO: do this only if needed*/
                GlRect_MAKE(obj->info.rct, -1, -1, 1, 1);
            }
            
            /*throw event*/
            if (obj->eventcb != NULL)
            {
                ev.type = GLEVENT_DRAW;
                ev.event.frameduration = event->event.frameduration;
                obj->eventcb(obj->extid, &ev);
            }
            
            return ret;
            
            
        case GLEVENT_DELETE:
            /*pass the event*/    
            if (obj->eventcb != NULL)
            {
                ev.type = GLEVENT_DELETE;
                obj->eventcb(obj->extid, &ev);
            }
            
            /*free memory*/
            Gl3DObject_setMesh(obj, NULL);  /*to destroy the mesh controller and the link*/
            for (i = 0; i < ANIM_NB; i++)
            {
                AnimControl_del(obj->animcontrol[i]);
                if (obj->anim[i] != NULL)
                {
                    Anim_del(obj->anim[i]);
                }
            }
            FREE(obj);
            return FALSE;
        default:
            ;
    }
    return FALSE;
}

/*----------------------------------------------------------------------------*/
int
Gl3DObject_cmp(Gl3DObject* obj1, Gl3DObject* obj2)
{
    if ((*obj1)->info.z < (*obj2)->info.z)
    {
        return 1;
    }
    else if ((*obj1)->info.z > (*obj2)->info.z)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Gl3DObject
Gl3DObject_new(GlExtID extid, Gl3DGroup group, GlEventCallback callback)
{
    Gl3DObject ret;
    
    ret = (Gl3DObject)MALLOC(sizeof(pv_Gl3DObject));
    ret->extid = extid;
    ret->group = group;
    ret->mesh = NULL;
    ret->meshcontrol = NULL;
    ret->visible = TRUE;
    ret->zsorted = graphicsIsGroupZSorted(group);
    GlRect_MAKE(ret->info.rct, -1, -1, 1, 1);
    ret->info.check = TRUE;
    ret->info.drawn = TRUE;
    ret->info.forcecheck = ret->zsorted;
    ret->info.z = 0.0;
    
    ret->anim[ANIM_POS] = NULL;
    ret->animcontrol[ANIM_POS] = AnimControl_new(3);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_POS], 0, &ret->x);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_POS], 1, &ret->y);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_POS], 2, &ret->z);
    
    ret->anim[ANIM_ANG] = NULL;
    ret->animcontrol[ANIM_ANG] = AnimControl_new(2);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_ANG], 0, &ret->angh);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_ANG], 1, &ret->angv);

    ret->anim[ANIM_COL] = NULL;
    ret->animcontrol[ANIM_COL] = AnimControl_new(4);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_COL], 0, ret->color + 0);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_COL], 1, ret->color + 1);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_COL], 2, ret->color + 2);
    AnimControl_setFloatControl(ret->animcontrol[ANIM_COL], 3, ret->color + 3);
    
    ret->x = 0.0;
    ret->y = 0.0;
    ret->z = 0.0;
    ret->angh = 0.0;
    ret->angv = 0.0;
    ret->color[0] = 1.0;
    ret->color[1] = 1.0;
    ret->color[2] = 1.0;
    ret->color[3] = 1.0;
    ret->eventcb = callback;
    
    graphicsAdd3DObject(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_del(Gl3DObject obj)
{
    graphicsDel3DObject(obj);
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_setData(Gl3DObject obj, GlExtID extid)
{
    obj->extid = extid;
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_show(Gl3DObject obj, Bool visible)
{
    obj->info.drawn = TRUE;
    obj->info.check = TRUE;
    obj->visible = visible;
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_setMesh(Gl3DObject obj, GlMesh mesh)
{
    obj->mesh = mesh;
    obj->info.drawn = TRUE;
    obj->info.check = TRUE;
    GlMesh_makeControl(mesh, &obj->meshcontrol);
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_setColor(Gl3DObject obj, GlColorRGBA col, CoreTime duration)
{
    if (obj->anim[ANIM_COL] != NULL)
    {
        /*there is already an anim*/
        Anim_del(obj->anim[ANIM_COL]);
        obj->anim[ANIM_COL] = NULL;
    }
    
    if (duration == 0)
    {
        obj->color[0] = (Float)col.r / 255.0f;
        obj->color[1] = (Float)col.g / 255.0f;
        obj->color[2] = (Float)col.b / 255.0f;
        obj->color[3] = (Float)col.a / 255.0f;
    }
    else
    {
        obj->anim[ANIM_COL] = Anim_new(NULL, 4);
        Anim_addFloatFrame(obj->anim[ANIM_COL], 0, 0, obj->color[0]);
        Anim_addFloatFrame(obj->anim[ANIM_COL], 0, 1, obj->color[1]);
        Anim_addFloatFrame(obj->anim[ANIM_COL], 0, 2, obj->color[2]);
        Anim_addFloatFrame(obj->anim[ANIM_COL], 0, 3, obj->color[3]);
        Anim_addFloatFrame(obj->anim[ANIM_COL], duration, 0, (Float)col.r / 255.0);
        Anim_addFloatFrame(obj->anim[ANIM_COL], duration, 1, (Float)col.g / 255.0);
        Anim_addFloatFrame(obj->anim[ANIM_COL], duration, 2, (Float)col.b / 255.0);
        Anim_addFloatFrame(obj->anim[ANIM_COL], duration, 3, (Float)col.a / 255.0);
        obj->animtotaltime[ANIM_COL] = AnimControl_linkToAnim(obj->animcontrol[ANIM_COL], obj->anim[ANIM_COL]);
        obj->animcurtime[ANIM_COL] = 0;
    }
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_setAnim(Gl3DObject obj, String anim, CoreTime time, unsigned int repeats)
{
    if (obj->mesh != NULL)
    {
        GlMesh_linkControl(obj->mesh, obj->meshcontrol, anim, time, repeats);
    }
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_setPos(Gl3DObject obj, Gl3DCoord x, Gl3DCoord y, Gl3DCoord z, CoreTime duration)
{
    if (obj->anim[ANIM_POS] != NULL)
    {
        /*there is already an anim*/
        Anim_del(obj->anim[ANIM_POS]);
        obj->anim[ANIM_POS] = NULL;
    }
    
    if (duration == 0)
    {
        obj->x = x;
        obj->y = y;
        obj->z = z;
        obj->info.drawn = TRUE;
        obj->info.check = TRUE;
    }
    else
    {
        obj->anim[ANIM_POS] = Anim_new(NULL, 3);
        Anim_addFloatFrame(obj->anim[ANIM_POS], 0, 0, obj->x);
        Anim_addFloatFrame(obj->anim[ANIM_POS], 0, 1, obj->y);
        Anim_addFloatFrame(obj->anim[ANIM_POS], 0, 2, obj->z);
        Anim_addFloatFrame(obj->anim[ANIM_POS], duration, 0, x);
        Anim_addFloatFrame(obj->anim[ANIM_POS], duration, 1, y);
        Anim_addFloatFrame(obj->anim[ANIM_POS], duration, 2, z);
        obj->animtotaltime[ANIM_POS] = AnimControl_linkToAnim(obj->animcontrol[ANIM_POS], obj->anim[ANIM_POS]);
        obj->animcurtime[ANIM_POS] = 0;
    }
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_setAngle(Gl3DObject obj, Gl3DCoord angh, Gl3DCoord angv, CoreTime duration)
{
    if (obj->anim[ANIM_ANG] != NULL)
    {
        /*there is already an anim*/
        Anim_del(obj->anim[ANIM_ANG]);
        obj->anim[ANIM_ANG] = NULL;
    }
    
    if (duration == 0)
    {
        obj->angh = angh;
        obj->angv = angv;
        obj->info.drawn = TRUE;
        obj->info.check = TRUE;
    }
    else
    {
        obj->anim[ANIM_ANG] = Anim_new(NULL, 2);
        Anim_addFloatFrame(obj->anim[ANIM_ANG], 0, 0, obj->angh);
        Anim_addFloatFrame(obj->anim[ANIM_ANG], 0, 1, obj->angv);
        Anim_addFloatFrame(obj->anim[ANIM_ANG], duration, 0, angh);
        Anim_addFloatFrame(obj->anim[ANIM_ANG], duration, 1, angv);
        obj->animtotaltime[ANIM_ANG] = AnimControl_linkToAnim(obj->animcontrol[ANIM_ANG], obj->anim[ANIM_ANG]);
        obj->animcurtime[ANIM_ANG] = 0;
    }
}

/*----------------------------------------------------------------------------*/
Bool
Gl3DObject_isVisible(Gl3DObject obj)
{
    return obj->visible;
}

/*----------------------------------------------------------------------------*/
GlMesh
Gl3DObject_getMesh(Gl3DObject obj)
{
    return obj->mesh;
}

/*----------------------------------------------------------------------------*/
Gl3DGroup
Gl3DObject_getGroup(Gl3DObject obj)
{
    return obj->group;
}

/*----------------------------------------------------------------------------*/
GlRect
Gl3DObject_getRect(Gl3DObject obj)
{
    obj->info.forcecheck = TRUE;
    return obj->info.rct;
}

/*----------------------------------------------------------------------------*/
void
Gl3DObject_getPos(Gl3DObject obj, Gl3DCoord* x, Gl3DCoord* y, Gl3DCoord* z)
{
    *x = obj->x;
    *y = obj->y;
    *z = obj->z;
}

/*----------------------------------------------------------------------------*/
Gl3DCoord
Gl3DObject_getX(Gl3DObject obj)
{
    return obj->x;
}

/*----------------------------------------------------------------------------*/
Gl3DCoord
Gl3DObject_getY(Gl3DObject obj)
{
    return obj->y;
}

/*----------------------------------------------------------------------------*/
Gl3DCoord
Gl3DObject_getZ(Gl3DObject obj)
{
    return obj->z;
}

/*----------------------------------------------------------------------------*/
Gl3DCoord
Gl3DObject_getAngh(Gl3DObject obj)
{
    return obj->angh;
}

/*----------------------------------------------------------------------------*/
Gl3DCoord
Gl3DObject_getAngv(Gl3DObject obj)
{
    return obj->angv;
}
