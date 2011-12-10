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
 *   Graphical surface management                                             *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "SDL.h"    /*to make GlRect work*/
#include "graphics/glsurface.h"

#include "graphics/glrect.h"
#include "graphics/color.h"
#include "graphics/gliterator.h"

#include "core/core.h"
#include "core/string.h"
#include "tools/fonct.h"

#include "SDL_image.h"

#include "graphics/impl/opengl.h"   /*for GLU*/

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
struct pv_GlSurface
{
    SDL_Surface* surf;      /*Associated SDL surface.*/
    GlRect rct;             /*Surface rectangle (0,0,w,h).*/
    GlColor* pixels;        /*Pointer to pixels data.*/
    Uint32 lineskip;        /*Number of GlColor on a line.*/
};

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static Bool
codePoint(Sint16 x, Sint16 y, Sint16 xmax, Sint16 ymax, Bool code[])
{
    Bool ret;
    Bool bety;
    unsigned int i;
    
    for (i = 0; i < 4; i++)
    {
        code[i] = FALSE;
    }
    ret = FALSE;
    bety = FALSE;
    if (y > ymax)
    {
        code[0] = TRUE;
    }
    else
    {
        if (y < 0)
        {
            code[1] = TRUE;
        }
        else
        {
            bety = TRUE;
        }
    }
    if (x > xmax)
    {
        code[2] = TRUE;
    }
    else
    {
        if (x < 0)
        {
            code[3] = TRUE;
        }
        else
        {
            if (bety)
            {
                ret = TRUE;
            }
        }
    }
    return ret;
}

/*----------------------------------------------------------------------------*/
static Bool
prodCode(Bool codestart[], Bool codeend[])
{
    unsigned int i;
    Bool ret;
    
    ret = TRUE;
    i = 0;
    while ((i < 4) && ret)
    {
        ret &= !(codestart[i] & codeend[i]);
        i++;
    }
    return ret;
}

/******************************************************************************
 *############################################################################*
 *#                             Internal functions                           #*
 *############################################################################*
 ******************************************************************************/
void 
GlSurface_initIterator(GlSurface surf, GlIterator* it)
{
    it->defaultrct = it->rct = surf->rct;
    it->yprogr = surf->lineskip;
    it->pos = it->initpos = surf->pixels;
}

/*----------------------------------------------------------------------------*/
SDL_Surface*
GlSurface_getSDLSurface(GlSurface surf)
{
    return surf->surf;
}

/******************************************************************************
 *############################################################################*
 *#                            GlSurface functions                           #*
 *############################################################################*
 ******************************************************************************/
GlSurface GlSurface_new(Gl2DSize width, Gl2DSize height, Bool alpha)
{
    GlSurface ret;
    SDL_Surface* surf;
    SDL_Surface* surfconv;
    Uint32 flags;
    Uint32 rmask, gmask, bmask, amask;
    
    ret = (GlSurface)MALLOC(sizeof(pv_GlSurface));

    /*Create a 32-bit surface with the bytes of each pixel in R,G,B,A order*/
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif

    width = MAX(width, 1);
    height = MAX(height, 1);
    
    flags = SDL_SWSURFACE;
    if (alpha)
    {
        flags |= SDL_SRCALPHA;
    }

    surf = SDL_CreateRGBSurface(flags, width, height, 32, rmask, gmask, bmask, amask);

    if (surf == NULL)
    {
        FREE(ret);
        error("GlSurface", "GlSurface_new", "Can't create SDL surface.");
        return NULL;
    }

    /*now we convert this surface to the screen format*/
    surfconv = SDL_DisplayFormatAlpha(surf);
    SDL_FreeSurface(surf);
    if (surfconv == NULL)
    {
        FREE(ret);
        error("GlSurface", "GlSurface_new", "Can't convert SDL surface.");
        return NULL;
    }
    
    ret->surf = (SDL_Surface*)debugALLOC(surfconv, sizeof(GlColor) * width * height + sizeof(SDL_Surface));
    GlRect_MAKE(ret->rct, 0, 0, width, height);
    ret->pixels = surfconv->pixels;
    ret->lineskip = surfconv->pitch / surfconv->format->BytesPerPixel;
    
    return ret;
}

/*----------------------------------------------------------------------------*/
GlSurface
GlSurface_newByCopy(GlSurface src)
{
    GlSurface ret;
    
    ret = GlSurface_new(src->rct.w, src->rct.h, (src->surf->flags & SDL_SRCALPHA) != 0);
    GlSurface_copyRect(src, ret, NULL, NULL);
    
    return ret;
}

/*----------------------------------------------------------------------------*/
GlSurface
GlSurface_newFromFile(String imagefile)
{
    SDL_Surface* surf;
    SDL_Surface* surfconv;
    GlSurface ret;
    String file = String_newByCopy(imagefile);
    coreFindData(file);
    
    surf = IMG_Load(String_get(file));

    if (surf == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Can't open image file '%s', fall back on a default surface.", String_get(imagefile));
        String_del(file);
        return GlSurface_new(1, 1, FALSE);
    }

    /*now we convert this surface to the screen format*/
    surfconv = SDL_DisplayFormatAlpha(surf);
    SDL_FreeSurface(surf);
    if (surfconv == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Can't convert image file '%s', fall back on a default surface.", String_get(imagefile));
        String_del(file);
        return GlSurface_new(1, 1, FALSE);
    }

    ret = MALLOC(sizeof(pv_GlSurface));
    GlRect_MAKE(ret->rct, 0, 0, surfconv->w, surfconv->h);
    ret->surf = (SDL_Surface*)debugALLOC(surfconv, sizeof(GlColor) * ret->rct.w * ret->rct.h + sizeof(SDL_Surface));
    ret->pixels = surfconv->pixels;
    ret->lineskip = surfconv->pitch / surfconv->format->BytesPerPixel;
    
    String_del(file);
    return ret;
}

/*----------------------------------------------------------------------------*/
void
GlSurface_del(GlSurface surf)
{
    SDL_FreeSurface((SDL_Surface*)debugFREE(surf->surf));
    FREE(surf);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_resize(GlSurface surf, Gl2DSize w, Gl2DSize h, GlSurfaceResizeMode mode)
{
    GlSurface fakenewsurf;
    SDL_Surface* newsurf;
    SDL_Surface* oldsurf = surf->surf;

    w = MAX(w, 1);
    h = MAX(h, 1);
    fakenewsurf = GlSurface_new(w, h, ((oldsurf->flags & SDL_SRCALPHA) != 0));    /*we use original flags*/
    newsurf = fakenewsurf->surf;

    if (mode == SURFACE_KEEP)
    {
        GlSurface_copyRectSecure(surf, fakenewsurf, NULL, NULL);
    }
    else if (mode == SURFACE_RESAMPLE)
    {
        gluScaleImage(
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
        GL_RGBA,
#else
        GL_BGRA,
#endif
        surf->rct.w, surf->rct.h, GL_UNSIGNED_BYTE, surf->pixels, w, h, GL_UNSIGNED_BYTE, newsurf->pixels);
    }
    
    surf->rct.w = w;
    surf->rct.h = h;
    surf->pixels = newsurf->pixels;
    surf->lineskip = newsurf->pitch / newsurf->format->BytesPerPixel;
    
    /*delete the old surface*/
    SDL_FreeSurface((SDL_Surface*)debugFREE(oldsurf));
    /*steal the new one*/
    surf->surf = newsurf;
    /*free the fake object*/
    FREE(fakenewsurf);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_saveToPNM(GlSurface surf, String file)
{
    FILE* f;
    GlIterator it;
    GlColor col;
    
    f = fopen(String_get(file), "r");
    if (f != NULL)
    {
        fclose(f);
        shellPrintf(LEVEL_ERROR, "Existing file '%s' will not be overwritten for PNM writing.", String_get(file));
        return;
    }
    
    f = fopen(String_get(file), "w");
    if (f == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Unable to open file '%s' for PNM writing.", String_get(file));
        return;
    }
    
    fprintf(f, "P3\n# CREATOR: StormWar "VERSION"\n%d %d\n255\n", ((SDL_Surface*)surf)->w, ((SDL_Surface*)surf)->h);
    GlIterator_init(&it, surf);
    while (!GlIterator_endReached(it))
    {
        col = GlIterator_getColor(it);
        fprintf(f, "%d\n%d\n%d\n", GlColor_GETR(col), GlColor_GETG(col), GlColor_GETB(col));
        GlIterator_fwd(&it);
    }
    fclose(f);
    
    shellPrintf(LEVEL_USER, _("Surface saved to: %s"), String_get(file));
}

/*----------------------------------------------------------------------------*/
Gl2DSize
GlSurface_getWidth(GlSurface surface)
{
    return surface->rct.w;
}

/*----------------------------------------------------------------------------*/
Gl2DSize
GlSurface_getHeight(GlSurface surface)
{
    return surface->rct.h;
}

/*----------------------------------------------------------------------------*/
void
GlSurface_getRect(GlSurface surface, GlRect* rect)
{
    *rect = surface->rct;
}

/*----------------------------------------------------------------------------*/
GlColor*
GlSurface_getPixels(GlSurface surface)
{
    return surface->pixels;
}

/*----------------------------------------------------------------------------*/
GlColor
GlSurface_getColor(GlSurface surf, Gl2DCoord x, Gl2DCoord y)
{
    ASSERT(x >= 0, return 0x00000000);
    ASSERT(y >= 0, return 0x00000000);
    ASSERT(x < surf->rct.w, return 0x00000000);
    ASSERT(y < surf->rct.h, return 0x00000000);
    
    return *(surf->pixels + y * surf->lineskip + x);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_clear(GlSurface surface, GlColor color)
{
    SDL_FillRect(surface->surf, NULL, color);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawPixel(GlSurface surf, Gl2DCoord x, Gl2DCoord y, GlColor col)
{
    GlColor* pos;
    
    ASSERT(x >= 0, return);
    ASSERT(y >= 0, return);
    ASSERT(x < surf->rct.w, return);
    ASSERT(y < surf->rct.h, return);

    pos = surf->pixels + y * surf->lineskip + x;
    *pos = col;
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawPixelSecure(GlSurface surf, Gl2DCoord x, Gl2DCoord y, GlColor col)
{
    GlColor* pos;
    
    if ((x >= 0) && (y >= 0) && (x < surf->rct.w) && (y < surf->rct.h))
    {
        pos = surf->pixels + y * surf->lineskip + x;
        *pos = col;
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawRect(GlSurface surf, GlRect rct, GlColor colfill)
{
    ASSERT(rct.x >= 0, return);
    ASSERT(rct.y >= 0, return);
    ASSERT(rct.x + rct.w <= surf->rct.w, return);
    ASSERT(rct.y + rct.h <= surf->rct.h, return);
    
    SDL_FillRect(surf->surf, &rct, colfill);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawBorderRect(GlSurface surf, GlRect rct, Gl2DSize border, GlColor colborder, GlColor colfill)
{
    ASSERT(rct.x + rct.w <= surf->rct.w, return);
    ASSERT(rct.y + rct.h <= surf->rct.h, return);
    
    if (border != 0)
    {
        /*TODO: test if border isn't too large*/
        SDL_FillRect(surf->surf, &rct, colborder);
        rct.w -= 2 * border;
        rct.h -= 2 * border;
        rct.x += border;
        rct.y += border;
    }
    SDL_FillRect(surf->surf, &rct, colfill);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawLine(GlSurface surf, Gl2DCoord x1, Gl2DCoord y1, Gl2DCoord x2, Gl2DCoord y2, GlColor col)
{
    /*incremental Bresenham algorithm*/
    Uint16 dx, dy, dxy, i;
    Sint16 stx, sty, d;
    GlColor* pos;
    
    ASSERT(x1 >= 0, return);
    ASSERT(x2 >= 0, return);
    ASSERT(y1 >= 0, return);
    ASSERT(y2 >= 0, return);
    ASSERT(x1 < surf->rct.w, return);
    ASSERT(x2 < surf->rct.w, return);
    ASSERT(y1 < surf->rct.h, return);
    ASSERT(y2 < surf->rct.h, return);

    dx = iabs(x2 - x1);
    dy = iabs(y2 - y1);

    stx = (x2 > x1) ? 1 : -1;
    sty = (y2 > y1) ? 1 : -1;

    pos = surf->pixels + y1 * surf->lineskip + x1;
    *pos = col;

    if (dx > dy)
    {
        dy = 2 * dy;
        dxy = 2 * dx - dy;
        d = dx - dy;
        for (i = 1; i <= dx; i++)
        {
            if (d < 0)
            {
                pos += sty * surf->lineskip;
                d += dxy;
            }
            else
            {
                d -= dy;
            }
            pos += stx;
            *pos = col;
        }
    }
    else
    {
        dx = 2 * dx;
        dxy = 2 * dy - dx;
        d = dy - dx;
        for (i = 1; i <= dy; i++)
        {
            if (d < 0)
            {
                pos += stx;
                d += dxy;
            }
            else
            {
                d -= dx;
            }
            pos += sty * surf->lineskip;
            *pos = col;
        }
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawLineSecure(GlSurface surf, Gl2DCoord x1, Gl2DCoord y1, Gl2DCoord x2, Gl2DCoord y2, GlColor col)
{
    /*incremental Bresenham algorithm*/
    Uint16 dx, dy, dxy, i;
    Sint16 stx, sty, d;

    dx = iabs(x2 - x1);
    dy = iabs(y2 - y1);

    stx = (x2 > x1) ? 1 : -1;
    sty = (y2 > y1) ? 1 : -1;

    GlSurface_drawPixelSecure(surf, x1, y1, col);

    if (dx > dy)
    {
        dy = 2 * dy;
        dxy = 2 * dx - dy;
        d = dx - dy;
        for (i = 1; i < dx; i++)
        {
            if (d < 0)
            {
                y1 += sty;
                d += dxy;
            }
            else
            {
                d -= dy;
            }
            x1 += stx;
            GlSurface_drawPixelSecure(surf, x1, y1, col);
        }
    }
    else
    {
        dx = 2 * dx;
        dxy = 2 * dy - dx;
        d = dy - dx;
        for (i = 1; i < dy; i++)
        {
            if (d < 0)
            {
                x1 += stx;
                d += dxy;
            }
            else
            {
                d -= dx;
            }
            y1 += sty;
            GlSurface_drawPixelSecure(surf, x1, y1, col);
        }
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawLineCut(GlSurface surf, Gl2DCoord x1, Gl2DCoord y1, Gl2DCoord x2, Gl2DCoord y2, GlColor col)
{
    Bool codestart[4];
    Bool codeend[4];
    Gl2DCoord xm, ym;
    
    /*dichotomic (recursive) cutting algorithm*/
    
    if (codePoint(x1, y1, surf->rct.w - 1, surf->rct.h - 1, codestart) & codePoint(x2, y2, surf->rct.w - 1, surf->rct.h - 1, codeend))
    {
        GlSurface_drawLine(surf, x1, y1, x2, y2, col);
    }
    else
    {
        if (prodCode(codestart, codeend))
        {
            xm = (x1 + x2) / 2;
            ym = (y1 + y2) / 2;
            if (((xm != x1) && (xm != x2)) || ((ym != y1) && (ym != y2)))
            {
                GlSurface_drawLineCut(surf, x1, y1, xm, ym, col);
                GlSurface_drawLineCut(surf, xm, ym, x2, y2, col);
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawCircle(GlSurface surf, Gl2DCoord centerx, Gl2DCoord centery, Gl2DSize radius, GlColor col)
{
    Gl2DCoord x, y, d, d1, d2;

    ASSERT(centerx >= radius, return)
    ASSERT(centery >= radius, return)
    ASSERT(centerx - radius >= 0, return)
    ASSERT(centery - radius >= 0, return)
    ASSERT(centerx + radius < surf->rct.w, return)
    ASSERT(centery + radius < surf->rct.h, return)
    
    x = 0;
    y = radius;
    d = 3 - 2 * radius;
    d1 = 6;
    d2 = 4 * (1 - radius);

    while (x <= y)
    {
        if (x != 0)
        {
            GlSurface_drawPixel(surf, centerx + x, centery + y, col);
            GlSurface_drawPixel(surf, centerx + y, centery - x, col);
            GlSurface_drawPixel(surf, centerx - x, centery - y, col);
            GlSurface_drawPixel(surf, centerx - y, centery + x, col);
        }
        if (x != y)
        {
            GlSurface_drawPixel(surf, centerx + x, centery - y, col);
            GlSurface_drawPixel(surf, centerx - y, centery - x, col);
            GlSurface_drawPixel(surf, centerx - x, centery + y, col);
            GlSurface_drawPixel(surf, centerx + y, centery + x, col);
        }

        d += d1;
        if (d >= d1)
        {
            d += d2;
            y--;
            d2 += 4;
        }
        d1 += 4;
        x++;
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_drawCircleSecure(GlSurface surf, Gl2DCoord centerx, Gl2DCoord centery, Gl2DSize radius, GlColor col)
{
    Gl2DCoord x, y, d, d1, d2;

    x = 0;
    y = radius;
    d = 3 - 2 * radius;
    d1 = 6;
    d2 = 4 * (1 - radius);

    while (x <= y)
    {
        if (x != 0)
        {
            GlSurface_drawPixelSecure(surf, centerx + x, centery + y, col);
            GlSurface_drawPixelSecure(surf, centerx + y, centery - x, col);
            GlSurface_drawPixelSecure(surf, centerx - x, centery - y, col);
            GlSurface_drawPixelSecure(surf, centerx - y, centery + x, col);
        }
        if (x != y)
        {
            GlSurface_drawPixelSecure(surf, centerx + x, centery - y, col);
            GlSurface_drawPixelSecure(surf, centerx - y, centery - x, col);
            GlSurface_drawPixelSecure(surf, centerx - x, centery + y, col);
            GlSurface_drawPixelSecure(surf, centerx + y, centery + x, col);
        }

        d += d1;
        if (d >= d1)
        {
            d += d2;
            y--;
            d2 += 4;
        }
        d1 += 4;
        x++;
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_doBlit(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect)
{
    GlRect srcrct;
    GlRect destrct;
    
    if (srcrect != NULL)
    {
        srcrct = *srcrect;
    }
    if (destrect != NULL)
    {
        destrct = *destrect;
    }
    
    SDL_BlitSurface(src->surf, (srcrect == NULL) ? NULL : &srcrct, dest->surf, (destrect == NULL) ? NULL : &destrct);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_copyRect(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect)
{
    GlIterator itdest;
    GlIterator itsrc;
    Uint32 time;
    
    time = getTicks();
    
    GlIterator_init(&itdest, dest);
    GlIterator_init(&itsrc, src);

    GlIterator_setClipRect(&itsrc, srcrect, CORNER_TOPLEFT);
    GlIterator_setClipRect(&itdest, destrect, CORNER_TOPLEFT);
    
    ASSERT(GlIterator_getRect(itsrc).w == GlIterator_getRect(itdest).w, return);
    ASSERT(GlIterator_getRect(itsrc).h == GlIterator_getRect(itdest).h, return);

    while (!GlIterator_endReached(itsrc))    /*rects should have same size*/
    {
        memCOPY(itdest.pos, itsrc.pos, (size_t)GlIterator_getLineSize(itsrc));
        GlIterator_downCoupled(&itsrc, &itdest);
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_copyRectTiled(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect)
{
    GlRect overall_rectsrc;
    GlRect overall_rectdest;
    GlRect rectsrc;
    GlRect rectdest;
    Uint16 w;
    if (srcrect == NULL)
    {
        overall_rectsrc = src->rct;
    }
    else
    {
        overall_rectsrc = *srcrect;
    }
    if (destrect == NULL)
    {
        overall_rectdest = dest->rct;
    }
    else
    {
        overall_rectdest = *destrect;
    }
    
    rectsrc.x = overall_rectsrc.x;
    rectsrc.y = overall_rectsrc.y;
    rectdest.y = overall_rectdest.y;
    while (overall_rectdest.h > 0)
    {
        /*we tile a 'row'*/
        w = overall_rectdest.w;
        rectdest.x = overall_rectdest.x;
        rectsrc.h = rectdest.h = MIN(overall_rectdest.h, overall_rectsrc.h);
        while (w > 0)
        {
            rectsrc.w = rectdest.w = MIN(w, overall_rectsrc.w);
            GlSurface_copyRect(src, dest, &rectsrc, &rectdest);
            rectdest.x += rectdest.w;
            w -= rectdest.w;
        }
        rectdest.y += rectdest.h;
        overall_rectdest.h -= rectdest.h;
    }
}

/*----------------------------------------------------------------------------*/
void
GlSurface_copyRectSecure(GlSurface src, GlSurface dest, GlRect* srcrect, GlRect* destrect)
{
    GlRect rectsrc;
    GlRect rectdest;
    if (srcrect == NULL)
    {
        rectsrc = src->rct;
    }
    else
    {
        rectsrc = *srcrect;
        GlRect_clip(&rectsrc, &src->rct);
    }
    if (destrect == NULL)
    {
        rectdest = dest->rct;
    }
    else
    {
        rectdest = *destrect;
        GlRect_clip(&rectdest, &dest->rct);
    }

    rectsrc.w = rectdest.w = MIN(rectsrc.w, rectdest.w);
    rectsrc.h = rectdest.h = MIN(rectsrc.h, rectdest.h);

    GlSurface_copyRect(src, dest, &rectsrc, &rectdest);
}

/*----------------------------------------------------------------------------*/
void
GlSurface_movePart(GlSurface surf, GlRect rct, Gl2DCoord dx, Gl2DCoord dy)
{
    Bool reverse;
    GlRect destrct;
    GlIterator itsrc;
    GlIterator itdest;
    
    ASSERT(rct.x >= 0, return);
    ASSERT(rct.y >= 0, return);
    ASSERT(rct.x + rct.w <= surf->rct.w, return);
    ASSERT(rct.y + rct.h <= surf->rct.h, return);
    ASSERT(rct.x + dx >= 0, return);
    ASSERT(rct.y + dy >= 0, return);
    ASSERT(rct.x + rct.w + dx <= surf->rct.w, return);
    ASSERT(rct.y + rct.h + dy <= surf->rct.h, return);
    
    destrct = rct;
    destrct.x += dx;
    destrct.y += dy;

    if (dy > 0)
    {
        reverse = TRUE;
    }
    else if (dy < 0)
    {
        reverse = FALSE;
    }
    else    /*dy == 0*/
    {
        if (dx == 0)
        {
            return;
        }
        reverse = (dx > 0);
    }

    GlIterator_init(&itsrc, surf);
    GlIterator_init(&itdest, surf);
    GlIterator_setClipRect(&itsrc, &rct, (reverse) ? ((dy == 0) ? CORNER_BOTTOMRIGHT : CORNER_BOTTOMLEFT) : CORNER_TOPLEFT);
    GlIterator_setClipRect(&itdest, &destrct, (reverse) ? ((dy == 0) ? CORNER_BOTTOMRIGHT : CORNER_BOTTOMLEFT) : CORNER_TOPLEFT);

    if (reverse)
    {
        if (dy == 0)
        {
            while (!GlIterator_endReached(itsrc))
            {
                *(itdest.pos) = *(itsrc.pos);
                GlIterator_bwdCoupled(&itsrc, &itdest);
            }
        }
        else
        {
            while (!GlIterator_endReached(itsrc))
            {
                memCOPY(itdest.pos, itsrc.pos, GlIterator_getLineSize(itsrc));
                GlIterator_upCoupled(&itsrc, &itdest);
            }
        }
    }
    else
    {
        if (dy == 0)
        {
            while (!GlIterator_endReached(itsrc))
            {
                *(itdest.pos) = *(itsrc.pos);
                GlIterator_fwdCoupled(&itsrc, &itdest);
            }
        }
        else
        {
            while (!GlIterator_endReached(itsrc))
            {
                memCOPY(itdest.pos, itsrc.pos, GlIterator_getLineSize(itsrc));
                GlIterator_downCoupled(&itsrc, &itdest);
            }
        }
    }
}
