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
 *   Facility for animation by keyframes                                      *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_TOOLS_ANIM_H_
#define _SW_TOOLS_ANIM_H_ 1

/*!
 * \file
 * \brief Facility for animation by keyframes.
 *
 * \todo Detailed doc.
 * \todo More dynamic linking (an animation can change even if controllers are linked to it).
 * \todo Optimization: Precalc missing track value to search only once the frame position (avoid searching
 *          for all tracks).
 * \todo Optimization: Use better frame searching (dichotomic or even interpolation).
 * \todo Maybe use bresenham algorithms for integer tracks.
 * \todo Embedded time strategy.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Private structure for an Anim.
 */
typedef struct pv_Anim pv_Anim;

/*!
 * \brief Abstract type for animation datas.
 */
typedef pv_Anim* Anim;

/*!
 * \brief Private structure for an AnimControl.
 */
typedef struct pv_AnimControl pv_AnimControl;

/*!
 * \brief Abstract type for an animation controller.
 */
typedef pv_AnimControl* AnimControl;

/*!
 * \brief Animation track.
 */
typedef Uint16 AnimTrack;

/*!
 * \brief Enumeration of supported interpolation algorithms.
 */
typedef enum
{
    ANIM_LINEAR         /*!< Linear interpolation. */
} AnimInterpolation;

/******************************************************************************
 *############################################################################*
 *#                              Anim functions                              #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create an animation.
 *
 * \param name - Animation name, can ne NULL.
 * \param nbtracks - Number of tracks.
 * \return The newly allocated animation.
 */
Anim Anim_new(String name, AnimTrack nbtracks);

/*!
 * \brief Delete an animation.
 *
 * \param anim - The animation.
 */
void Anim_del(Anim anim);

/*!
 * \brief Comparison function.
 *
 * This function can be cast as \ref PtrCmpFunc.
 * \param a1 - First animation.
 * \param a2 - Second animation.
 * \return 0 if a1==a2, x>0 if a1>a2, x<0 if a1<a2.
 */
int Anim_cmp(Anim* a1, Anim* a2);

/*!
 * \brief Set an anim track from a variable.
 *
 * \param anim - The animation.
 * \param track - The track to set.
 * \param v - The definition variable.
 */
void Anim_setTrackFromVar(Anim anim, AnimTrack track, Var v);

/*!
 * \brief Add an integer frame value to an animation track.
 *
 * If the track has no frames yet, it will become an integer track. If the
 * track is already a float track, \a value will be converted.<br>
 * This value will replace any existing one.
 * \param anim - The animation.
 * \param time - The time position of the frame.
 * \param track - The track to set to set.
 * \param value - The value of the track 'track' at the time 'time'.
 */
void Anim_addIntFrame(Anim anim, CoreTime time, AnimTrack track, Int value);

/*!
 * \brief Add a float frame value to an animation track.
 *
 * If the track has no frames yet, it will become a float track. If the
 * track is already an integer track, \a value will be converted.
 * This value will replace any existing one.
 * \param anim - The animation.
 * \param time - The time position of the frame.
 * \param track - The track to set to set.
 * \param value - The value of the field 'field' at the time 'time'.
 */
void Anim_addFloatFrame(Anim anim, CoreTime time, AnimTrack track, Float value);

/******************************************************************************
 *############################################################################*
 *#                           AnimControl functions                          #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new animation controller.
 *
 * \param nbtracks - Number of tracks.
 * \return The newly allocated animation controller.
 */
AnimControl AnimControl_new(AnimTrack nbtracks);

/*!
 * \brief Delete an animation controller.
 *
 * \param control - The animation controller.
 */
void AnimControl_del(AnimControl control);

/*!
 * \brief Add an integer track control.
 *
 * This won't be validated until a call to AnimControl_linkToAnim.
 * \param control - The animation controller.
 * \param track - The track to control.
 * \param i - Pointer to an integer that will be filled with the track value at each \ref AnimControl_update call.
 */
void AnimControl_setIntControl(AnimControl control, AnimTrack track, Int* i);

/*!
 * \brief Add a float track control.
 *
 * This won't be validated until a call to AnimControl_linkToAnim.
 * \param control - The animation controller.
 * \param track - The track to control.
 * \param f - Pointer to a float that will be filled with the track value at each \ref AnimControl_update call.
 */
void AnimControl_setFloatControl(AnimControl control, AnimTrack track, Float* f);

/*!
 * \brief Link a controller to an animation.
 *
 * The track controls that don't match the animation signature (track number, type...) will be silently ignored.<br>
 * An anim must not change while there is a controller linked to it.
 * \param control - The animation controller.
 * \param anim - The animation to link the controller to, NULL to destroy any link.
 * \return Time of the last frame.
 */
CoreTime AnimControl_linkToAnim(AnimControl control, Anim anim);

/*!
 * \brief Update controls with track values.
 *
 * This will fill variables pointed by \ref AnimControl_setIntControl and \ref AnimControl_setFloatControl with
 * track values.
 * \param control - The animation controller.
 * \param time - Track values will be picked at this time inside the animation.
 */
void AnimControl_update(AnimControl control, CoreTime time);

#endif
