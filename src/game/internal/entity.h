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
 *   Type for the game pieces                                                 *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_ENTITY_H_
#define _SW_ENTITY_H_ 1

/*!
 * \file
 * \brief This is the type for game pieces.
 *
 * An entity is a type that can be used to produce pieces.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"
#include "graphics/glsurface.h"
#include "graphics/glmesh.h"

#include "game/internal/droprules.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Private structure of an entity. */
typedef struct
{
    String name;            /*!< Entity name. */
    GlMesh mesh;            /*!< 3d mesh for this entity. */
    Uint16 width;           /*!< Width in tiles. */
    Uint16 height;          /*!< Height in tiles. */
    float rangedist;        /*!< Range distance. */
    float rangeangle;       /*!< Range angle. */
    Var droprules_var;      /*!< Internal use only !!! */
    DropRules droprules;    /*!< Drop rules. */
    Bool isstatic;          /*!< TRUE if this entity is static, FALSE for dynamic. */
    Bool keepdrop;          /*!< Stay in drop mode. */
    Bool selectable;        /*!< Pieces selectable or not. */
    Bool blended;           /*!< Pieces will be blended (need z-sorting) or not. */
} Entity;

/******************************************************************************
 *############################################################################*
 *#                             Entities functions                           #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create an entity.
 *
 * This will fill the fields of the entity structure to make it valid.
 * This will not allocate the structure in memory.
 * Don't call this on an already valid structure.
 * \param entity - Pointer to an entity structure.
 * \param v - Definition variable.
 */
void Entity_set(Entity* entity, Var v);

/*!
 * \brief Activate an entity.
 *
 * Call this just after all entities have been set.
 * \param entity - The entity to activate.
 */
void Entity_activate(Entity* entity);

/*!
 * \brief Free the memory used by an entity.
 *
 * This will not delete the structure itself in memory.
 * The entity pointed must be considered unvalid after this.
 * \param entity - Pointer to an entity structure.
 */
void Entity_unset(Entity* entity);

#endif
