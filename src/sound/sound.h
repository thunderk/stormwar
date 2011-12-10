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
 *   Engine to manage music and sounds                                        *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief This is the sound engine to mix music and samples.
 *
 * The sound engine allows to easily mix a music with sound samples.
 * This engine is based on sound channels. A channel is a sound allocation
 * that can play only one sound at a time.<br>
 * There is one channel always reserved for the music. This channel is controled
 * by dedicated functions.<br>
 * There are several sample channels (their number can be changed).
 * Sample channels allow to mix sound samples together. These sound samples are loaded in the mod datas.
 * Channels can be 'reserved' (when not reserved, they are called 'common'). A reserved channel allows
 * to keep track of the sound playing on it and to have a better control over it than a common channel.<br>
 * When no reserved channel is specified to play a sample, the first free common channel will be used
 * (reserved channels are exclusive for their owner).
 */

#ifndef _SW_SOUND_H_
#define _SW_SOUND_H_ 1

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
/*#include "graphics/glcoord.h"*/

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief A reserved sound channel. */
typedef int SoundChannel;

/*! \brief A sound sample. */
typedef void* SoundSample;

/*! \brief Volume for a channel or a sample. */
typedef int SoundVolume;

/*! \brief Stereo parameter (0=left, 254=right, 127=center) */
typedef Uint8 SoundStereo;

/*! \brief Sound's distance to the camera. */
typedef Uint8 SoundDistance;

/*! \brief 3D coordinate. */
typedef float Sound3DCoord;

/*! \brief Events that can occur on a reserved channel. */
typedef enum
{
    SOUNDEVENT_SAMPLEEND,       /*!< The sample ended playing. Can occur even when a sound is started over a playing one, cutting it. */
    SOUNDEVENT_RELEASE          /*!< The channel was released. If a sound was playing, it has been stopped. A callback is automatically destroyed on a release. */
} SoundEvent;

/*! \brief Monitor to watch over external channel releasing. */
typedef void (*SoundCallback)(SoundChannel channel, SoundEvent event);

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
/*! \brief A default void but usable sound sample. */
#define SoundSample_NULL NULL

#define SoundStereo_LEFT 0
#define SoundStereo_RIGHT 254
#define SoundStereo_CENTER 127

#define SoundDistance_NEAR 0
#define SoundDistance_FAR 255

/******************************************************************************
 *############################################################################*
 *#                             Engine functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Initialize the sound engine.
 */
void soundInit(void);

/*!
 * \brief Uninitialize the sound engine.
 */
void soundUninit(void);

/*!
 * \brief Prevent sound init.
 *
 * This must be called before soundInit() call.
 */
void soundDisable(void);

/*!
 * \brief Restore sound engine (call this if the sound has been altered).
 *
 * WARNING: this will release ALL reserved channels and thus send SOUNDEVENT_RELEASE by callbacks.
 */
void soundRestore(void);

/*!
 * \brief Start playing the loaded music.
 *
 * \param fade - Use the music's fadein time.
 */
void soundMusicPlay(Bool fade);

/*!
 * \brief Stop playing the loaded music.
 *
 * \param fade - Use the music's fadeout time.
 */
void soundMusicStop(Bool fade);

/*!
 * \brief Pause the currently playing music.
 */
void soundMusicPause(void);

/*!
 * \brief Resume the paused music.
 */
void soundMusicResume(void);

/*!
 * \brief Set the maximal number of mixing channels that can be allocated.
 *
 * This take count of 'reserved' and 'common' channels.
 * WARNING: this might release some reserved channels and thus send SOUNDEVENT_RELEASE by callbacks.
 * \param nbchannels - Maximal number of allocated channels.
 */
void soundSetMaxChannels(int nbchannels);

/*!
 * \brief Reserve a channel for exclusive use.
 *
 * \return The reserved channel, a default valid but unplayed channel in case of failure.
 */
SoundChannel soundReserveChannel(void);

/*!
 * \brief Release a reserved channel.
 *
 * This will stop the sample playing on this channel if any.
 * As it is a voluntary release, this will NOT send SOUNDEVENT_RELEASE to the callback.
 * \return channel - A reserved channel.
 */
void soundReleaseChannel(SoundChannel channel);

/*!
 * \brief Release all reserved channels.
 *
 * This will send a SOUNDEVENT_RELEASE to all set callbacks.
 */
void soundReleaseAllChannels(void);

/*!
 * \brief Set the graphical camera location.
 *
 * \param posx - Camera's X position.
 * \param posy - Camera's Y position.
 * \param posz - Camera's Z position.
 * \param angh - Horizontal angle.
 * \param angv - Vertical angle.
 */
void soundSetCamera(Sound3DCoord posx, Sound3DCoord posy, Sound3DCoord posz, Sound3DCoord angh, Sound3DCoord angv);

/*!
 * \brief Set a callback to catch channel events.
 *
 * A channel can only have one callback set.
 * \param channel - The channel to watch.
 * \param cb - The callback called when an event occurs.
 */
void soundSetCallback(SoundChannel channel, SoundCallback cb);

/*!
 * \brief Remove a callback from a channel.
 *
 * \param channel - The channel to stop watching.
 */
void soundUnsetCallback(SoundChannel channel);

/*!
 * \brief Stop the sample playing on a reserved channel.
 *
 * This will NOT release the channel.
 * A SOUNDEVENT_SAMPLEEND will be sent to the callback.
 * \param channel - A reserved channel.
 */
void soundStopChannel(SoundChannel channel);

/*!
 * \brief Fade out then stop the sample playing on a reserved channel.
 *
 * This will NOT release the channel.
 * A SOUNDEVENT_SAMPLEEND will be sent to the callback at the end of the fading effect.
 * \param channel - A reserved channel.
 * \param fadetime - Time in milliseconds that should last the fadeout effect.
 */
void soundFadeChannel(SoundChannel channel, unsigned int fadetime);

/*!
 * \brief Stop all channels.
 *
 * This will stop the samples playing on every channels (reserved or common).
 * Reserved channels are NOT released by this.
 * SOUNDEVENT_SAMPLEEND are sent.
 */
void soundStopAll(void);

/*!
 * \brief Find a sample in the samples set by its name.
 *
 * \param name - Searched sample's name.
 * \return The found sample or SoundSample_NULL if not found.
 */
SoundSample soundGetSample(String name);

/*!
 * \brief Set the volume of a sample.
 *
 * This will discard the volume specified in the samples set.
 * TODO: applied on playing samples or not?
 * \param sample - The sound sample.
 * \param volume - Sample's new volume.
 */
void soundSetSampleVolume(SoundSample sample, SoundVolume volume);

/*!
 * \brief Set the volume of a reserved channel.
 *
 * TODO: applied on playing samples or not?
 * \param channel - A reserved channel.
 * \param volume - Channel's new volume.
 */
void soundSetChannelVolume(SoundChannel channel, SoundVolume volume);

/*!
 * \brief Set the spatial positioning of a reserved channel.
 *
 * TODO: applied in realtime?
 * The sound output position and volume will be adjusted, given the graphical camera position.
 * This will cancel the 2D positioning on this channel if any.
 * 3D spatialization already causes a volume attenuation with the distance to the camera (but no time offset).
 * \param channel - A reserved channel.
 * \param posx - Sound's X position.
 * \param posy - Sound's Y position.
 * \param posz - Sound's Z position.
 */
void soundSetChannel3DPos(SoundChannel channel, Sound3DCoord posx, Sound3DCoord posy, Sound3DCoord posz);

/*!
 * \brief Set the stereo positioning on a reserved channel.
 *
 * TODO: applied in realtime?
 * This will cancel the 3D positioning on this channel, if any.
 * \param channel - A reserved channel.
 * \param stereo - Stereo parameter.
 * \param distance -
 */
void soundSetChannel2DPos(SoundChannel channel, SoundStereo stereo, SoundDistance distance);

/*!
 * \brief Play a sample on a reserved channel.
 *
 * If a sound was already playing on this channel, it will be replaced.
 * \param sample - The sound sample.
 * \param channel - The reserved channel to play on.
 * \param nbloops - 1 to play once, n>1 to play n times, n<=0 to play indefinitely.
 */
void soundPlaySampleOnChannel(SoundSample sample, SoundChannel channel, int nbloops);

/*!
 * \brief Play a 2D-localized sample on common channels.
 *
 * The sample will be played accordind to the number of free common channels.
 * If no channels are available, the sample will not be played.
 * \param sample - The sound sample.
 * \param nbloops - 1 to play once, n>1 to play n times, n<=0 will not play.
 * \param volume - Playing volume.
 * \param stereo - Stereo parameter.
 * \param distance - Sound distance.
 */
void soundPlaySample2D(SoundSample sample, int nbloops, SoundVolume volume, SoundStereo stereo, SoundDistance distance);

/*!
 * \brief Play a 3D-localized sample on common channels.
 *
 * The sample will be played accordind to the number of free common channels.
 * If no channels are available, the sample will not be played.
 * \param sample - The sound sample.
 * \param nbloops - 1 to play once, n>1 to play n times, n<=0 will not play.
 * \param volume - Playing volume.
 * \param posx - Sound's X location.
 * \param posy - Sound's Y location.
 * \param posz - Sound's Z location.
 */
void soundPlaySample3D(SoundSample sample, int nbloops, SoundVolume volume, Sound3DCoord posx, Sound3DCoord posy, Sound3DCoord posz);

/*!
 * \brief Convert a numerical volume to a usable SoundVolume.
 *
 * \param vol - The numerical volume [0-255]. 0 is mute, 255 is loudest.
 * \return A usable volume.
 */
SoundVolume soundVolume(Uint8 vol);

#endif
