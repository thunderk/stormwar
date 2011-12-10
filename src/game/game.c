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
 *   Game engine                                                              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/game.h"

#include "core/core.h"

#include "game/internal/game.h"
#include "game/internal/entity.h"
#include "game/internal/player.h"
#include "game/internal/piece.h"
#include "game/internal/register.h"

#include "gui/guitexture.h"

#include "world/world.h"
#include "world/ground.h"

#include "graphics/graphics.h"
#include "graphics/glrect.h"
#include "graphics/color.h"
#include "graphics/gl2dobject.h"
#include "graphics/gl3dobject.h"
#include "graphics/particle.h"
#include "sound/sound.h"

#include "core/string.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static int _entities_nb;
static Entity* _entities;

static PtrArray _players;

static PtrArray _pieces;
static Piece _piece_sel;
#define RANGE_PART_NB 10
static Particle _piece_range_part[RANGE_PART_NB];
static float _piece_range_progr;
static float _piece_range_effect;

static Gl2DObject _selection_frame;
static GlSurface _selection_framesurf;
static GuiTexture _selection_frametex;
static GlRect _selection_framerect;

/*local player use*/
static SoundSample _sound_entity_select;
static SoundSample _sound_entity_drop;

/*core things*/
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID RES_ENTITIES_AVAILABLE = CORE_INVALID_ID;
static CoreID RES_ENTITY_SELECTED = CORE_INVALID_ID;

/******************************************************************************
 *                              Global variables                              *
 ******************************************************************************/
Player global_localplayer;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
selectPiece(Piece piece)
{
    Gl3DCoord x, y, z;
    int i;
    Entity* ent;
    
    _piece_sel = piece;
    
    if (piece != NULL)
    {
        soundPlaySample2D(_sound_entity_select, 1, soundVolume(255), SoundStereo_CENTER, SoundDistance_NEAR);
    }
    else
    {
        GlRect_MAKE(_selection_framerect, -1, -1, 1, 1);
    }

    for (i = 0; i < RANGE_PART_NB; i++)
    {
        Particle_show(_piece_range_part[i], (piece != NULL) && (Piece_getEntity(piece)->rangedist > 0.0f));
        if (piece != NULL)
        {
            ent = Piece_getEntity(piece);
            Piece_getPos(piece, &x, &y, &z);
            Particle_setPos(_piece_range_part[i], x + (Gl3DCoord)ent->width / 2.0, y + 0.5, z + (Gl3DCoord)ent->height / 2.0);
        }
    }
    _piece_range_effect = 0.0f;
}

/*----------------------------------------------------------------------------*/
static Bool
gleventCallback(GlExtID data, GlEvent* event)
{
    (void)data;
    
    if (event->type == GLEVENT_DRAW)
    {
        if (_piece_sel != NULL)
        {
            Gl3DObject obj;
            GlRect rct, rect;
            
            obj = Piece_getObject(_piece_sel);
            rct = Gl3DObject_getRect(obj);
            if (!GlRect_equal(rct, _selection_framerect))
            {
                _selection_framerect = rct;
                if (rct.w * rct.h > 50000)
                {
                    Gl2DObject_show(_selection_frame, FALSE);
                }
                else
                {
                    GlSurface_resize(_selection_framesurf, rct.w, rct.h, SURFACE_UNDEFINED);
                    GlSurface_getRect(_selection_framesurf, &rect);
                    GuiTexture_draw(_selection_frametex, _selection_framesurf, rect);
                    /*GlSurface_clear(_selection_framesurf, GlColor_MAKE(0xFF, 0xFF, 0x80, 0x50));*/
                    Gl2DObject_relink(_selection_frame, _selection_framesurf);
                    Gl2DObject_setPos(_selection_frame, rct.x, rct.y, 0);
                    Gl2DObject_show(_selection_frame, TRUE);
                }
            }
        }
        else
        {
            Gl2DObject_show(_selection_frame, FALSE);
        }
    }
    else if (event->type == GLEVENT_SELECTION)
    {
        if (global_localplayer != NULL)
        {
            Piece p;
            if ((p = Player_processSelectionEvent(global_localplayer, event)) != NULL)
            {
                Entity* entity;
                
                entity = Piece_getEntity(p);
                
                PtrArray_append(_pieces, p);
                soundPlaySample2D(_sound_entity_drop, 1, soundVolume(255), SoundStereo_CENTER, SoundDistance_NEAR);
                if (!entity->keepdrop)
                {
                    Var_setInt(temp_corevar, -1);
                    coreSetResourceValue(MOD_ID, RES_ENTITY_SELECTED, temp_corevar);
                }
                if (entity->isstatic)
                {
                    int x, y;
                    
                    /*set static pieces in the register*/
                    /*TODO: this should be done by the piece itself*/
                    for (x = 0; x < entity->width; x++)
                    {
                        for (y = 0; y < entity->height; y++)
                        {
                            registerSetFlag(event->event.selectionevent.groundx + x, event->event.selectionevent.groundy + y, p, REGISTER_PRESENCE);
                        }
                    }
                }
            }
            else if (event->event.selectionevent.type == SELECTIONEVENT_LEFTCLICK)
            {
                if (!registerIsOutbound(event->event.selectionevent.groundx, event->event.selectionevent.groundy))
                {
                    RegisterResult* res;
                    RegisterResult* i;
                    
                    /*TODO: cycling selection*/                
                    i = res = registerQuery(event->event.selectionevent.groundx, event->event.selectionevent.groundy, NULL, NULL, REGISTER_PRESENCE);
                    while (i->piece != NULL)
                    {
                        if (Piece_getEntity(i->piece)->selectable)
                        {
                            break;
                        }
                        i++;
                    }
                    selectPiece(i->piece);  /*either NULL for deselect or the piece to select*/
                    FREE(res);
                }
            }
        }
    }
    
    return FALSE;
}

/*----------------------------------------------------------------------------*/
static void
resourceCallback(CoreID id, Var value)
{
    if (id == RES_ENTITY_SELECTED)
    {
        if (global_localplayer != NULL)
        {
            ASSERT(Var_getType(value) == VAR_INT, return);
            
            if (Player_selectEntity(global_localplayer, Var_getValueInt(value)) != -1)
            {
                soundPlaySample2D(_sound_entity_select, 1, soundVolume(255), SoundStereo_CENTER, SoundDistance_NEAR);
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
datasCallback(Var datas)
{
    VarValidator valid;
    Var varent;
    int i;
    
    /*quit creation mode*/
    if (global_localplayer != NULL)
    {
        Player_selectEntity(global_localplayer, -1);
    }
    
    /*clear pieces*/
    PtrArray_clear(_pieces);
    selectPiece(NULL);
    
    /*set parameters*/
    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "sound_entity_select", "");
    VarValidator_declareStringVar(valid, "sound_entity_drop", "");
    VarValidator_declareStringVar(valid, "particle_range_tex", "");
    VarValidator_declareArrayVar(valid, "particle_select_tex");
    VarValidator_declareArrayVar(valid, "entities");
    
    VarValidator_validate(valid, datas);
    VarValidator_del(valid);
    
    /*free entities info*/
    if (_entities_nb != 0)
    {
        for (i = 0; i < _entities_nb; i++)
        {
            Entity_unset(_entities + i);
        }
        FREE(_entities);
    }
    
    /*alloc new entities and info*/
    varent = Var_getArrayElemByCName(datas, "entities");
    _entities_nb = Var_getArraySize(varent);
    if (_entities_nb != 0)
    {
        _entities = MALLOC(sizeof(Entity) * _entities_nb);
        
        /*fill these*/
        for (i = 0; i < _entities_nb; i++)
        {
            Entity_set(_entities + i, Var_getArrayElemByPos(varent, i));
        }
        for (i = 0; i < _entities_nb; i++)
        {
            Entity_activate(_entities + i);
        }
    }
    
    GuiTexture_set(_selection_frametex, Var_getArrayElemByCName(datas, "particle_select_tex"));
    for (i = 0; i < RANGE_PART_NB; i++)
    {
        Particle_setTex(_piece_range_part[i], Var_getValueString(Var_getArrayElemByCName(datas, "particle_range_tex")));
        Particle_setSize(_piece_range_part[i], 1.0, 1.0);
        Particle_show(_piece_range_part[i], FALSE);
    }

    _sound_entity_select = soundGetSample(Var_getValueString(Var_getArrayElemByCName(datas, "sound_entity_select")));
    _sound_entity_drop = soundGetSample(Var_getValueString(Var_getArrayElemByCName(datas, "sound_entity_drop")));
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime duration)
{
    (void)thread;
    
    if (_piece_sel != NULL)
    {
        Entity* e;
        Gl3DCoord x, y, z;
        int i;
        
        e = Piece_getEntity(_piece_sel);
        if (!floatEqual(e->rangedist, 0.0))
        {
            if (_piece_range_effect >= 1.0f)
            {
                _piece_range_effect = 1.0f;
            }
            else
            {
                _piece_range_effect += (float)duration / 500.0;
            }
            _piece_range_progr += (float)duration / 1500.0;
            while (_piece_range_progr >= 1.0)
            {
                _piece_range_progr -= 1.0;
            }
            Piece_getPos(_piece_sel, &x, &y, &z);
            for (i = 0; i < RANGE_PART_NB; i++)
            {
                Particle_setPos(_piece_range_part[i], x + ((Gl3DCoord)e->width / 2.0) + e->rangedist * _piece_range_effect * cos(e->rangeangle * (_piece_range_progr + (float)i / RANGE_PART_NB))
                                                    , y + 0.5
                                                    , z + ((Gl3DCoord)e->height / 2.0) + e->rangedist * _piece_range_effect * sin(e->rangeangle * (_piece_range_progr + (float)i / RANGE_PART_NB)));
            }
        }
    }
}

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
void
gameUpdateEntitiesAvailable(int nb, Entity** entities)
{
    Var v;
    Var vs;
    int i;
    
    vs = Var_new();
    v = Var_new();
    Var_setArray(v);
    
    for (i = 0; i < nb; i++)
    {
        Var_setString(vs, entities[i]->name);
        Var_addToArray(v, vs);
    }
    
    coreSetResourceValue(MOD_ID, RES_ENTITIES_AVAILABLE, v);
    Var_del(vs);
    Var_del(v);
}

/*----------------------------------------------------------------------------*/
Entity*
gameGetAllEntities(int* r_nb)
{
    *r_nb = _entities_nb;
    return _entities;
}

/*----------------------------------------------------------------------------*/
Entity*
gameGetEntity(String name)
{
    int i;
    
    for (i = 0; i < _entities_nb; i++)
    {
        if (String_equal(_entities[i].name, name))
        {
            return _entities + i;
        }
    }
    
    return NULL;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
gameInit()
{
    int i;
    
    registerInit();
    gameCameraInit();
    
    /*entities*/
    _entities_nb = 0;
    _entities = NULL;

    /*players*/
    _players = PtrArray_newFull(5, 2, (PtrFunc)Player_del, NULL);
    global_localplayer = NULL;
    
    /*pieces*/
    _pieces = PtrArray_newFull(50, 50, (PtrFunc)Piece_del, NULL);
    _piece_sel = NULL;
    for (i = 0; i < RANGE_PART_NB; i++)
    {
        _piece_range_part[i] = Particle_new();
    }
    _piece_range_progr = 0.0f;
    _piece_range_effect = 0.0;
    
    /*selection*/
    GlRect_MAKE(_selection_framerect, -1, -1, 1, 1);
    _selection_framesurf = GlSurface_new(1, 1, TRUE);
    _selection_frametex = GuiTexture_new();
    _selection_frame = Gl2DObject_new(_selection_framesurf, NULL, gleventCallback);

    graphicsAddEventCollector(GLEVENT_SELECTION | GLEVENT_DRAW, gleventCallback);

    /*sounds*/
    _sound_entity_select = SoundSample_NULL;
    _sound_entity_drop = SoundSample_NULL;
    
    /*core declaration*/
    MOD_ID = coreDeclareModule("game", NULL, datasCallback, NULL, NULL, resourceCallback, threadCallback);
    RES_ENTITIES_AVAILABLE = coreCreateResource(MOD_ID, "entities_available", VAR_ARRAY, FALSE);
    RES_ENTITY_SELECTED = coreCreateResource(MOD_ID, "entity_selected", VAR_INT, TRUE);
    coreAddResourceWatcher(MOD_ID, "entity_selected");
    coreRequireThreadSlot(MOD_ID, coreGetThreadID(NULL));
}

/*----------------------------------------------------------------------------*/
void
gameUninit()
{
    int i;

    /*delete pieces and particules*/
    PtrArray_del(_pieces);
    Gl2DObject_del(_selection_frame);
    GuiTexture_del(_selection_frametex);
    GlSurface_del(_selection_framesurf);
    for (i = 0; i < RANGE_PART_NB; i++)
    {
        Particle_del(_piece_range_part[i]);
    }
    
    /*delete players*/
    PtrArray_del(_players);
    
    /*then entities*/
    if (_entities_nb != 0)
    {
        for (i = 0; i < _entities_nb; i++)
        {
            Entity_unset(_entities + i);
        }
        FREE(_entities);
    }
    
    gameCameraUninit();
    registerUninit();
    
    shellPrint(LEVEL_INFO, "Game engine destroyed.");
}

/*----------------------------------------------------------------------------*/
void
gameNew()
{
    global_localplayer = NULL;
    groundClear();
    PtrArray_clear(_pieces);
    selectPiece(NULL);
    PtrArray_clear(_players);
    registerClear();
    
    shellPrint(LEVEL_INFO, "New game initialized. Waiting for players.");
}

/*----------------------------------------------------------------------------*/
void
gameAddLocalPlayer(String name)
{
    if (PtrArray_SIZE(_players) > 0)
    {
        shellPrintf(LEVEL_ERROR, "Can't add local player '%s', must be declared first.", String_get(name));
    }
    else
    {
        Player p;
        
        p = Player_new(PLAYER_LOCAL, name);
        if (p != NULL)
        {
            PtrArray_append(_players, p);
            global_localplayer = p;
        }
        
        shellPrintf(LEVEL_INFO, "Local player '%s' added.", String_get(name));
    }
}
