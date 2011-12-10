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
 *   Useful misc functions                                                    *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "fonct.h"
#include "main.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <float.h>  /*TODO: check*/

#ifdef HAVE_TIME_H
#include <time.h>
#endif

/******************************************************************************
 *############################################################################*
 *#                                Functions                                 #*
 *############################################################################*
 ******************************************************************************/
int
iabs(int val)
{
    return ABS(val);
}

/*----------------------------------------------------------------------------*/
float
carre(float x)
{
    return CARRE(x);
}

/*----------------------------------------------------------------------------*/
float
dist2d(float x1, float y1, float x2, float y2)
{
    return sqrt(carre(x2 - x1) + carre(y2 - y1));
}

/*----------------------------------------------------------------------------*/
float
dist3d(float x1, float y1, float z1, float x2, float y2, float z2)
{
    return sqrt(carre(x2 - x1) + carre(y2 - y1) + carre(z2 - z1));
}

/*----------------------------------------------------------------------------*/
float
interpol_lin(float v00, float v10, float v01, float v11, float x,
             float y)
{
    float a, b;

    a = v00 + (v01 - v00) * y;
    b = v10 + (v11 - v10) * y;
    return a + (b - a) * x;
}

/*----------------------------------------------------------------------------*/
int
imax(int a, int b)
{
    return MAX(a, b);
}

/*----------------------------------------------------------------------------*/
int
imin(int a, int b)
{
    return MIN(a, b);
}

/*----------------------------------------------------------------------------*/
float
flmax(float a, float b)
{
    return MAX(a, b);
}

/*----------------------------------------------------------------------------*/
float
flmin(float a, float b)
{
    return MIN(a, b);
}

/*----------------------------------------------------------------------------*/
double
dmax(double a, double b)
{
    return MAX(a, b);
}

/*----------------------------------------------------------------------------*/
double
dmin(double a, double b)
{
    return MIN(a, b);
}

/*----------------------------------------------------------------------------*/
float
modulo(float nb, float base, float mod)
{
    float temp = nb;

    while (temp - base >= mod)
        temp -= mod;
    while (temp < base)
        temp += mod;
    return temp;
}

/*----------------------------------------------------------------------------*/
float
angle2d(float x, float y)
{
    float nx, ny, d, ret;

    if (floatEqual(x, 0.0))
    {
        if (floatEqual(y, 0.0))
        {
            return 0.0;
        }
        else
        {
            if (y < 0.0)
            {
                return 3 * M_PI_2;
            }
            else
            {
                return M_PI_2;
            }
        }
    }

    d = dist2d(0.0, 0.0, x, y);
    nx = x / d;
    ny = y / d;


    ret = asin(ny);
    if (nx < 0.0)
    {
        ret = M_PI - ret;
    }
    return modulo(ret, 0.0, 2.0 * M_PI);
}

/*----------------------------------------------------------------------------*/
void
angle3d(float x, float y, float z, float* ah, float* av)
{
    *ah = angle2d(x, z);
    *av = angle2d(dist2d(0.0, 0.0, x, z), y);
}

/*----------------------------------------------------------------------------*/
void
format0(int num, char *st, int nbchar)
{
    int n = num;
    int i;

    for (i = 0; i < nbchar; i++)
    {
        st[i] = 48 + (int) ((float) n / pow(10, nbchar - i - 1));
        n = n - (st[i] - 48) * (int) pow(10, nbchar - i - 1);
    }
}

/*----------------------------------------------------------------------------*/
void
rnd_init()
{
#ifdef HAVE_TIME_H
    srand((unsigned int) time(NULL));
#else
    int i;

    /*we spend some time doing a very interesting empty loop*/
    for (i = 0; i < 10000000; i++);
    srand((unsigned int) SDL_GetTicks());
#endif
}

/*----------------------------------------------------------------------------*/
int
rnd(int base, int range)
{
    if (range == 0)
    {
        return base;
    }
    return base + (rand() % range);
}

/*----------------------------------------------------------------------------*/
Bool
floatEqual(float f1, float f2)
{
    float f = f2 - f1;
    return (f <= FLT_EPSILON) & (f >= -FLT_EPSILON);
}

/*----------------------------------------------------------------------------*/
Bool
doubleEqual(double d1, double d2)
{
    double d = d2 - d1;
    return (d <= DBL_EPSILON) & (d >= -DBL_EPSILON);
}
