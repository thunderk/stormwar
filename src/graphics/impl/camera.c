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
 *   Viewing camera                                                           *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/camera.h"

#include "graphics/impl/opengl.h"
#include "graphics/impl/impl.h"

#include "core/core.h"
#include "tools/fonct.h"
#include <math.h>

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    GLdouble x;
    GLdouble y;
    GLdouble z;
} CameraPos;

typedef struct
{
    CameraPos eye;
    CameraPos look;
    CameraPos eyebg;
    CameraPos lookbg;
    CameraPos up;
    float dist;
    float angh;
    float angv;
} Camera;

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Camera camcurrent, camnext;
static Gl3DCoord xmin, xmax, ymin, ymax, zmin, zmax;
static Gl3DCoord worldwidth, worldheight, worldfactor;
static GlEvent event;
static Bool eventneeded;

static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID RES_WORLD_WIDTH = CORE_INVALID_ID;
static CoreID RES_WORLD_HEIGHT = CORE_INVALID_ID;

/******************************************************************************
 *                              Global variables                              *
 ******************************************************************************/
Gl3DCoord global_camangh;
Gl3DCoord global_camangv;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
/*Calculate camera position. Look position, dist and angles must be set before.*/
static void
calcEye()
{
    camnext.eye.x = camnext.look.x + camnext.dist * cos(camnext.angh) * cos(camnext.angv);
    camnext.eye.y = camnext.look.y + camnext.dist * sin(camnext.angv);
    camnext.eye.z = camnext.look.z + camnext.dist * sin(camnext.angh) * cos(camnext.angv);
    camnext.eyebg.x = (camnext.eye.x - worldwidth / 2) / worldfactor;
    camnext.eyebg.y = camnext.eye.y / worldfactor;
    camnext.eyebg.z = (camnext.eye.z - worldwidth / 2) / worldfactor;
    eventneeded = TRUE;
}

/*----------------------------------------------------------------------------*/
/*Calculate up vector. Angles might be set before.*/
static void
calcUp()
{
    float ang;

    ang = camnext.angv + M_PI_2;
    
    camnext.up.x = cos(camnext.angh) * cos(ang);
    camnext.up.y = sin(ang);
    camnext.up.z = sin(camnext.angh) * cos(ang);
}

/*----------------------------------------------------------------------------*/
/*Apply limits on looked-at position*/
static void
calcLook()
{
    camnext.look.x = MAX(camnext.look.x, xmin);
    camnext.look.x = MIN(camnext.look.x, xmax);
    camnext.look.y = MAX(camnext.look.y, ymin);
    camnext.look.y = MIN(camnext.look.y, ymax);
    camnext.look.z = MAX(camnext.look.z, zmin);
    camnext.look.z = MIN(camnext.look.z, zmax);
    camnext.lookbg.x = (camnext.look.x - worldwidth / 2) / worldfactor;
    camnext.lookbg.y = camnext.look.y / worldfactor;
    camnext.lookbg.z = (camnext.look.z - worldwidth / 2) / worldfactor;
    eventneeded = TRUE;
}

/*----------------------------------------------------------------------------*/
static void
resCallback(CoreID id, Var value)
{
    if (id == RES_WORLD_WIDTH)
    {
        if (Var_getType(value) == VAR_INT)
        {
            xmin = 0.0;
            xmax = (Gl3DCoord)Var_getValueInt(value);
        }
    }
    else if (id == RES_WORLD_HEIGHT)
    {
        if (Var_getType(value) == VAR_INT)
        {
            zmin = 0.0;
            zmax = (Gl3DCoord)Var_getValueInt(value);
        }
    }
    camnext.look.x = (xmax + xmin) / 2.0;
    camnext.look.y = (ymax + ymin) / 2.0;
    camnext.look.z = (zmax + zmin) / 2.0;
    camnext.dist = 20.0;
    camnext.angh = M_PI_2;
    camnext.angv = 1.1;
    calcLook();
    calcEye();
    calcUp();
}

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
void
cameraInit()
{
    camnext.look.x = 0.0;
    camnext.look.y = 0.0;
    camnext.look.z = 0.0;
    camnext.dist = 20.0;
    camnext.angh = 0.0;
    camnext.angv = 1.2;
    global_camangh = camnext.angh;
    global_camangv = camnext.angv;
    xmin = -100.0;
    xmax = 100.0;
    ymin = -10.0;
    ymax = 50.0;
    zmin = -100.0;
    zmax = 100.0;
    worldwidth = 100.0f;
    worldheight = 100.0f;
    worldfactor = 100.0f;
    calcEye();
    calcUp();
    camcurrent = camnext;
    event.type = GLEVENT_CAMERA;
    event.event.camevent.newcam.posx = camnext.eye.x;
    event.event.camevent.newcam.posy = camnext.eye.y;
    event.event.camevent.newcam.posz = camnext.eye.z;
    event.event.camevent.newcam.lookx = camnext.look.x;
    event.event.camevent.newcam.looky = camnext.look.y;
    event.event.camevent.newcam.lookz = camnext.look.z;
    
    MOD_ID = coreDeclareModule("camera", NULL, NULL, NULL, NULL, resCallback, NULL);
    RES_WORLD_WIDTH = coreAddResourceWatcher(MOD_ID, "world_width");
    RES_WORLD_HEIGHT = coreAddResourceWatcher(MOD_ID, "world_height");
    
    shellPrint(LEVEL_INFO, "Camera module loaded.");
}

/*----------------------------------------------------------------------------*/
void
cameraUninit()
{
    shellPrint(LEVEL_INFO, "Camera module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
cameraCollectEvents()
{
    if (eventneeded)
    {
        camcurrent = camnext;
        global_camangh = camnext.angh;
        global_camangv = camnext.angv;
        eventneeded = FALSE;
        event.event.camevent.oldcam = event.event.camevent.newcam;
        event.event.camevent.newcam.posx = camnext.eye.x;
        event.event.camevent.newcam.posy = camnext.eye.y;
        event.event.camevent.newcam.posz = camnext.eye.z;
        event.event.camevent.newcam.lookx = camnext.look.x;
        event.event.camevent.newcam.looky = camnext.look.y;
        event.event.camevent.newcam.lookz = camnext.look.z;
        graphicsProcessEvent(&event);
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
cameraSetSceneNormal()
{
    /*projection*/
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    /*glFrustum(-0.1 * camcurrent.dist, 0.1 * camcurrent.dist, -0.1 * camcurrent.dist * global_screenratio, 0.1 * camcurrent.dist * global_screenratio, 0.2 * camcurrent.dist, 20.0 * camcurrent.dist);*/
    glFrustum(-0.05, 0.05, -0.05 * global_screenratio, 0.05 * global_screenratio, 0.1, 1000.0);
    
    /*viewpoint*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camcurrent.eye.x, camcurrent.eye.y, camcurrent.eye.z, camcurrent.look.x, camcurrent.look.y, camcurrent.look.z, camcurrent.up.x, camcurrent.up.y, camcurrent.up.z);
}

/*----------------------------------------------------------------------------*/
void
cameraSetSceneBackground()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-0.05, 0.05, -0.05 * global_screenratio, 0.05 * global_screenratio, 0.07, 50.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(camcurrent.eyebg.x, camcurrent.eyebg.y, camcurrent.eyebg.z, camcurrent.lookbg.x, camcurrent.lookbg.y, camcurrent.lookbg.z, camcurrent.up.x, camcurrent.up.y, camcurrent.up.z);
}

/*----------------------------------------------------------------------------*/
void
cameraPushObject(Gl3DCoord x, Gl3DCoord y, Gl3DCoord z, Gl3DCoord angh, Gl3DCoord angv)
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    
    /*object transformations*/
    glTranslatef(x, y, z);
    glRotatef(-RAD2DEG(angh), 0.0, 1.0, 0.0);
    glRotatef(-RAD2DEG(angv), 0.0, 0.0, -1.0);
}

/*----------------------------------------------------------------------------*/
void
cameraPopObject(void)
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

/*----------------------------------------------------------------------------*/
void
cameraSetPos(Gl3DCoord x, Gl3DCoord y, Gl3DCoord z)
{
    camnext.look.x = x;
    camnext.look.y = y;
    camnext.look.z = z;
    calcLook();
    calcEye();
}

/*----------------------------------------------------------------------------*/
void
cameraScroll(Gl3DCoord right, Gl3DCoord front)
{
    Gl3DCoord f;
    
    f = camnext.dist / 16.0;
    f = MIN(f, 5.0);
    f = MAX(f, 0.4);
    camnext.look.x -= right * f * sin(camnext.angh) + front * f * cos(camnext.angh);
    camnext.look.z += right * f * cos(camnext.angh) - front * f * sin(camnext.angh);
    calcLook();
    calcEye();
}

/*----------------------------------------------------------------------------*/
void
cameraSetAngle(Gl3DCoord _angh, Gl3DCoord _angv)
{
    camnext.angh = modulo(_angh, 0.0, M_2PI);
    camnext.angv = MAX(_angv, M_PI_2 * 0.1);
    camnext.angv = MIN(camnext.angv, M_PI_2);
    calcLook();
    calcEye();
    calcUp();
}

/*----------------------------------------------------------------------------*/
void
cameraPan(Gl3DCoord ang)
{
    camnext.angh = modulo(camnext.angh - ang, 0.0, M_2PI);
    camnext.look.x = camnext.eye.x - camnext.dist * cos(camnext.angh) * cos(camnext.angv);
    camnext.look.y = camnext.eye.y - camnext.dist * sin(camnext.angv);
    camnext.look.z = camnext.eye.z - camnext.dist * sin(camnext.angh) * cos(camnext.angv);
    calcLook();
    calcEye();     /*the real position doesn't change but special ones yes*/
    calcUp();
}

/*----------------------------------------------------------------------------*/
void
cameraRotateAround(Gl3DCoord angh, Gl3DCoord angv)
{
    camnext.angh = modulo(camnext.angh + angh, 0.0, M_2PI);
    camnext.angv = camnext.angv + angv;
    camnext.angv = MAX(camnext.angv, M_PI_2 * 0.1);
    camnext.angv = MIN(camnext.angv, M_PI_2);
    calcEye();
    calcUp();
}

/*----------------------------------------------------------------------------*/
void
cameraSetZoom(Gl3DCoord zoom)
{
    ASSERT(zoom >= 5.0, return)
    ASSERT(zoom <= 250.0, return)
    
    camnext.dist = zoom;
    calcEye();
}

/*----------------------------------------------------------------------------*/
void
cameraZoom(Gl3DCoord dzoom)
{
    camnext.dist += dzoom * camnext.dist / 2.0;
    camnext.dist = MAX(camnext.dist, 5.0);
    camnext.dist = MIN(camnext.dist, 250.0);
    calcEye();
}

/*----------------------------------------------------------------------------*/
Gl3DCoord
cameraGetDist()
{
    return camcurrent.dist;
}

/*----------------------------------------------------------------------------*/
void
cameraGetRealPos(Gl3DCoord* x, Gl3DCoord* y, Gl3DCoord* z)
{
    *x = camcurrent.eye.x;
    *y = camcurrent.eye.y;
    *z = camcurrent.eye.z;
}
