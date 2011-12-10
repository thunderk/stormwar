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
 *   Iterator to progress through a GlSurface                                 *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "tools/fonct.h"
#include "graphics/gliterator.h"
#include "graphics/impl/impl.h"

/******************************************************************************
 *############################################################################*
 *#                            GlIterator functions                          #*
 *############################################################################*
 ******************************************************************************/
void
GlIterator_init(GlIterator* iterator, GlSurface surf)
{
    GlSurface_initIterator(surf, iterator);
    /*iterator->defaultrct = iterator->rct = surf->clip_rect;
    iterator->yprogr = surf->pitch / surf->format->BytesPerPixel;
    iterator->pos = iterator->initpos = (GlColor*)surf->pixels;*/
    iterator->x = iterator->y = 0;
    iterator->xlim = iterator->rct.w - 1;
    iterator->ylim = iterator->rct.h - 1;
    iterator->end = FALSE;
}

/*----------------------------------------------------------------------------*/
void
GlIterator_fwd(GlIterator* iterator)
{
    iterator->end = FALSE;
    if (iterator->x == iterator->xlim)
    {
        iterator->pos -= iterator->x;
        iterator->x = 0;
        if (iterator->y == iterator->ylim)
        {
            iterator->pos -= (iterator->y * iterator->yprogr);
            iterator->y = 0;
            iterator->end = TRUE;
        }
        else
        {
            iterator->y++;
            iterator->pos += iterator->yprogr;
        }
    }
    else
    {
        iterator->x++;
        iterator->pos++;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_fwdCoupled(GlIterator* iterator1, GlIterator* iterator2)
{
    iterator1->end = FALSE;
    iterator2->end = FALSE;
    if (iterator1->x == iterator1->xlim)
    {
        iterator1->pos -= iterator1->x;
        iterator2->pos -= iterator1->x;
        iterator1->x = 0;
        iterator2->x = 0;
        if (iterator1->y == iterator1->ylim)
        {
            iterator1->pos -= (iterator1->y * iterator1->yprogr);
            iterator2->pos -= (iterator1->y * iterator2->yprogr);
            iterator1->y = 0;
            iterator2->y = 0;
            iterator1->end = TRUE;
            iterator2->end = TRUE;
        }
        else
        {
            iterator1->y++;
            iterator2->y++;
            iterator1->pos += iterator1->yprogr;
            iterator2->pos += iterator2->yprogr;
        }
    }
    else
    {
        iterator1->x++;
        iterator2->x++;
        iterator1->pos++;
        iterator2->pos++;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_bwd(GlIterator* iterator)
{
    iterator->end = FALSE;
    if (iterator->x == 0)
    {
        iterator->x = iterator->xlim;
        iterator->pos += iterator->x;
        if (iterator->y == 0)
        {
            iterator->y = iterator->ylim;
            iterator->pos += (iterator->y * iterator->yprogr);
            iterator->end = TRUE;
        }
        else
        {
            iterator->y--;
            iterator->pos -= iterator->yprogr;
        }
    }
    else
    {
        iterator->x--;
        iterator->pos--;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_bwdCoupled(GlIterator* iterator1, GlIterator* iterator2)
{
    iterator1->end = FALSE;
    iterator2->end = FALSE;
    if (iterator1->x == 0)
    {
        iterator1->x = iterator1->xlim;
        iterator2->x = iterator1->xlim;
        iterator1->pos += iterator1->x;
        iterator2->pos += iterator1->x;
        if (iterator1->y == 0)
        {
            iterator1->y = iterator1->ylim;
            iterator2->y = iterator1->ylim;
            iterator1->pos += (iterator1->y * iterator1->yprogr);
            iterator2->pos += (iterator1->y * iterator2->yprogr);
            iterator1->end = TRUE;
            iterator2->end = TRUE;
        }
        else
        {
            iterator1->y--;
            iterator2->y--;
            iterator1->pos -= iterator1->yprogr;
            iterator2->pos -= iterator2->yprogr;
        }
    }
    else
    {
        iterator1->x--;
        iterator2->x--;
        iterator1->pos--;
        iterator2->pos--;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_up(GlIterator* iterator)
{
    if (iterator->y == 0)
    {
        iterator->end = TRUE;
    }
    else
    {
        iterator->y--;
        iterator->pos -= iterator->yprogr;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_upCoupled(GlIterator* iterator1, GlIterator* iterator2)
{
    if (iterator1->y == 0)
    {
        iterator1->end = iterator2->end = TRUE;
    }
    else
    {
        iterator1->y--;
        iterator2->y--;
        iterator1->pos -= iterator1->yprogr;
        iterator2->pos -= iterator2->yprogr;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_down(GlIterator* iterator)
{
    if (iterator->y == iterator->ylim)
    {
        iterator->end = TRUE;
    }
    else
    {
        iterator->y++;
        iterator->pos += iterator->yprogr;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_downCoupled(GlIterator* iterator1, GlIterator* iterator2)
{
    if (iterator1->y == iterator1->ylim)
    {
        iterator1->end = iterator2->end = TRUE;
    }
    else
    {
        iterator1->y++;
        iterator2->y++;
        iterator1->pos += iterator1->yprogr;
        iterator2->pos += iterator2->yprogr;
    }
}

/*----------------------------------------------------------------------------*/
void
GlIterator_setPos(GlIterator* iterator, Gl2DCoord x, Gl2DCoord y)
{
    iterator->x = x;
    iterator->y = y;
    iterator->pos = (GlColor*)(iterator->initpos + ((y + iterator->rct.y) * iterator->yprogr) + (x + iterator->rct.x));
    iterator->end = FALSE;
}

/*----------------------------------------------------------------------------*/
void
GlIterator_setClipRect(GlIterator* iterator, GlRect* rct, GlIteratorCorner corner)
{
    if (rct == NULL)
    {
        iterator->rct = iterator->defaultrct;
    }
    else
    {
        iterator->rct = *rct;
    }

    iterator->xlim = iterator->rct.w - 1;
    iterator->ylim = iterator->rct.h - 1;
    iterator->end = FALSE;
    
    iterator->pos = iterator->initpos + iterator->rct.y * iterator->yprogr + iterator->rct.x;

    switch (corner)
    {
        case CORNER_TOPLEFT:
            iterator->x = 0;
            iterator->y = 0;
            break;
        case CORNER_TOPRIGHT:
            iterator->x = iterator->xlim;
            iterator->y = 0;
            iterator->pos += iterator->xlim;
            break;
        case CORNER_BOTTOMLEFT:
            iterator->x = 0;
            iterator->y = iterator->ylim;
            iterator->pos += iterator->ylim * iterator->yprogr;
            break;
        case CORNER_BOTTOMRIGHT:
            iterator->x = iterator->xlim;
            iterator->y = iterator->ylim;
            iterator->pos += iterator->ylim * iterator->yprogr + iterator->xlim;
            break;
    }
}
