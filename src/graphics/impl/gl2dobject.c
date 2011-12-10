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
 *   Graphical 2d object                                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/gl2dobject.h"

#include "graphics/impl/impl.h"
#include "graphics/impl/opengl.h"
#include "graphics/color.h"
#include "graphics/glrect.h"
#include "graphics/glsurface.h"
#include "graphics/impl/gltextures.h"

#include "tools/fonct.h"
#include "tools/anim.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief State of a 2D object.
 */
typedef enum
{
    STATE2D_OK,         /*!< Not modified, will be drawn. */
    STATE2D_HIDDEN,     /*!< Not drawn (parts are good but textures aren't uploaded in OpenGL) */
    STATE2D_HIDE,       /*!< The object needs to be hidden (must clear OpenGL textures) */
    STATE2D_REDRAW,     /*!< Need to reupload the textures to OpenGL (parts are good). */
    STATE2D_RELINK,     /*!< Parts will be adjusted (or created) and textures uploaded to OpenGL. */
    STATE2D_RELINKHIDE  /*!< Parts will be adjusted (or created) but textures not uploaded. */
} Gl2DState;

/*!
 * \brief Private structure for a Gl2DObject.
 */
struct pv_Gl2DObject
{
    Gl2DState state;            /*!< Current state of the object. */
    GlSurface surf;             /*!< Surface containing graphical data. */
    Gl2DCoord x;                /*!< Current X position, relative to the screen. */
    Gl2DCoord y;                /*!< Current Y position, relative to the screen. */
    Gl2DSize z;                 /*!< Current Z position (altitude). */
    Bool zmoved;                /*!< Object has been moved along z axis (need z-sorting again). */
    
    Anim animpos;               /*!< Animation for position. */
    AnimControl animposcontrol; /*!< Animation controller for position. */
    CoreTime animpostotaltime;  /*!< Pos animation total time. */
    CoreTime animpostime;       /*!< Pos animation current time. */
    
    Float col[4];               /*!< Global color. */

    Anim animcol;               /*!< Animation for color. */
    AnimControl animcolcontrol; /*!< Animation controller for color. */
    CoreTime animcoltotaltime;  /*!< Color animation total time. */
    CoreTime animcoltime;       /*!< Color animation current time. */
    
    GlEventCallback eventcb;    /*!< Callback function to execute for catched events. */
    GlExtID extid;              /*!< Additional data to pass to the callback. */
    
    Uint16 nbparts;             /*!< Total number of parts. */
    GlLinkedTexture tex;        /*!< Linked texture. */
};

/******************************************************************************
 *                                  Statics                                   *
 ******************************************************************************/
/*values used for anim*/
static Int _v0, _v1;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
Bool
Gl2DObject_processEvent(Gl2DObject obj, GlEvent* event)
{
    unsigned int i;
    GlEvent ev;
    TexRenderInfo* info;
    
    switch (event->type)
    {
        case GLEVENT_DELETE:
            /*pass the event*/    
            ev = *event;
            obj->eventcb(obj->extid, &ev);
            
            /*free memory*/
            GlLinkedTexture_del(obj->tex);
            AnimControl_del(obj->animposcontrol);
            if (obj->animpos != NULL)
            {
                Anim_del(obj->animpos);
            }
            AnimControl_del(obj->animcolcontrol);
            if (obj->animcol != NULL)
            {
                Anim_del(obj->animcol);
            }
            FREE(obj);
            return TRUE;
        
        case GLEVENT_MOUSE:
        case GLEVENT_KEYBOARD:
        case GLEVENT_RESIZE:
            ev = *event;
            return obj->eventcb(obj->extid, &ev);
        
        case GLEVENT_DRAW:
            ASSERT_MAINTHREAD;
            
            /*throw event*/
            ev.type = GLEVENT_DRAW;
            ev.event.frameduration = event->event.frameduration;
            obj->eventcb(obj->extid, &ev);
            
            /*state change*/
            if (obj->state == STATE2D_RELINK)
            {
                obj->nbparts = GlLinkedTexture_linkToSurf(obj->tex, obj->surf);
                obj->state = STATE2D_OK;
            }
            else if (obj->state == STATE2D_REDRAW)
            {
                GlLinkedTexture_updateLink(obj->tex);
                obj->state = STATE2D_OK;
            }
            else if (obj->state == STATE2D_RELINKHIDE)
            {
                obj->nbparts = GlLinkedTexture_linkToSurf(obj->tex, obj->surf);
                obj->state = STATE2D_HIDDEN;
            }
            else if (obj->state == STATE2D_HIDE)
            {
                obj->state = STATE2D_HIDDEN;
            }
            
            /*do animations*/
            if (obj->animpos != NULL)
            {
                obj->animpostime += event->event.frameduration;
                AnimControl_update(obj->animposcontrol, obj->animpostime);
                obj->x = (Gl2DCoord)_v0;
                obj->y = (Gl2DCoord)_v1;
                if (obj->animpostime >= obj->animpostotaltime)
                {
                    /*end animation*/
                    Anim_del(obj->animpos);
                    obj->animpos = NULL;
                }
            }
            if (obj->animcol != NULL)
            {
                obj->animcoltime += event->event.frameduration;
                AnimControl_update(obj->animcolcontrol, obj->animcoltime);
                if (obj->animcoltime >= obj->animcoltotaltime)
                {
                    /*end animation*/
                    Anim_del(obj->animcol);
                    obj->animcol = NULL;
                }
            }
            
            /*drawing*/
            if (obj->state == STATE2D_OK)
            {
                glColor4fv(obj->col);
                for (i = 0; i < obj->nbparts; i++)
                {
                    info = GlLinkedTexture_use(obj->tex);
                    glBegin(GL_QUADS);
                    
                    glTexCoord2f(info->texcoords[0], info->texcoords[1]);
                    glVertex3i(obj->x + info->rct.x, obj->y + info->rct.y, obj->z);
                    
                    glTexCoord2f(info->texcoords[2], info->texcoords[3]);
                    glVertex3i(obj->x + info->rct.x + info->rct.w, obj->y + info->rct.y, obj->z);
                    
                    glTexCoord2f(info->texcoords[4], info->texcoords[5]);
                    glVertex3i(obj->x + info->rct.x + info->rct.w, obj->y + info->rct.y + info->rct.h, obj->z);
                    
                    glTexCoord2f(info->texcoords[6], info->texcoords[7]);
                    glVertex3i(obj->x + info->rct.x, obj->y + info->rct.y + info->rct.h, obj->z);
                    
                    glEnd();
                    openglCount(1);
                }
            }
            if (obj->zmoved)
            {
                obj->zmoved = FALSE;
                return TRUE;
            }
        default:
            ;
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
int
Gl2DObject_cmp(Gl2DObject* obj1, Gl2DObject* obj2)
{
    return (*obj1)->z - (*obj2)->z;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Gl2DObject
Gl2DObject_new(GlSurface surf, GlExtID extid, GlEventCallback callback)
{
    Gl2DObject ret;
    
    ASSERT_CRITICAL(surf != NULL);
    ASSERT_CRITICAL(callback != NULL);
    
    ret = (Gl2DObject)MALLOC(sizeof(pv_Gl2DObject));
    ret->state = STATE2D_OK;
    ret->surf = surf;
    ret->x = 0;
    ret->y = 0;
    ret->z = 0;
    ret->zmoved = TRUE;
    ret->animpos = NULL;
    ret->animposcontrol = AnimControl_new(2);
    AnimControl_setIntControl(ret->animposcontrol, 0, &_v0);
    AnimControl_setIntControl(ret->animposcontrol, 1, &_v1);
    ret->col[0] = 1.0f;
    ret->col[1] = 1.0f;
    ret->col[2] = 1.0f;
    ret->col[3] = 1.0f;
    ret->animcol = NULL;
    ret->animcolcontrol = AnimControl_new(4);
    AnimControl_setFloatControl(ret->animcolcontrol, 0, ret->col);
    AnimControl_setFloatControl(ret->animcolcontrol, 1, ret->col + 1);
    AnimControl_setFloatControl(ret->animcolcontrol, 2, ret->col + 2);
    AnimControl_setFloatControl(ret->animcolcontrol, 3, ret->col + 3);
    ret->eventcb = callback;
    ret->extid = extid;
    ret->tex = GlLinkedTexture_new(surf, &ret->nbparts);
    
    graphicsAdd2DObject(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_del(Gl2DObject obj)
{
    graphicsDel2DObject(obj);
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_show(Gl2DObject obj, Bool visible)
{
    ASSERT(obj->surf != NULL, return);
    
    if (visible)
    {
        if ((obj->state == STATE2D_HIDDEN) || (obj->state == STATE2D_HIDE))
        {
            obj->state = STATE2D_REDRAW;
        }
        else if (obj->state == STATE2D_RELINKHIDE)
        {
            obj->state = STATE2D_RELINK;
        }
    }
    else
    {
        if ((obj->state == STATE2D_OK) || (obj->state == STATE2D_REDRAW))
        {
            obj->state = STATE2D_HIDE;
        }
        else if (obj->state == STATE2D_RELINK)
        {
            obj->state = STATE2D_RELINKHIDE;
        }
    }
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_redraw(Gl2DObject obj)
{
    ASSERT(obj->surf != NULL, return)
    
    if (obj->state == STATE2D_OK)
    {
        obj->state = STATE2D_REDRAW;
    }
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_relink(Gl2DObject obj, GlSurface surf)
{
    ASSERT(obj->surf != NULL, return)
    
    obj->surf = surf;
    if ((obj->state == STATE2D_HIDE) || (obj->state == STATE2D_HIDDEN) || (obj->state == STATE2D_RELINKHIDE))
    {
        obj->state = STATE2D_RELINKHIDE;
    }
    else /*OK or REDRAW or RELINK*/
    {
        obj->state = STATE2D_RELINK;
    }
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_setColor(Gl2DObject obj, GlColorRGBA col, CoreTime duration)
{
    if (obj->animcol != NULL)
    {
        /*there is already an anim*/
        Anim_del(obj->animcol);
        obj->animcol = NULL;
    }
    
    if (duration == 0)
    {
        obj->col[0] = (Float)col.r / 255.0;
        obj->col[1] = (Float)col.g / 255.0;
        obj->col[2] = (Float)col.b / 255.0;
        obj->col[3] = (Float)col.a / 255.0;
        return;
    }

    /*setting animation*/
    obj->animcol = Anim_new(NULL, 4);
    Anim_addFloatFrame(obj->animcol, 0, 0, obj->col[0]);
    Anim_addFloatFrame(obj->animcol, 0, 1, obj->col[1]);
    Anim_addFloatFrame(obj->animcol, 0, 2, obj->col[2]);
    Anim_addFloatFrame(obj->animcol, 0, 3, obj->col[3]);
    Anim_addFloatFrame(obj->animcol, duration, 0, (Float)col.r / 255.0);
    Anim_addFloatFrame(obj->animcol, duration, 1, (Float)col.g / 255.0);
    Anim_addFloatFrame(obj->animcol, duration, 2, (Float)col.b / 255.0);
    Anim_addFloatFrame(obj->animcol, duration, 3, (Float)col.a / 255.0);
    obj->animcoltotaltime = AnimControl_linkToAnim(obj->animcolcontrol, obj->animcol);
    obj->animcoltime = 0;
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_setPos(Gl2DObject obj, Gl2DCoord x, Gl2DCoord y, CoreTime duration)
{
    if (obj->animpos != NULL)
    {
        /*there is already an anim*/
        Anim_del(obj->animpos);
        obj->animpos = NULL;
    }
    
    if (duration == 0)
    {
        obj->x = x;
        obj->y = y;
        return;
    }

    /*setting animation*/
    obj->animpos = Anim_new(NULL, 2);
    Anim_addIntFrame(obj->animpos, 0, 0, (Int)(obj->x));
    Anim_addIntFrame(obj->animpos, 0, 1, (Int)(obj->y));
    Anim_addIntFrame(obj->animpos, duration, 0, (Int)x);
    Anim_addIntFrame(obj->animpos, duration, 1, (Int)y);
    obj->animpostotaltime = AnimControl_linkToAnim(obj->animposcontrol, obj->animpos);
    obj->animpostime = 0;
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_setAlt(Gl2DObject obj, Gl2DSize z)
{
    obj->z = z;
    obj->zmoved = TRUE;
}

/*----------------------------------------------------------------------------*/
GlSurface
Gl2DObject_getDrawingSurface(Gl2DObject obj)
{
    return obj->surf;
}

/*----------------------------------------------------------------------------*/
Bool
Gl2DObject_isVisible(Gl2DObject obj)
{
    return ((obj->state != STATE2D_HIDDEN) & (obj->state != STATE2D_HIDE) & (obj->state != STATE2D_RELINKHIDE));
}

/*----------------------------------------------------------------------------*/
void
Gl2DObject_getRect(Gl2DObject obj, GlRect* rect)
{
    rect->x = obj->x;
    rect->y = obj->y;
    rect->w = GlSurface_getWidth(obj->surf);
    rect->h = GlSurface_getHeight(obj->surf);
}

/*----------------------------------------------------------------------------*/
Gl2DCoord
Gl2DObject_getX(Gl2DObject obj)
{
    return obj->x;
}

/*----------------------------------------------------------------------------*/
Gl2DCoord
Gl2DObject_getY(Gl2DObject obj)
{
    return obj->y;
}

/*----------------------------------------------------------------------------*/
Gl2DCoord
Gl2DObject_getAlt(Gl2DObject obj)
{
    return obj->z;
}

/*----------------------------------------------------------------------------*/
Gl2DSize
Gl2DObject_getWidth(Gl2DObject obj)
{
    return GlSurface_getWidth(obj->surf);
}

/*----------------------------------------------------------------------------*/
Gl2DSize
Gl2DObject_getHeight(Gl2DObject obj)
{
    return GlSurface_getHeight(obj->surf);
}
