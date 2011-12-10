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
 *   Functions to make tileable textures from pictures                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "tools/internal/texturizer.h"
#include "tools/fonct.h"
#include "graphics/glsurface.h"
#include "graphics/color.h"

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
texturize(String src, String dest)
{
    GlSurface surf;
    GlSurface surfdisp;
    GlSurface surfnew;
    Uint16 x, y;
    GlColor col;
    float r, g, b, a;
    float inf, infx, infy, infxy;
    Uint16 w, h;

    surf = GlSurface_newFromFile(src);
    w = GlSurface_getWidth(surf);
    h = GlSurface_getHeight(surf);
    surfdisp = GlSurface_new(w, h, FALSE);
    surfnew = GlSurface_new(w, h, FALSE);

    /*create the displacement surface*/
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            if (x < w / 2)
            {
                infx = 1.0 - ((float)x * 2.0 / (float)w);
            }
            else
            {
                infx = ((float)x * 2.0 / (float)w) - 1.0;
            }

            if (y < h / 2)
            {
                infy = 1.0 - ((float)y * 2.0 / (float)h);
            }
            else
            {
                infy = ((float)y * 2.0 / (float)h) - 1.0;
            }

            infxy = MIN(infx, infy);

            col = GlSurface_getColor(surf, x, y);
            r = (float)GlColor_GETR(col);
            g = (float)GlColor_GETG(col);
            b = (float)GlColor_GETB(col);
            a = (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, w - 1 - x, y);
            r += infx * (float)GlColor_GETR(col);
            g += infx * (float)GlColor_GETG(col);
            b += infx * (float)GlColor_GETB(col);
            a += infx * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, x, h - 1 - y);
            r += infy * (float)GlColor_GETR(col);
            g += infy * (float)GlColor_GETG(col);
            b += infy * (float)GlColor_GETB(col);
            a += infy * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, w - 1 - x, h - 1 - y);
            r += infxy * (float)GlColor_GETR(col);
            g += infxy * (float)GlColor_GETG(col);
            b += infxy * (float)GlColor_GETB(col);
            a += infxy * (float)GlColor_GETA(col);

            inf = 1.0 + infx + infy + infxy;
            GlColor_SETR(col, (Uint8)(r / inf));
            GlColor_SETG(col, (Uint8)(g / inf));
            GlColor_SETB(col, (Uint8)(b / inf));
            GlColor_SETA(col, (Uint8)(a / inf));
            GlSurface_drawPixel(surfdisp, x, y, col);
        }
    }

    /*create the final surface*/
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            if (x < w / 2)
            {
                infx = 1.0 - ((float)x * 2.0 / (float)w);
            }
            else
            {
                infx = ((float)x * 2.0 / (float)w) - 1.0;
            }

            if (y < h / 2)
            {
                infy = 1.0 - ((float)y * 2.0 / (float)h);
            }
            else
            {
                infy = ((float)y * 2.0 / (float)h) - 1.0;
            }

            inf = MAX(infx, infy);

            col = GlSurface_getColor(surf, x, y);
            r = (1.0 - inf) * (float)GlColor_GETR(col);
            g = (1.0 - inf) * (float)GlColor_GETG(col);
            b = (1.0 - inf) * (float)GlColor_GETB(col);
            a = (1.0 - inf) * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surfdisp, (x + w / 2) % w, (y + h / 2) % h);
            r += inf * (float)GlColor_GETR(col);
            g += inf * (float)GlColor_GETG(col);
            b += inf * (float)GlColor_GETB(col);
            a += inf * (float)GlColor_GETA(col);

            GlColor_SETR(col, (Uint8)r);
            GlColor_SETG(col, (Uint8)g);
            GlColor_SETB(col, (Uint8)b);
            GlColor_SETA(col, (Uint8)a);
            GlSurface_drawPixel(surf, x, y, col);
        }
    }

    GlSurface_saveToPNM(surfnew, dest);

    GlSurface_del(surf);
    GlSurface_del(surfnew);
    GlSurface_del(surfdisp);
}

/*----------------------------------------------------------------------------*/
void
texturesym(String src, String dest)
{
    GlSurface surf;
    GlSurface surfnew;
    Uint16 x, y;
    GlColor col;
    float r, g, b, a;
    Uint16 w, h;

    surf = GlSurface_newFromFile(src);
    w = GlSurface_getWidth(surf);
    h = GlSurface_getHeight(surf);
    surfnew = GlSurface_new(w, h, FALSE);

    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            col = GlSurface_getColor(surf, x, y);
            r = 0.125 * (float)GlColor_GETR(col);
            g = 0.125 * (float)GlColor_GETG(col);
            b = 0.125 * (float)GlColor_GETB(col);
            a = 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, w - 1 - x, y);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, x, h - 1 - y);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, w - 1 - x, h - 1 - y);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, y, x);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, y, w - 1 - x);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, h - 1 - y, x);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            col = GlSurface_getColor(surf, h - 1 - y, w - 1 - x);
            r += 0.125 * (float)GlColor_GETR(col);
            g += 0.125 * (float)GlColor_GETG(col);
            b += 0.125 * (float)GlColor_GETB(col);
            a += 0.125 * (float)GlColor_GETA(col);

            GlColor_SETR(col, (Uint8)r);
            GlColor_SETG(col, (Uint8)g);
            GlColor_SETB(col, (Uint8)b);
            GlColor_SETA(col, (Uint8)a);
            GlSurface_drawPixel(surf, x, y, col);
        }
    }

    GlSurface_saveToPNM(surfnew, dest);

    GlSurface_del(surf);
    GlSurface_del(surfnew);
}
