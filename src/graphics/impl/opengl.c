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
 *   OpenGL management                                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/impl/opengl.h"

#include "SDL_opengl.h"

#include "graphics/graphics.h"
#include "graphics/gl2dobject.h"
#include "graphics/camera.h"
#include "graphics/color.h"

#include "core/core.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                              Global variables                              *
 ******************************************************************************/
float global_screenratio;
Bool global_forceblend;  /*force blending*/

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Uint16 screen_w;
static Uint16 screen_h;

static GLint shading;
static Bool lighting;
static Bool wireframe2d;
static Bool wireframe3d;
static Bool texture2d;
static Bool texture3d;

static Bool fog;
static float fogdensity;
static float fogcolor[4];

static CoreID MOD_ID;
static CoreID FUNC_WIREFRAME;
static CoreID FUNC_FOG;
static CoreID FUNC_LIGHTING;
static CoreID FUNC_TEXTURE;
static CoreID FUNC_COUNT;

static Uint32 curcount;
static Uint32 count;

static Bool vp_wrel;
static Bool vp_hrel;
static Sint16 vp_left;
static Sint16 vp_top;
static Sint16 vp_width;
static Sint16 vp_height;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNC_WIREFRAME)
    {
        wireframe2d = (Var_getValueInt(func->params[0]) == 0) ? FALSE : TRUE;
        wireframe3d = (Var_getValueInt(func->params[1]) == 0) ? FALSE : TRUE;
        Var_setVoid(func->ret);
    }
    else if (func->id == FUNC_FOG)
    {
        fog = (Var_getValueInt(func->params[0]) == 0) ? FALSE : TRUE;
        Var_setVoid(func->ret);
    }
    else if (func->id == FUNC_LIGHTING)
    {
        lighting = (Var_getValueInt(func->params[0]) == 0) ? FALSE : TRUE;
        shading = (Var_getValueInt(func->params[1]) == 0) ? GL_SMOOTH : GL_FLAT;
        Var_setVoid(func->ret);
    }
    else if (func->id == FUNC_TEXTURE)
    {
        texture2d = (Var_getValueInt(func->params[0]) == 0) ? FALSE : TRUE;
        texture3d = (Var_getValueInt(func->params[1]) == 0) ? FALSE : TRUE;
        Var_setVoid(func->ret);
    }
    else if (func->id == FUNC_COUNT)
    {
        Var_setInt(func->ret, count);
    }
}

/*----------------------------------------------------------------------------*/
static void
openglSetFog(Var vfog)
{
    VarValidator valid;
    GlColorRGBA col;

    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "fog", 0);
    VarValidator_declareFloatVar(valid, "density", 0.01);
    VarValidator_declareArrayVar(valid, "color");
    VarValidator_validate(valid, vfog);
    VarValidator_del(valid);

    fog = (Var_getValueInt(Var_getArrayElemByCName(vfog, "fog"))) ? TRUE : FALSE;
    fogdensity = Var_getValueFloat(Var_getArrayElemByCName(vfog, "density"));
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(vfog, "color"));
    fogcolor[0] = (float)col.r / 255.0;
    fogcolor[1] = (float)col.g / 255.0;
    fogcolor[2] = (float)col.b / 255.0;
    fogcolor[3] = (float)col.a / 255.0;
}

/******************************************************************************
 *############################################################################*
 *#                             Screen functions                             #*
 *############################################################################*
 ******************************************************************************/
void
openglInit()
{
    /*setting up OpenGL*/
    screen_w = 10;
    screen_h = 10;
    global_screenratio = 1.0;
    glViewport(0, 0, 10, 10);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    vp_wrel = vp_hrel = FALSE;
    vp_top = vp_left = 0;
    vp_width = vp_height = 10;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    wireframe2d = FALSE;
    wireframe3d = FALSE;
    shading = GL_SMOOTH;
    lighting = TRUE;
    
    texture2d = TRUE;
    texture3d = TRUE;

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    global_forceblend = FALSE;
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_FOG_HINT, GL_FASTEST);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, TRUE);

    curcount = 0;
    count = 0;

    fog = FALSE;
    fogdensity = 0.0;
    fogcolor[0] = 0.5;
    fogcolor[1] = 0.5;
    fogcolor[2] = 0.5;
    fogcolor[3] = 1.0;

    /*core declaration*/
    MOD_ID = coreDeclareModule("opengl", NULL, NULL, shellCallback, NULL, NULL, NULL);
    FUNC_WIREFRAME = coreDeclareShellFunction(MOD_ID, "wireframe", VAR_VOID, 2, VAR_INT, VAR_INT);
    FUNC_FOG = coreDeclareShellFunction(MOD_ID, "fog", VAR_VOID, 1, VAR_INT);
    FUNC_LIGHTING = coreDeclareShellFunction(MOD_ID, "lighting", VAR_VOID, 2, VAR_INT, VAR_INT);
    FUNC_TEXTURE = coreDeclareShellFunction(MOD_ID, "texture", VAR_VOID, 2, VAR_INT, VAR_INT);
    FUNC_COUNT = coreDeclareShellFunction(MOD_ID, "count", VAR_INT, 0);

    /*OpenGL info*/
    shellPrintf(LEVEL_INFO, " -> OpenGL version:  %s", (const char*)glGetString(GL_VERSION));
    shellPrintf(LEVEL_INFO, " -> OpenGL vendor:   %s", (const char*)glGetString(GL_VENDOR));
    shellPrintf(LEVEL_INFO, " -> OpenGL renderer: %s", (const char*)glGetString(GL_RENDERER));
    shellPrintf(LEVEL_DEBUG, " -> OpenGL extensions: %s", (const char*)glGetString(GL_EXTENSIONS));
    /*GLU info*/
    shellPrintf(LEVEL_INFO, " -> GLU version:     %s", (const char*)gluGetString(GLU_VERSION));
    shellPrintf(LEVEL_DEBUG, " -> GLU extensions: %s", (const char*)gluGetString(GLU_EXTENSIONS));
}

/*----------------------------------------------------------------------------*/
void
openglUninit()
{
    shellPrint(LEVEL_INFO, "OpenGL module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
openglSet(Var v)
{
    VarValidator valid;
    GlColorRGBA col;

    valid = VarValidator_new();
    VarValidator_declareArrayVar(valid, "fog");
    VarValidator_declareArrayVar(valid, "clearcolor");
    VarValidator_declareIntVar(valid, "view_x", 0);
    VarValidator_declareIntVar(valid, "view_y", 0);
    VarValidator_declareIntVar(valid, "view_wrel", 1);
    VarValidator_declareIntVar(valid, "view_hrel", 1);
    VarValidator_declareIntVar(valid, "view_w", 0);
    VarValidator_declareIntVar(valid, "view_h", 0);
    VarValidator_validate(valid, v);
    VarValidator_del(valid);

    openglSetFog(Var_getArrayElemByCName(v, "fog"));
    GlColorRGBA_makeFromVar(&col, Var_getArrayElemByCName(v, "clearcolor"));
    glClearColor((float)col.r / 255.0, (float)col.g / 255.0, (float)col.b / 255.0, (float)col.a / 255.0);
    vp_left = Var_getValueInt(Var_getArrayElemByCName(v, "view_x"));
    vp_top = Var_getValueInt(Var_getArrayElemByCName(v, "view_y"));
    vp_wrel = Var_getValueInt(Var_getArrayElemByCName(v, "view_wrel"));
    vp_hrel = Var_getValueInt(Var_getArrayElemByCName(v, "view_hrel"));
    vp_width = Var_getValueInt(Var_getArrayElemByCName(v, "view_w"));
    vp_height = Var_getValueInt(Var_getArrayElemByCName(v, "view_h"));
}

/*----------------------------------------------------------------------------*/
void
openglScreenSize(Uint16 width, Uint16 height)
{
    screen_w = width;
    screen_h = height;
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    
    glDepthFunc(GL_LEQUAL);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glHint(GL_FOG_HINT, GL_FASTEST);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, TRUE);
}

/*----------------------------------------------------------------------------*/
void
openglStep3DBackground()
{
    Sint16 w, h;

    w = (vp_wrel) ? screen_w + vp_width : vp_width;
    h = (vp_hrel) ? screen_h + vp_height : vp_height;
    glViewport(vp_left, screen_h - h - vp_top, w, h);
    global_screenratio = (float)h / (float)w;

    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if (texture3d)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    if (wireframe3d)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glClear(GL_COLOR_BUFFER_BIT);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

/*----------------------------------------------------------------------------*/
void
openglStep3DObjects()
{
    glDepthMask(GL_TRUE);           /*bacause background can set it to false, what cancels the clear effect*/
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (lighting)
    {
        glEnable(GL_LIGHTING);
        glShadeModel(shading);
    }

    if (fog)
    {
        glEnable(GL_FOG);
        glFogi(GL_FOG_MODE, GL_EXP2);
        glFogf(GL_FOG_DENSITY, fogdensity);
        glFogfv(GL_FOG_COLOR, fogcolor);
    }
}

/*----------------------------------------------------------------------------*/
void
openglStep3DObjectsGhost()
{
    global_forceblend = TRUE;
    glDepthMask(GL_FALSE);
    glDisable(GL_LIGHTING);
    glDisable(GL_FOG);
    glDisable(GL_DEPTH_TEST);
}

/*----------------------------------------------------------------------------*/
void
openglStepParticles(void)
{
    global_forceblend = FALSE;
    glEnable(GL_BLEND);
    glEnable(GL_FOG);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

/*----------------------------------------------------------------------------*/
void
openglStep2D()
{
    glViewport(0, 0, screen_w, screen_h);
    global_screenratio = (float)screen_w / (float)screen_h;

    glDepthMask(GL_TRUE);
	glDisable(GL_LIGHTING);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glDisable(GL_FOG);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_w, screen_h, 0, -GL2D_HIGHEST, GL2D_LOWEST);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    if (texture2d)
    {
        glEnable(GL_TEXTURE_2D);
    }
    else
    {
        glDisable(GL_TEXTURE_2D);
    }

    if (wireframe2d)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

#ifdef DEBUG_OPENGL
/*----------------------------------------------------------------------------*/
void
pv_openglResetCount()
{
    count = curcount;
    curcount = 0;
}

/*----------------------------------------------------------------------------*/
void
pv_openglCount(unsigned int i)
{
    curcount += i;
}
#endif
