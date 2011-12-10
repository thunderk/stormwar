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
 *   Internal graphics functions                                              *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_GRAPHICS_IMPL_IMPL_H_
#define _SW_GRAPHICS_IMPL_IMPL_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "graphics/types.h"
#include "graphics/graphics.h"
#include "graphics/impl/opengl.h"
#include "graphics/particle.h"
#include "core/types.h"
#include "graphics/gliterator.h"
#include "SDL.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct pv_GlMeshControl pv_GlMeshControl;
    
typedef pv_GlMeshControl* GlMeshControl;

typedef struct
{
    Bool drawn;
    Bool check;             /*need control points check because camera moved*/
    Bool forcecheck;        /*force control points check*/
    GlRect rct;
    Gl3DCoord z;
} GlMeshInfo;

/******************************************************************************
 *                                  Globals                                   *
 ******************************************************************************/
extern Bool global_pause;
extern CoreTime global_frametime;
extern Gl3DCoord global_camangh;
extern Gl3DCoord global_camangv;
extern Bool global_cammoved;    /*TRUE if the camera moved*/
extern Bool global_forceblend;  /*defined in opengl.c*/
extern Uint32 global_checkcount;  /*number of 3d objects with check==TRUE that have been treated in checked frame*/

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
OpenGLTexture gltexturesReserveTex(void);
void gltexturesDeleteTex(OpenGLTexture tex);

void GlSurface_initIterator(GlSurface surf, GlIterator* it);
SDL_Surface* GlSurface_getSDLSurface(GlSurface surf);

void colorInit(void);

void gltextInit(void);
void gltextUninit(void);
void gltextClearFonts(void);
void gltextSetFonts(Var vfonts);

void inputInit(void);
void inputUninit(void);
void inputCollectEvents(void);
void inputSetFullscreen(Bool fs);

void lightInit(void);
void lightUninit(void);
void lightSetScene(void);

void cameraInit(void);
void cameraUninit(void);
void cameraSetSceneNormal(void);
void cameraSetSceneBackground(void);
void cameraPushObject(Gl3DCoord x, Gl3DCoord y, Gl3DCoord z, Gl3DCoord angh, Gl3DCoord angv);
void cameraPopObject(void);
void cameraCollectEvents(void);
void cameraGetRealPos(Gl3DCoord* cx, Gl3DCoord* cy, Gl3DCoord* cz);

void graphicsProcessEvent(GlEvent* event);
void graphicsAdd2DObject(Gl2DObject obj);
void graphicsDel2DObject(Gl2DObject obj);
void graphicsAdd3DObject(Gl3DObject obj);
void graphicsDel3DObject(Gl3DObject obj);
void graphicsAddParticle(Particle obj);
void graphicsDelParticle(Particle obj);
Bool graphicsIsGroupZSorted(Gl3DGroup group);

void Gl2DObject_dropTextures(Gl2DObject obj);
void Gl2DObject_uploadTextures(Gl2DObject obj);
Bool Gl2DObject_processEvent(Gl2DObject obj, GlEvent* event);
int Gl2DObject_cmp(Gl2DObject* obj1, Gl2DObject* obj2);

Bool Gl3DObject_processEvent(Gl3DObject obj, GlEvent* event);
int Gl3DObject_cmp(Gl3DObject* obj1, Gl3DObject* obj2);

Bool Particle_processEvent(Particle obj, GlEvent* event);

void GlMesh_draw(GlMesh mesh, GlMeshControl control, GlMeshInfo* info);
void GlMesh_makeControl(GlMesh mesh, GlMeshControl* control_p);
void GlMesh_linkControl(GlMesh mesh, GlMeshControl control, String anim, CoreTime time, unsigned int repeats);

#endif
