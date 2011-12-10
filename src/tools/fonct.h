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

#ifndef _SW_FONCT_H_
#define _SW_FONCT_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include <math.h>

#include "main.h"
#include "SDL.h"

/******************************************************************************
 *                                 Constants                                  *
 ******************************************************************************/
#ifndef M_E
    #define M_E		2.7182818284590452354	/* e */
#endif
#ifndef M_LOG2E
    #define M_LOG2E	1.4426950408889634074	/* log_2 e */
#endif
#ifndef M_LOG10E
    #define M_LOG10E	0.43429448190325182765	/* log_10 e */
#endif
#ifndef M_LN2
    #define M_LN2		0.69314718055994530942	/* log_e 2 */
#endif
#ifndef M_LN10
    #define M_LN10		2.30258509299404568402	/* log_e 10 */
#endif
#ifndef M_PI
    #define M_PI		3.14159265358979323846	/* pi */
#endif
#ifndef M_2PI
    #define M_2PI		6.28318530717958647692	/* 2*pi */
#endif
#ifndef M_PI_2
    #define M_PI_2		1.57079632679489661923	/* pi/2 */
#endif
#ifndef M_PI_4
    #define M_PI_4		0.78539816339744830962	/* pi/4 */
#endif
#ifndef M_1_PI
    #define M_1_PI		0.31830988618379067154	/* 1/pi */
#endif
#ifndef M_2_PI
    #define M_2_PI		0.63661977236758134308	/* 2/pi */
#endif
#ifndef M_2_SQRTPI
    #define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
#endif
#ifndef M_SQRT2
    #define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
#endif
#ifndef M_SQRT1_2
    #define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
#endif

/******************************************************************************
 *                                   Macros                                   *
 ******************************************************************************/
#undef MAX
#undef MIN
#undef SGN
#undef ABS
#undef CARRE
/*!
 * \brief Macro that gives the maximum of two scalar values.
 *
 * Don't use complex expressions as parameters (they will be copied several times).
 * \param x - First value
 * \param y - Second value
 * \return The greatest value.
 */
#define MAX(x,y) (((x)>(y))?(x):(y))
/*!
 * \brief Macro that gives the minimum of two scalar values.
 *
 * Don't use complex expressions as parameters (they will be copied several times).
 * \param x - First value
 * \param y - Second value
 * \return The least value.
 */
#define MIN(x,y) (((x)<(y))?(x):(y))
/*!
 * \brief Macro that returns the sign of a scalar.
 *
 * Don't use complex expressions as parameters (they will be copied several times).
 * \param x - Value
 * \return 1 if x is positive, -1 if x is negative, 0 if x is zero.
 */
#define SGN(x) (((x)>0)?1:(((x)==0)?0:-1))
/*!
 * \brief Macro that returns absolute value of a scalar.
 *
 * Don't use complex expressions as parameters (they will be copied several times).
 * \param x - Value
 * \return The absolute value.
 */
#define ABS(x) (((x)>=0)?(x):(-(x)))
/*!
 * \brief Macro that returns the square of a scalar.
 *
 * Don't use complex expressions as parameters (they will be copied several times).
 * \param x - Value
 * \return Square of the value.
 */
#define CARRE(x) ((x)*(x))

/*!
 * \brief Macro to convert a radian angle into a degree angle.
 *
 * \param x - Radian value
 * \return Degree value
 */
#define RAD2DEG(x) ((x)*57.295779515)

/*!
 * \brief Macro to convert a degree angle into a radian angle.
 *
 * \param x - Degree value
 * \return Radian value
 */
#define DEG2RAD(x) ((x)*0.017453293)

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/*! \brief Absolute value of an integer */
int     iabs(int val);

/*! \brief Square of a number */
float carre(float x);

/*! \brief 2D distance between two points
 *
 *  \param x1 - x coord of the first point
 *  \param y1 - y coord of the first point
 *  \param x2 - x coord of the second point
 *  \param y2 - y coord of the second point
 *  \return     Distance between the two points
 */
float dist2d(float x1, float y1, float x2, float y2);

/*! \brief 2D distance between two points
 *
 *  \param x1 - x coord of the first point
 *  \param y1 - y coord of the first point
 *  \param z1 - z coord of the first point
 *  \param x2 - x coord of the second point
 *  \param y2 - y coord of the second point
 *  \param z2 - z coord of the second point
 *  \return     Distance between the two points
 */
float dist3d(float x1, float y1, float z1, float x2, float y2, float z2);

/*! \brief Linear interpolation between corners of a square
 *
 *  Length of the square's edges is supposed to be 1.
 *  So coords x and y must be given between 0 and 1.
 *  Interpolation will be done linearly between the corners.
 *  \param v00 - Value at top-left corner
 *  \param v10 - Value at top-right corner
 *  \param v01 - Value at bottom-left corner
 *  \param v11 - Value at bottom-right corner
 *  \param x   - Horizontal position inside the square
 *  \param y   - Vertical position inside the square
 *  \return    - Interpolated value
 */
float interpol_lin(float v00, float v10, float v01, float v11, float x, float y);

/*! \brief Maximal value between two integers */
int     imax(int a, int b);

/*! \brief Minimal value between two intergers */
int     imin(int a, int b);

/*! \brief Maximal value between two floats */
float   flmax(float a, float b);

/*! \brief Minimal value between two floats */
float   flmin(float a, float b);

/*! \brief Maximal value between two double floats */
double  dmax(double a, double b);

/*! \brief Minimal value between two double floats */
double  dmin(double a, double b);

/*! \brief Do a modulo range
 *
 *  Final value will be in [base..base+mod[ .
 *  \param nb   - Number that will be modulated
 *  \param base - Base (start) value of modulo interval
 *  \param mod  - Length of modulo interval
 *  \return       Number modulated
 */
float modulo(float nb, float base, float mod);

/*! \brief Find the angle value done by a point
 *
 *  This function finds the angle formed by the vector (x,y)
 *  and the reference vector (1,0)
 *  \param x - Horizontal coordinate of the vector (point)
 *  \param y - Vertical coordinate of the vector (point)
 */
float angle2d(float x, float y);

/*!
 * \brief Find the 3d angle done by a point
 *
 * This function finds the angle formed by the vector (x,y,z)
 * and the reference vector (1,0,0)
 * \param x - X coordinate of the vector (point)
 * \param y - Y coordinate of the vector (point)
 * \param z - Z coordinate of the vector (point)
 * \param ah - Returned horizontal angle.
 * \param av - Returned vertical angle.
 */
void    angle3d(float x, float y, float z, float* ah, float* av);

/*! \brief Converts an integer in a formatted string
 *
 *  Formatted string will be completed with as many '0' as required
 *  \param num    - Number to be formatted
 *  \param st     - Buffer to fill with the formatted string
 *  \param nbchar - Size of the buffer
 */
void    format0(int num, char *st, int nbchar);

/*! \brief Initialize the random number generator as best as it can */
void    rnd_init();

/*! \brief Generate a pseudo-random integer
 *
 *  \param base  - Starting value of random interval
 *  \param range - Length of random interval (number of possible values)
 *  \return        An integer between base and base+range-1
 */
int     rnd(int base, int range);

/*!
 * \brief Get the number of milliseconds since the program began.
 *
 * \return (Uint32) Time in milliseconds, relative to the program lifetime.
 */
#define getTicks() SDL_GetTicks()

#define delay SDL_Delay

/*!
 * \brief Compare correctly 2 floats.
 *
 * This will handle the float approximation.
 * \param f1 - First float.
 * \param f2 - Second float.
 * \return (f1 == f2).
 */
Bool    floatEqual(float f1, float f2);

/*!
 * \brief Compare correctly 2 doubles.
 *
 * This will handle the double approximation.
 * \param d1 - First double.
 * \param d2 - Second double.
 * \return (d1 == d2).
 */
Bool    doubleEqual(double d1, double d2);

#ifdef HAVE_ROUND
#define floatToInt(_f_) ((int)round(_f_))
#else
#define floatToInt(_f_) ((int)_f_)
#endif

#endif
