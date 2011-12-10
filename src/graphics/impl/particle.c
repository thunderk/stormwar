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
 *   Graphical particles                                                      *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/particle.h"

#include "graphics/impl/gltextures.h"
#include "graphics/impl/opengl.h"
#include "graphics/impl/impl.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_Particle
{
    Bool visible;
    GlStaticTexture tex;
    Gl3DCoord x;
    Gl3DCoord y;
    Gl3DCoord z;
    Gl3DCoord w;
    Gl3DCoord h;
};

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
Bool
Particle_processEvent(Particle obj, GlEvent* event)
{
    if (event->type == GLEVENT_DRAW)
    {
        if (obj->visible)
        {
            GlStaticTexture_use(obj->tex);
            cameraPushObject(obj->x, obj->y, obj->z, global_camangh, global_camangv - M_PI_2);
            glBegin(GL_TRIANGLE_STRIP);
                glTexCoord2d(0, 1); glVertex3f(-obj->w, 0.0f, obj->h);
                glTexCoord2d(1, 1); glVertex3f(obj->w, 0.0f, obj->h);
                glTexCoord2d(0, 0); glVertex3f(-obj->w, 0.0f, -obj->h);
                glTexCoord2d(1, 0); glVertex3f(obj->w, 0.0f, -obj->h);
            glEnd();
            cameraPopObject();
        }
    }
    else if (event->type == GLEVENT_DELETE)
    {
        FREE(obj);
    }
    else if (event->type == GLEVENT_CAMERA)
    {
        /*TODO*/
    }
    return FALSE;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Particle
Particle_new()
{
    Particle ret;
    
    ret = (Particle)MALLOC(sizeof(pv_Particle));
    ret->visible = TRUE;
    ret->tex = GlStaticTexture_NULL;
    ret->x = 0.0f;
    ret->y = 0.0f;
    ret->z = 0.0f;
    ret->w = 0.0f;
    ret->h = 0.0f;
    
    graphicsAddParticle(ret);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Particle_del(Particle particle)
{
    graphicsDelParticle(particle);
    FREE(particle);
}

/*----------------------------------------------------------------------------*/
void
Particle_show(Particle particle, Bool show)
{
    particle->visible = show;
}

/*----------------------------------------------------------------------------*/
void
Particle_setTex(Particle particle, String texname)
{
    particle->tex = gltexturesGet(texname);
}

/*----------------------------------------------------------------------------*/
void
Particle_setSize(Particle particle, Gl3DCoord sizex, Gl3DCoord sizey)
{
    particle->w = sizex / 2.0;
    particle->h = sizey / 2.0;
}

/*----------------------------------------------------------------------------*/
void
Particle_setPos(Particle particle, Gl3DCoord x, Gl3DCoord y, Gl3DCoord z)
{
    particle->x = x;
    particle->y = y;
    particle->z = z;
}
