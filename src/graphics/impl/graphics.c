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
 *   Engine that handles the graphical part of the game                       *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/graphics.h"

#include "graphics/gl2dobject.h"
#include "graphics/gl3dobject.h"
#include "graphics/glrect.h"
#include "graphics/glsurface.h"
#include "graphics/impl/impl.h"
#include "graphics/impl/cursor.h"
#include "graphics/impl/keyboard.h"
#include "graphics/impl/glscreen.h"
#include "graphics/impl/gltextures.h"
#include "core/core.h"
#include "core/shell.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                                   Typedefs                                 *
 ******************************************************************************/
struct pv_GlEventCollector
{
    GlEventType events;
    GlEventCallback callback;
};

struct pv_Gl3DGroup
{
    Gl3DRenderMode mode;
    PtrArray array;
};

/******************************************************************************
 *                              Internal variables                            *
 ******************************************************************************/
Uint32 global_frametime;
Bool global_pause;
Bool global_cammoved;
Uint32 global_checkcount;

/******************************************************************************
 *                               Static variables                             *
 ******************************************************************************/
static PtrArray array2d;
static PtrArray array2d_add;
static PtrArray array2d_del;

static Uint16 nbgroups3d;
static Gl3DGroup* groups3d;
static PtrArray array3d_add;
static PtrArray array3d_del;

static PtrArray particles;
static PtrArray particles_add;
static PtrArray particles_del;

static PtrArray eventcollectors;
static PtrArray eventcollectors_add;
static PtrArray eventcollectors_del;

static Uint32 needsorting;
static Gl2DObject needsorting_obj;

static Gl2DObject obj_holdmouse;
static Gl2DObject obj_focus;

/* Static events */
static GlEvent event_delete;
static GlEvent event_screenstate;
static GlEvent event_camerastate;
static GlEvent event_selection;

static Uint32 fpsmin = 50000;
static Uint32 fpsmax = 0;
static Uint32 fpscurrent = 0;
static Uint32 fpsbase;
static Uint32 fpscount = 0;

/* Shell module functions */
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID FUNC_FPS = CORE_INVALID_ID;
static CoreID FUNC_FPSSTAT = CORE_INVALID_ID;
static CoreID FUNC_FPSCLEAR = CORE_INVALID_ID;
static CoreID FUNC_SETFPSMAX = CORE_INVALID_ID;
static CoreID FUNC_SETGAMMA = CORE_INVALID_ID;

static Var _prefsvar = NULL;

/******************************************************************************
 *                                  Globals                                   *
 ******************************************************************************/
Gl3DGroup global_groupbackground;
Gl3DGroup global_groupnormal;
Gl3DGroup global_grouptransparent;
Gl3DGroup global_groupghost;

/******************************************************************************
 *############################################################################*
 *#                             Internal functions                           #*
 *############################################################################*
 ******************************************************************************/
void
graphicsAdd2DObject(Gl2DObject obj)
{
    PtrArray_append(array2d_add, obj);
}

/*----------------------------------------------------------------------------*/
void
graphicsDel2DObject(Gl2DObject obj)
{
    PtrArray_append(array2d_del, obj);

    if (obj == obj_focus)
    {
        obj_focus = NULL;
    }
    if (obj == obj_holdmouse)
    {
        obj_holdmouse = NULL;
    }
}

/*----------------------------------------------------------------------------*/
void
graphicsAdd3DObject(Gl3DObject obj)
{
    PtrArray_append(array3d_add, obj);
}

/*----------------------------------------------------------------------------*/
void
graphicsDel3DObject(Gl3DObject obj)
{
    PtrArray_append(array3d_del, obj);
}

/*----------------------------------------------------------------------------*/
void
graphicsAddParticle(Particle obj)
{
    PtrArray_append(particles_add, obj);
}

/*----------------------------------------------------------------------------*/
void
graphicsDelParticle(Particle obj)
{
    PtrArray_append(particles_del, obj);
}

/*----------------------------------------------------------------------------*/
Bool
graphicsIsGroupZSorted(Gl3DGroup group)
{
    return (group->mode > GL3DRENDER_NORMAL);
}

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static void
GlEventCollector_del(GlEventCollector ec)
{
    FREE(ec);
}

/*----------------------------------------------------------------------------*/
static void
setFpsMax(int fps)
{
    CoreID mainthread = coreGetThreadID(NULL);

    if (fps == 0)
    {
        coreSetThreadTimer(MOD_ID, mainthread, 0);
    }
    else
    {
        if (fps < 10)
        {
            fps = 10;
        }
        coreSetThreadTimer(MOD_ID, mainthread, 1000 / fps);
    }

    if (_prefsvar != NULL)
    {
        Var v;

        v = Var_getArrayElemByCName(_prefsvar, "fpsmax");
        ASSERT(v != NULL, return);
        Var_setInt(v, fps);
    }
}

/*----------------------------------------------------------------------------*/
static void
collectSelectionEvent(void)
{
    Sint16 cursorx, cursory;
    GLdouble px, py, pz, f;
    Gl3DCoord cx, cy, cz;
    GLdouble modelmatrix[16];
    GLdouble projmatrix[16];
    GLint viewport[4];
    Sint16 nx, ny;

    /*TODO: redo this only if the camera changed*/

    glGetDoublev(GL_MODELVIEW_MATRIX, modelmatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
    glGetIntegerv(GL_VIEWPORT, viewport);

    /*find cursor's 3d coordinates*/
    cursorGetPos(&cursorx, &cursory);
    gluUnProject((GLdouble)cursorx, (GLdouble)global_screenheight - (GLdouble)cursory, 0.0, modelmatrix, projmatrix, viewport, &px, &py, &pz);

    /*retrieve camera position*/
    cameraGetRealPos(&cx, &cy, &cz);

    /*calculate intersection between the line of vision and the ground plane*/
    if (cy - py > 0.001)
    {
        f = cy / (cy - py);
        /*TODO: maybe clamp the value to Sint16 boundaries*/
        nx = floor(cx + (px - cx) * f);
        ny = floor(cz + (pz - cz) * f);
        if ((event_selection.event.selectionevent.type == SELECTIONEVENT_NONE) && ((nx != event_selection.event.selectionevent.groundx) | (ny != event_selection.event.selectionevent.groundy)))
        {
            event_selection.event.selectionevent.type = SELECTIONEVENT_GROUNDMOVE;
        }

        event_selection.event.selectionevent.groundx = nx;
        event_selection.event.selectionevent.groundy = ny;
    }

    if (event_selection.event.selectionevent.type != SELECTIONEVENT_NONE)
    {
        graphicsProcessEvent(&event_selection);
        event_selection.event.selectionevent.type = SELECTIONEVENT_NONE;
    }
}

/*----------------------------------------------------------------------------*/
static Bool
loadingPictCallback(GlExtID data, GlEvent* event)
{
    (void)event;
    (void)data;
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
showLoadingPicture(String file)
{
    GlSurface surf;
    Gl2DObject obj;
    GlEvent drawevent;
    
    glscreenUpdate();
    surf = GlSurface_newFromFile(file);
    GlSurface_resize(surf, global_screenwidth, global_screenheight, SURFACE_RESAMPLE);
    obj = Gl2DObject_new(surf, NULL, loadingPictCallback);
    Gl2DObject_setPos(obj, 0, 0, 0);
    Gl2DObject_setAlt(obj, GL2D_HIGHEST);
    
    drawevent.type = GLEVENT_DRAW;
    openglStep2D();
    glClear(GL_COLOR_BUFFER_BIT);
    Gl2DObject_processEvent(obj, &drawevent);
    glscreenUpdate();
    
    Gl2DObject_del(obj);
    GlSurface_del(surf);
}

/*----------------------------------------------------------------------------*/
static void
prefsCallback(Var prefs)
{
    VarValidator valid;

    _prefsvar = prefs;

    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "screen_width", 800);
    VarValidator_declareIntVar(valid, "screen_height", 600);
    VarValidator_declareIntVar(valid, "screen_full", 1);
    VarValidator_declareIntVar(valid, "fpsmax", 50);

    VarValidator_validate(valid, prefs);
    VarValidator_del(valid);

    graphicsSetVideoMode(Var_getValueInt(Var_getArrayElemByCName(prefs, "screen_width")),
                         Var_getValueInt(Var_getArrayElemByCName(prefs, "screen_height")),
                         Var_getValueInt(Var_getArrayElemByCName(prefs, "screen_full")));
    setFpsMax(Var_getValueInt(Var_getArrayElemByCName(prefs, "fpsmax")));
}

/*----------------------------------------------------------------------------*/
static void
coreCallback(CoreEvent event)
{
    if (event == CORE_PAUSE)
    {
        global_pause = TRUE;
    }
    if (event == CORE_RESUME)
    {
        global_pause = FALSE;
    }
    if (event == CORE_DATALOADED)
    {
        /*produce a resize event to put all GUI objects in place*/
        graphicsProcessEvent(&event_screenstate);
    }
}

/*----------------------------------------------------------------------------*/
static void
datasCallback(Var datas)
{
    VarValidator valid;
    float f;

    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "loading_pict", "");
    VarValidator_declareFloatVar(valid, "gamma", 1.0);
    VarValidator_declareArrayVar(valid, "opengl");
    VarValidator_declareArrayVar(valid, "cursors");
    VarValidator_declareArrayVar(valid, "fonts");
    VarValidator_declareArrayVar(valid, "textures");
    VarValidator_validate(valid, datas);
    VarValidator_del(valid);

    showLoadingPicture(Var_getValueString(Var_getArrayElemByCName(datas, "loading_pict")));

    openglSet(Var_getArrayElemByCName(datas, "opengl"));
    f = Var_getValueFloat(Var_getArrayElemByCName(datas, "gamma"));
    cursorSetCursors(Var_getArrayElemByCName(datas, "cursors"));
    SDL_SetGamma(f, f, f);

    gltextClearFonts();
    gltextSetFonts(Var_getArrayElemByCName(datas, "fonts"));
    gltexturesLoadSet(Var_getArrayElemByCName(datas, "textures"));
}

/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNC_FPS)
    {
        Var_setInt(func->ret, fpscurrent);
    }
    else if (func->id == FUNC_FPSSTAT)
    {
        shellPrintf(LEVEL_USER, _("Minimum FPS: %d"), fpsmin);
        shellPrintf(LEVEL_USER, _("Maximum FPS: %d"), fpsmax);
    }
    else if (func->id == FUNC_SETFPSMAX)
    {
        setFpsMax(Var_getValueInt(func->params[0]));
    }
    else if (func->id == FUNC_FPSCLEAR)
    {
        fpsmin = 50000;
        fpsmax = 0;
    }
    else if (func->id == FUNC_SETGAMMA)
    {
        SDL_SetGamma(Var_getValueFloat(func->params[0]), Var_getValueFloat(func->params[0]), Var_getValueFloat(func->params[0]));
    }
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime duration)
{
    CoreTime curtime;
    GlEvent event;
    PtrArrayIterator it;
    Uint16 group;

    (void)thread;

    global_frametime = duration;

    /*calculate fps*/
    curtime = getTicks();
    if (curtime - fpsbase > 1000)
    {
        fpsbase = curtime;
        fpsmax = MAX(fpscount, fpsmax);
        fpsmin = MIN(fpscount, fpsmin);
        fpscurrent = fpscount;
        fflush(stdout);
        fpscount = 0;
    }

    /*we collect pending events*/
    inputCollectEvents();
    global_cammoved = FALSE;
    cameraCollectEvents();

    /*we perform adds and dels (with state and delete events)*/
    for (it = PtrArray_START(array2d_add); it != PtrArray_STOP(array2d_add); it++)
    {
        PtrArray_append(array2d, (Gl2DObject)(*it));
        Gl2DObject_processEvent((Gl2DObject)(*it), &event_screenstate);
    }
    PtrArray_clear(array2d_add);
    for (it = PtrArray_START(array2d_del); it != PtrArray_STOP(array2d_del); it++)
    {
        PtrArray_remove(array2d, (Gl2DObject)(*it));
        Gl2DObject_processEvent((Gl2DObject)(*it), &event_delete);
    }
    PtrArray_clear(array2d_del);
    for (it = PtrArray_START(array3d_add); it != PtrArray_STOP(array3d_add); it++)
    {
        PtrArray_append(Gl3DObject_getGroup((Gl3DObject)(*it))->array, (Gl3DObject)(*it));
    }
    PtrArray_clear(array3d_add);
    for (it = PtrArray_START(array3d_del); it != PtrArray_STOP(array3d_del); it++)
    {
        PtrArray_remove(Gl3DObject_getGroup((Gl3DObject)(*it))->array, (Gl3DObject)(*it));
        Gl3DObject_processEvent((Gl3DObject)(*it), &event_delete);
    }
    PtrArray_clear(array3d_del);
    for (it = PtrArray_START(particles_add); it != PtrArray_STOP(particles_add); it++)
    {
        PtrArray_append(particles, (Particle)(*it));
        Particle_processEvent((Particle)(*it), &event_camerastate);
    }
    PtrArray_clear(particles_add);
    for (it = PtrArray_START(particles_del); it != PtrArray_STOP(particles_del); it++)
    {
        PtrArray_remove(particles, (Particle)(*it));
        Particle_processEvent((Particle)(*it), &event_delete);
    }
    PtrArray_clear(particles_del);
    for (it = PtrArray_START(eventcollectors_add); it != PtrArray_STOP(eventcollectors_add); it++)
    {
        if (((GlEventCollector)(*it))->events & GLEVENT_RESIZE)
        {
            ((GlEventCollector)(*it))->callback(*it, &event_screenstate);
        }
        if (((GlEventCollector)(*it))->events & GLEVENT_CAMERA)
        {
            ((GlEventCollector)(*it))->callback(*it, &event_camerastate);
        }
        PtrArray_append(eventcollectors, (GlEventCollector)(*it));
    }
    PtrArray_clear(eventcollectors_add);
    for (it = PtrArray_START(eventcollectors_del); it != PtrArray_STOP(eventcollectors_del); it++)
    {
        if (((GlEventCollector)(*it))->events & GLEVENT_DELETE)
        {
            ((GlEventCollector)(*it))->callback(*it, &event_delete);
        }
        PtrArray_removeFast(eventcollectors, (GlEventCollector)(*it));
    }
    PtrArray_clear(eventcollectors_del);

    event.type = GLEVENT_DRAW;
    event.event.frameduration = duration;
    group = 0;

    /*if (global_checkcount >= 50)
        printf("50 objects rechecked.\n");
    else
        printf("%2d objects rechecked.\n", global_checkcount);*/

    global_checkcount = 0;
    
    /*draw 3d background*/
    openglStep3DBackground();
    cameraSetSceneBackground();
    while ((group < nbgroups3d) && (groups3d[group]->mode == GL3DRENDER_BACKGROUND))
    {
        PtrArray_foreachWithData(groups3d[group]->array, (PtrFuncWithData)Gl3DObject_processEvent, &event);
        group++;
    }

    /*draw plain 3d objects*/
    openglStep3DObjects();
    cameraSetSceneNormal();
    lightSetScene();
    while ((group < nbgroups3d) && (groups3d[group]->mode == GL3DRENDER_NORMAL))
    {
        PtrArray_foreachWithData(groups3d[group]->array, (PtrFuncWithData)Gl3DObject_processEvent, &event);
        group++;
    }

    /*draw blended 3d objects*/
    while ((group < nbgroups3d) && (groups3d[group]->mode == GL3DRENDER_BLENDED))
    {
        needsorting = 0;
        for (it = PtrArray_START(groups3d[group]->array); it != PtrArray_STOP(groups3d[group]->array); it++)
        {
            if (Gl3DObject_processEvent((Gl3DObject)*it, &event))
            {
                needsorting++;
                needsorting_obj = *it;
            }
        }
        if (needsorting > 1)
        {
            /*all objects need to be sorted*/
            PtrArray_sort(groups3d[group]->array);
        }
        else if (needsorting == 1)
        {
            /*one object needs to be sorted again*/
            /*FIXME: use a PtrArray_sortOne*/
            PtrArray_sort(groups3d[group]->array);
        }
        group++;
    }

    /*we get the selection event here (because of the opengl step)*/
    collectSelectionEvent();

    /*draw ghost objects*/
    openglStep3DObjectsGhost();
    while ((group < nbgroups3d) && (groups3d[group]->mode == GL3DRENDER_GHOST))
    {
        PtrArray_foreachWithData(groups3d[group]->array, (PtrFuncWithData)Gl3DObject_processEvent, &event);
        group++;
    }
    
    /*draw particles*/
    openglStepParticles();
    PtrArray_foreachWithData(particles, (PtrFuncWithData)Particle_processEvent, &event);

    /*draw 2d objects*/
    openglStep2D();
    needsorting = 0;
    for (it = PtrArray_START(array2d); it != PtrArray_STOP(array2d); it++)
    {
        if (Gl2DObject_processEvent((Gl2DObject)*it, &event))
        {
            needsorting++;
            needsorting_obj = *it;
        }
    }
    if (needsorting > 1)
    {
        /*all objects need to be sorted*/
        PtrArray_sort(array2d);
    }
    else if (needsorting == 1)
    {
        /*one object needs to be sorted again*/
        /*FIXME: use a PtrArray_sortOne*/
        PtrArray_sort(array2d);
    }

    /*throw draw event to the collectors*/
    graphicsProcessEvent(&event);
    
    openglResetCount();

#ifdef DEBUG_OPENGL
    {
        GLenum e;
        do
        {
            e = glGetError();
            if (e != GL_NO_ERROR)
            {
                if (e == GL_OUT_OF_MEMORY)
                {
                    shellPrintf(LEVEL_DEBUG, "OpenGL: %s", gluErrorString(e));
                    error("graphics", "threadCallback", "Fatal OpenGL memory error.");
                }
                else
                {
                    shellPrintf(LEVEL_DEBUG, "OpenGL: %s", gluErrorString(e));
                }
            }
        } while (e != GL_NO_ERROR);
    }
#endif

    glscreenUpdate();
    fpscount++;
}

/******************************************************************************
 *############################################################################*
 *#                              Public functions                            #*
 *############################################################################*
 ******************************************************************************/
void
graphicsInit()
{
    array2d = PtrArray_newFull(50, 10, NULL, (PtrCmpFunc)Gl2DObject_cmp);
    array2d_del = PtrArray_new();
    array2d_add = PtrArray_new();

    nbgroups3d = 0;
    groups3d = NULL;
    array3d_del = PtrArray_new();
    array3d_add = PtrArray_new();

    particles = PtrArray_newFull(100, 50, NULL, NULL);
    particles_del = PtrArray_new();
    particles_add = PtrArray_new();

    eventcollectors = PtrArray_newFull(10, 5, (PtrFunc)GlEventCollector_del, NULL);
    eventcollectors_del = PtrArray_new();
    eventcollectors_add = PtrArray_new();

    event_delete.type = GLEVENT_DELETE;
    event_selection.type = GLEVENT_SELECTION;
    event_selection.event.selectionevent.type = SELECTIONEVENT_NONE;
    event_selection.event.selectionevent.groundx = 0;
    event_selection.event.selectionevent.groundy = 0;
    event_selection.event.selectionevent.obj = NULL;
    event_screenstate.type = GLEVENT_RESIZE;
    event_screenstate.event.resizeevent.width = 10;
    event_screenstate.event.resizeevent.height = 10;
    event_camerastate.type = GLEVENT_CAMERA;

    global_frametime = 0;
    global_pause = FALSE;

    obj_holdmouse = NULL;
    obj_focus = NULL;

    global_groupbackground = graphicsCreate3DGroup(GL3DRENDER_BACKGROUND);
    global_groupnormal = graphicsCreate3DGroup(GL3DRENDER_NORMAL);
    global_grouptransparent = graphicsCreate3DGroup(GL3DRENDER_BLENDED);
    global_groupghost = graphicsCreate3DGroup(GL3DRENDER_GHOST);

    glscreenInit();
    keyboardInit();
    colorInit();
    openglInit();
    gltexturesInit();
    gltextInit();
    cameraInit();
    lightInit();
    cursorInit();
    inputInit();

    _prefsvar = NULL;

    fpsbase = getTicks();
    fpscount = 0;
    /*graphicsSetVideoMode(800, 600, FALSE);*/

    MOD_ID = coreDeclareModule("graph", coreCallback, datasCallback, shellCallback, prefsCallback, NULL, threadCallback);
    FUNC_FPS = coreDeclareShellFunction(MOD_ID, "fps", VAR_INT, 0);
    FUNC_FPSSTAT = coreDeclareShellFunction(MOD_ID, "fpsstat", VAR_VOID, 0);
    FUNC_SETFPSMAX = coreDeclareShellFunction(MOD_ID, "setfpsmax", VAR_VOID, 1, VAR_INT);
    FUNC_FPSCLEAR = coreDeclareShellFunction(MOD_ID, "fpsclear", VAR_VOID, 0);
    FUNC_SETGAMMA = coreDeclareShellFunction(MOD_ID, "setgamma", VAR_VOID, 1, VAR_FLOAT);
    coreRequireThreadSlot(MOD_ID, coreGetThreadID(NULL));   /*require a main thread slot*/

    setFpsMax(50);
}

/*----------------------------------------------------------------------------*/
void
graphicsUninit()
{
    Uint16 i;

    inputUninit();
    cursorUninit();
    cameraUninit();
    lightUninit();
    gltextUninit();
    gltexturesUninit();
    openglUninit();
    keyboardUninit();
    glscreenUninit();

    PtrArray_del(array3d_add);
    PtrArray_del(array3d_del);
    if (nbgroups3d != 0)
    {
        for (i = 0; i < nbgroups3d; i++)
        {
            PtrArray_del(groups3d[i]->array);
            FREE(groups3d[i]);
        }
        FREE(groups3d);
    }

    PtrArray_del(array2d_add);
    PtrArray_del(array2d_del);
    PtrArray_del(array2d);

    PtrArray_del(eventcollectors_add);
    PtrArray_del(eventcollectors_del);
    PtrArray_del(eventcollectors);

    shellPrintf(LEVEL_INFO, "Graphical engine destroyed.");
}

/*----------------------------------------------------------------------------*/
void
graphicsSetVideoMode(Uint16 width, Uint16 height, Bool fullscreen)
{
    GlEvent event;

    /*warn the sub-modules concerned*/
    glscreenSetVideo(&width, &height, &fullscreen);
    openglScreenSize(width, height);
    gltexturesCheck();
    inputSetFullscreen(fullscreen);

    /*throw an event for 2d objects*/
    event.type = GLEVENT_RESIZE;
    event.event.resizeevent.width = width;
    event.event.resizeevent.height = height;
    graphicsProcessEvent(&event);

    shellPrintf(LEVEL_INFO, "Screen resized to %s %d*%d.", fullscreen ? "fullscreen" : "windowed", width, height);

    /*tell preferences about this*/
    if (_prefsvar != NULL)
    {
        Var v;

        v = Var_getArrayElemByCName(_prefsvar, "screen_width");
        ASSERT(v != NULL, return);
        Var_setInt(v, width);
        v = Var_getArrayElemByCName(_prefsvar, "screen_height");
        ASSERT(v != NULL, return);
        Var_setInt(v, height);
        v = Var_getArrayElemByCName(_prefsvar, "screen_full");
        ASSERT(v != NULL, return);
        Var_setInt(v, fullscreen);
    }

    /*TODO: restore the sound on win32 platforms*/
}

/*----------------------------------------------------------------------------*/
GlEventCollector
graphicsAddEventCollector(GlEventType events, GlEventCallback callback)
{
    GlEventCollector ret;

    ret = MALLOC(sizeof(pv_GlEventCollector));
    ret->events = events;
    ret->callback = callback;

    PtrArray_append(eventcollectors_add, ret);

    return ret;
}

/*----------------------------------------------------------------------------*/
void
graphicsDelEventCollector(GlEventCollector collector)
{
    /*don't delete it immediately because this function can have been called
    inside a callback (so it would mess up the array processing)*/
    PtrArray_append(eventcollectors_del, collector);
}

/*----------------------------------------------------------------------------*/
Gl3DGroup
graphicsCreate3DGroup(Gl3DRenderMode mode)
{
    int i, j;

    if (nbgroups3d == 0)
    {
        groups3d = MALLOC(sizeof(Gl3DGroup));
        i = 0;
    }
    else
    {
        groups3d = REALLOC(groups3d, sizeof(Gl3DGroup) * (nbgroups3d + 1));
        i = 0;
        while ((i < nbgroups3d) && (groups3d[i]->mode <= mode))
        {
            i++;
        }
        for (j = nbgroups3d - 1; j > i; j--)
        {
            groups3d[j + 1] = groups3d[j];
        }
    }
    nbgroups3d++;
    groups3d[i] = MALLOC(sizeof(pv_Gl3DGroup));
    groups3d[i]->mode = mode;
    groups3d[i]->array = PtrArray_newFull(50, 50, NULL, (mode > GL3DRENDER_NORMAL) ? (PtrCmpFunc)Gl3DObject_cmp : NULL);

    return groups3d[i];
}

/*----------------------------------------------------------------------------*/
void
graphicsProcessEvent(GlEvent* event)
{
    GlRect rct;
    PtrArrayIterator i;
    Gl2DObject obj2d;

    switch (event->type)
    {
        case GLEVENT_MOUSE:
            /*handle selection button*/
            if (obj_holdmouse != NULL)
            {
                if (!Gl2DObject_isVisible(obj_holdmouse))
                {
                    obj_holdmouse = NULL;
                }
                else
                {
                    Gl2DObject_processEvent(obj_holdmouse, event);
                    if (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
                    {
                        obj_holdmouse = NULL;
                    }
                    return; /*hold events should not fall in collectors*/
                }
            }
            for (i = PtrArray_STOP(array2d); i > PtrArray_START(array2d); i--)
            {
                /*try to pass the event to each object*/
                obj2d = (Gl2DObject)*(i - 1);

                Gl2DObject_getRect(obj2d, &rct);
                if (Gl2DObject_isVisible(obj2d) && isInRect(&rct, event->event.mouseevent.x, event->event.mouseevent.y))
                {
                    if (Gl2DObject_processEvent(obj2d, event))
                    {
                        /*event has been accepted by the object*/
                        if (event->event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
                        {
                            obj_focus = obj2d;
                            obj_holdmouse = obj2d;
                        }
                        return;
                    }
                }
            }
            if (event->event.mouseevent.type == MOUSEEVENT_BUTTONPRESSED)
            {
                /*no object accepted the event*/
                /*TODO: maybe handle a holdmouse for background*/
                obj_focus = NULL;
            }
            else if (event->event.mouseevent.type == MOUSEEVENT_BUTTONRELEASED)
            {
                if (event->event.mouseevent.button == MOUSEBUTTON_LEFT)
                {
                    event_selection.event.selectionevent.type = SELECTIONEVENT_LEFTCLICK;
                }
                else if (event->event.mouseevent.button == MOUSEBUTTON_RIGHT)
                {
                    event_selection.event.selectionevent.type = SELECTIONEVENT_RIGHTCLICK;
                }
            }
            break;
        case GLEVENT_KEYBOARD:
            if (obj_focus != NULL)
            {
                if (!Gl2DObject_isVisible(obj_focus))
                {
                    obj_focus = NULL;
                }
                else if (Gl2DObject_processEvent(obj_focus, event))
                {
                    return;
                }
            }
            break;
        case GLEVENT_RESIZE:
            event_screenstate.event = event->event;
            PtrArray_foreachWithData(array2d, (PtrFuncWithData)Gl2DObject_processEvent, event);
            break;
        case GLEVENT_CAMERA:
            global_cammoved = TRUE;
            event_camerastate.event = event->event;
        case GLEVENT_SELECTION:
        case GLEVENT_DRAW:
            break;  /*go to the collectors*/
        default:
            return; /*don't let it go to the collectors*/
    }

    /*event collecting*/
    for (i = PtrArray_START(eventcollectors); i != PtrArray_STOP(eventcollectors); i++)
    {
        GlEventCollector ec;

        ec = (GlEventCollector)(*i);
        if (ec->events & event->type)
        {
            ec->callback(ec, event);
        }
    }
}
