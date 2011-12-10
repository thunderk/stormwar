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
 *   Game register                                                            *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "game/internal/register.h"

#include "core/core.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    Uint16 size;            /*Number of flags registered*/
    RegisterResult* data;   /*Registered flags*/
} RegisterArea;

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static WorldCoord _world_w;
static WorldCoord _world_h;
static RegisterArea* _register;

static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID RES_WORLD_WIDTH = CORE_INVALID_ID;
static CoreID RES_WORLD_HEIGHT = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
registerSetSize(WorldCoord w, WorldCoord h)
{
    Uint16 x, y;
    
    for (x = 0; x < _world_w; x++)
    {
        for (y = 0; y < _world_h; y++)
        {
            FREE(_register[y * _world_w + x].data);
        }
    }
    FREE(_register);
    _world_w = w;
    _world_h = h;
    _register = MALLOC(sizeof(RegisterArea) * w * h);
    for (x = 0; x < _world_w; x++)
    {
        for (y = 0; y < _world_h; y++)
        {
            _register[y * _world_w + x].size = 0;
            _register[y * _world_w + x].data = MALLOC(sizeof(RegisterResult));
            _register[y * _world_w + x].data[0].piece = NULL;
        }
    }
}

/*----------------------------------------------------------------------------*/
static void
resourceCallback(CoreID id, Var value)
{
    if (id == RES_WORLD_WIDTH)
    {
        ASSERT(Var_getType(value) == VAR_INT, return);
        
        registerSetSize((WorldCoord)Var_getValueInt(value), _world_h);
    }
    else if (id == RES_WORLD_HEIGHT)
    {
        ASSERT(Var_getType(value) == VAR_INT, return);
        
        registerSetSize(_world_w, (WorldCoord)Var_getValueInt(value));
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
registerInit()
{
    _world_w = _world_h = 1;
    _register = MALLOC(sizeof(RegisterArea));
    _register[0].data = MALLOC(sizeof(RegisterResult));
    _register[0].size = 0;
    _register[0].data[0].piece = NULL;
    
    MOD_ID = coreDeclareModule("register", NULL, NULL, NULL, NULL, resourceCallback, NULL);
    RES_WORLD_WIDTH = coreAddResourceWatcher(MOD_ID, "world_width");
    RES_WORLD_HEIGHT = coreAddResourceWatcher(MOD_ID, "world_height");
}

/*----------------------------------------------------------------------------*/
void
registerUninit()
{
    registerSetSize(1, 1);
    FREE(_register[0].data);
    FREE(_register);
    
    shellPrint(LEVEL_INFO, "Game register unloaded.");
}

/*----------------------------------------------------------------------------*/
void
registerClear()
{
    registerSetSize(_world_w, _world_h);
}

/*----------------------------------------------------------------------------*/
void
registerSetFlag(WorldCoord x, WorldCoord y, Piece piece, RegisterFlag flag)
{
    if ((x >= _world_w) | (y >= _world_h) | (flag == REGISTER_NONE) | (piece == NULL))
    {
        /*TODO: conditional debug message*/
    }
    else
    {
        /*TODO: maybe check that this flag isn't already set.*/
        RegisterArea* reg;
        
        reg = _register + y * _world_w + x;
        reg->data = REALLOC(reg->data, sizeof(RegisterResult) * (reg->size + 2));
        reg->data[reg->size].piece = piece;
        reg->data[reg->size].flag = flag;
        reg->data[++reg->size].piece = NULL;
    }
}

/*----------------------------------------------------------------------------*/
void
registerUnsetFlag(WorldCoord x, WorldCoord y, Piece piece, RegisterFlag flag)
{
    if ((x >= _world_w) | (y >= _world_h) | (flag == REGISTER_NONE) | (piece == NULL))
    {
        /*TODO: conditional debug message*/
    }
    else
    {
        RegisterArea* reg;
        RegisterResult* res;
        
        reg = _register + y * _world_w + x;
        res = reg->data;
        while (res->piece != NULL)
        {
            if ((res->piece == piece) && (res->flag == flag))
            {
                /*Unset this flag*/
                res->piece = reg->data[reg->size].piece;
                res->flag = reg->data[reg->size].flag;
                reg->data[reg->size--].piece = NULL;
                
                /*Don't need to realloc the data stack because we only use its
                  NULL-terminated feature. Though realloc will happen on next registerSetFlag call*/
                
                return;
            }
            
            res++;
        }
    }        
}

/*----------------------------------------------------------------------------*/
Bool
registerIsOutbound(WorldCoord x, WorldCoord y)
{
    return ((x >= _world_w) || (y >= _world_h));
}

/*----------------------------------------------------------------------------*/
RegisterResult*
registerQuery(WorldCoord x, WorldCoord y, Piece piece, Entity* entity, RegisterFlag flag)
{
    RegisterResult* ret;
    
    if ((x >= _world_w) | (y >= _world_h))
    {
        /*return a void list*/
        ret = MALLOC(sizeof(RegisterResult));
        ret[0].piece = NULL;
    }
    else if ((piece == NULL) & (entity == NULL) & (flag == REGISTER_NONE))
    {
        RegisterArea* reg;
        
        /*return a copy of the whole data stack*/
        reg = _register + y * _world_w + x;
        ret = MALLOC(sizeof(RegisterResult) * (reg->size + 1));
        memCOPY(ret, reg->data, sizeof(RegisterResult) * (reg->size + 1));
    }
    else
    {
        int size, asize;    /*results number and allocated size*/
        RegisterArea* reg;
        RegisterResult* res;
        
        /*return the results of a search*/
        size = 0;
        asize = 5;
        ret = MALLOC(sizeof(RegisterResult) * asize);
        ret->piece = NULL;
        reg = _register + y * _world_w + x;
        res = reg->data;
        while (res->piece != NULL)
        {
            if (((piece == NULL) || (res->piece == piece))
             && ((entity == NULL) || (Piece_getEntity(res->piece) == entity))
             && ((flag == REGISTER_NONE) || (res->flag == flag)))
            {
                /*matching criteria, adding result*/
                if ((size + 1) == asize)
                {
                    /*need more room*/
                    asize += 5;
                    ret = REALLOC(ret, sizeof(RegisterResult) * asize);
                }
                ret[size + 1].piece = NULL;
                ret[size++] = *res;
            }
            res++;
        }
    }
    
    /***DEBUG***/
    /*{
        RegisterResult* i;
        printf("***\n");
        i = ret;
        while (i->piece != NULL)
        {
            printf("%s\n", String_get(Piece_getEntity(i->piece)->name));
            i++;
        }
    }*/
    /***DEBUG***/

    return ret;
}

/*----------------------------------------------------------------------------*/
Bool
registerCheck(WorldCoord x, WorldCoord y, Piece piece, Entity* entity, RegisterFlag flag)
{
    if ((x >= _world_w) | (y >= _world_h))
    {
        return FALSE;
    }
    else
    {
        RegisterArea* reg;
        RegisterResult* res;

        reg = _register + y * _world_w + x;
        res = reg->data;
        while (res->piece != NULL)
        {
            if (((piece == NULL) || (res->piece == piece))
             && ((entity == NULL) || (Piece_getEntity(res->piece) == entity))
             && ((flag == REGISTER_NONE) || (res->flag == flag)))
            {
                return TRUE;
            }
            res++;
        }
        return FALSE;
    }
}
