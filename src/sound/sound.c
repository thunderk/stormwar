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

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "sound/sound.h"
#include "core/core.h"
#include "core/string.h"
#include "tools/varvalidator.h"
#include "tools/fonct.h"

#ifdef USE_SOUND
#include "SDL.h"
#include "SDL_mixer.h"
#include "core/ptrarray.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    String name;
    Mix_Chunk* sample;
} pv_SoundNamedSample;

typedef pv_SoundNamedSample* SoundNamedSample;

typedef enum
{
    CHANNEL_FREE,       /*not reserved, not playing*/
    CHANNEL_BUSY,       /*not reserved, playing*/
    CHANNEL_RESERVED    /*reserved, playing or not*/
} ChannelState;

/******************************************************************************
 *                               Static variables                             *
 ******************************************************************************/
static Mix_Music* music;
static Bool music_playing;
static Bool music_paused;
static PtrArray samples;
static SoundCallback* callbacks;
static ChannelState* channels;
static int nbchannels;

static Sound3DCoord camposx;
static Sound3DCoord camposy;
static Sound3DCoord camposz;
static Sound3DCoord camangh;
static Sound3DCoord camangv;

#endif  /*USE_SOUND*/
static Uint16 music_fadein;
static Uint16 music_fadeout;

static Bool nosound = FALSE;

static CoreID SND_ID = 0;
static CoreID MUS_ID = 0;
static CoreID FUNCMUS_SETFADEIN = 0;
static CoreID FUNCMUS_SETFADEOUT = 0;
static CoreID FUNCMUS_PLAY = 0;
static CoreID FUNCMUS_STOP = 0;
static CoreID FUNCMUS_PAUSE = 0;
static CoreID FUNCMUS_RESUME = 0;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
#ifdef USE_SOUND
/*----------------------------------------------------------------------------*/
static void
Sample_del(SoundNamedSample sample)
{
    Mix_FreeChunk(debugFREE(sample->sample));
    String_del(sample->name);
    FREE(sample);
}

/*----------------------------------------------------------------------------*/
static int
Sample_cmp(SoundNamedSample* sample1, SoundNamedSample* sample2)
{
    return String_cmp(&((*sample1)->name), &((*sample2)->name));
}

/*----------------------------------------------------------------------------*/
static void
channelFinished(int channel)
{
    ASSERT(channel >= 0, return);
    ASSERT(channel < nbchannels, return);
    ASSERT(channels[channel] != CHANNEL_FREE, return);

    if ((channels[channel] == CHANNEL_RESERVED) && (callbacks[channel] != NULL))
    {
        callbacks[channel](channel, SOUNDEVENT_SAMPLEEND);
    }
    else if (channels[channel] == CHANNEL_BUSY)
    {
        channels[channel] = CHANNEL_FREE;
    }
}

/*----------------------------------------------------------------------------*/
static void
soundAddSample(Var v)
{
    VarValidator valid;
    SoundNamedSample s;
    String st;

    s = MALLOC(sizeof(pv_SoundNamedSample));

    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "name", "");
    VarValidator_declareStringVar(valid, "file", "");
    VarValidator_validate(valid, v);
    VarValidator_del(valid);

    st = String_newByCopy(Var_getValueString(Var_getArrayElemByCName(v, "file")));
    coreFindData(st);
    s->sample = Mix_LoadWAV(String_get(st));
    String_del(st);
    if (s->sample == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Sound sample loading error from %s:", String_get(Var_getValueString(Var_getArrayElemByCName(v, "file"))));
        shellPrintf(LEVEL_ERROR, "   %s", Mix_GetError());
        FREE(s);
        return;
    }
    else
    {
        (void)debugALLOC(s->sample, 0);
    }
    s->name = String_newByCopy(Var_getValueString(Var_getArrayElemByCName(v, "name")));

    PtrArray_insertSorted(samples, s);
}
#endif

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNCMUS_SETFADEIN)
    {
        music_fadein = Var_getValueInt(func->params[0]);
    }
    else if (func->id == FUNCMUS_SETFADEOUT)
    {
        music_fadeout = Var_getValueInt(func->params[0]);
    }
    else if (func->id == FUNCMUS_PLAY)
    {
        soundMusicPlay(TRUE);
    }
    else if (func->id == FUNCMUS_STOP)
    {
        soundMusicStop(TRUE);
    }
    else if (func->id == FUNCMUS_PAUSE)
    {
        soundMusicPause();
    }
    else if (func->id == FUNCMUS_RESUME)
    {
        soundMusicResume();
    }
    Var_setVoid(func->ret);
}

/*----------------------------------------------------------------------------*/
static void
musicDatasCallback(Var musicvar)
{
#ifdef USE_SOUND
    VarValidator valid;
    String s;

    if (nosound)
    {
        return;
    }

    if (music_playing)
    {
        soundMusicStop(FALSE);
    }

    valid = VarValidator_new();
    VarValidator_declareStringVar(valid, "file", "");
    VarValidator_declareIntVar(valid, "fadein", 0);
    VarValidator_declareIntVar(valid, "fadeout", 0);
    VarValidator_validate(valid, musicvar);
    VarValidator_del(valid);

    /*load*/
    s = String_newByCopy(Var_getValueString(Var_getArrayElemByCName(musicvar, "file")));
    coreFindData(s);
    if (music != NULL)
    {
        Mix_FreeMusic(debugFREE(music));
        music = NULL;
    }
    music = debugALLOC(Mix_LoadMUS(String_get(s)), 0);
    if (music == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Unable to load music file: %s", String_get(s));
    }
    String_del(s);

    music_fadein = Var_getValueInt(Var_getArrayElemByCName(musicvar, "fadein"));
    music_fadeout = Var_getValueInt(Var_getArrayElemByCName(musicvar, "fadeout"));
#else
    (void)musicvar;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
static void
soundDatasCallback(Var v)
{
#ifdef USE_SOUND
    if (!nosound)
    {
        unsigned int i;

        PtrArray_clear(samples);

        for (i = 0; i < Var_getArraySize(v); i++)
        {
            soundAddSample(Var_getArrayElemByPos(v, i));
        }
    }
#else
    (void)v;
#endif  /*USE_SOUND*/
}

/******************************************************************************
 *############################################################################*
 *#                           Sound engine functions                         #*
 *############################################################################*
 ******************************************************************************/
void
soundInit()
{
#ifdef USE_SOUND
    SDL_version compile_version;
    const SDL_version *link_version;
    int frequency, nchannels;
    Uint16 format;

    if (!nosound)
    {
        /*TODO: try different parameters*/
        if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
        {
            shellPrintf(LEVEL_ERROR, "Failed to initialized sound device: %s", Mix_GetError());
            nosound = TRUE;
        }
        else
        {
            if (Mix_QuerySpec(&frequency, &format, &nchannels) != 1)
            {
                shellPrintf(LEVEL_ERROR, "Failed to initialized sound engine: %s", Mix_GetError());
                nosound = TRUE;
            }
            else
            {
                char* s = "Unknown";
                switch (format)
                {
                    case AUDIO_U8:      s = "U8"; break;
                    case AUDIO_S8:      s = "S8"; break;
                    case AUDIO_U16LSB:  s = "U16LSB"; break;
                    case AUDIO_S16LSB:  s = "S16LSB"; break;
                    case AUDIO_U16MSB:  s = "U16MSB"; break;
                    case AUDIO_S16MSB:  s = "S16MSB"; break;
                }
                shellPrintf(LEVEL_INFO, "Audio parameters: %dHz, %d channels, %s format", frequency, nchannels, s);

                samples = PtrArray_newFull(10, 5, (PtrFunc)Sample_del, (PtrCmpFunc)Sample_cmp);
                nbchannels = 1;
                channels = MALLOC(sizeof(ChannelState));
                channels[0] = CHANNEL_FREE;
                callbacks = MALLOC(sizeof(SoundCallback));
                callbacks[0] = NULL;

                Mix_ChannelFinished(channelFinished);

                soundSetMaxChannels(16);
            }

            music = NULL;
            music_fadein = 0;
            music_fadeout = 0;
            music_playing = FALSE;
            music_paused = FALSE;
        }
    }
#endif  /*USE_SOUND*/

    /*declare to core*/
    MUS_ID = coreDeclareModule("music", NULL, musicDatasCallback, shellCallback, NULL, NULL, NULL);
    SND_ID = coreDeclareModule("sounds", NULL, soundDatasCallback, NULL, NULL, NULL, NULL);
    FUNCMUS_SETFADEIN = coreDeclareShellFunction(MUS_ID, "setfadein", VAR_VOID, 1, VAR_INT);
    FUNCMUS_SETFADEOUT = coreDeclareShellFunction(MUS_ID, "setfadeout", VAR_VOID, 1, VAR_INT);
    FUNCMUS_PLAY = coreDeclareShellFunction(MUS_ID, "play", VAR_VOID, 0);
    FUNCMUS_STOP = coreDeclareShellFunction(MUS_ID, "stop", VAR_VOID, 0);
    FUNCMUS_PAUSE = coreDeclareShellFunction(MUS_ID, "pause", VAR_VOID, 0);
    FUNCMUS_RESUME = coreDeclareShellFunction(MUS_ID, "resume", VAR_VOID, 0);

    /*information*/
#ifdef USE_SOUND
    if (!nosound)
    {
        MIX_VERSION(&compile_version);
        shellPrintf(LEVEL_INFO, " -> compiled with SDL_mixer version: %d.%d.%d",
            compile_version.major,
            compile_version.minor,
            compile_version.patch);
        link_version=Mix_Linked_Version();
        shellPrintf(LEVEL_INFO, " -> running with SDL_mixer version:  %d.%d.%d",
            link_version->major,
            link_version->minor,
            link_version->patch);
    }
#else
    shellPrintf(LEVEL_INFO, " -> Sound disabled.");
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundUninit()
{
#ifdef USE_SOUND
    if (!nosound)
    {
        if (music_playing)
        {
            soundMusicStop(FALSE);
        }
        if (music)
        {
            Mix_FreeMusic(debugFREE(music));
            music = NULL;
        }
        soundStopAll();
        FREE(channels);
        FREE(callbacks);
        PtrArray_del(samples);
        Mix_CloseAudio();
    }
#endif  /*USE_SOUND*/
    shellPrint(LEVEL_INFO, "Sound engine destroyed.");
}

/*----------------------------------------------------------------------------*/
void
soundDisable()
{
    nosound = TRUE;
}

/*----------------------------------------------------------------------------*/
void
soundRestore()
{
    /*TODO: all this causes a segfault on win32*/
#if 0
    int frequency, nchannels;
    Uint16 format;

    if (nosound)
    {
        return;
    }

    Mix_CloseAudio();
    if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
    {
        shellPrintf(LEVEL_ERROR, "Failed to initialized sound engine: %s", Mix_GetError());
    }

    if (Mix_QuerySpec(&frequency, &format, &nchannels) != 1)
    {
        shellPrintf(LEVEL_ERROR, "Failed to initialized sound engine: %s", Mix_GetError());
        nosound = TRUE;
    }
    else
    {
        char* s = "Unknown";
        switch (format)
        {
            case AUDIO_U8:      s = "U8"; break;
            case AUDIO_S8:      s = "S8"; break;
            case AUDIO_U16LSB:  s = "U16LSB"; break;
            case AUDIO_S16LSB:  s = "S16LSB"; break;
            case AUDIO_U16MSB:  s = "U16MSB"; break;
            case AUDIO_S16MSB:  s = "S16MSB"; break;
        }
        shellPrintf(LEVEL_INFO, "Audio parameters: %dHz, %d channels, %s format", frequency, nchannels, s);
    }

    /*restore the channels and raise monitors*/
    soundSetMaxChannels(nbchannels);

    Mix_ChannelFinished(channelFinished);

    /*TODO: maybe check if the music was unloaded from the lib.*/
    if (music_playing)
    {
        Mix_PlayMusic(music, -1);
        if (music_paused)
        {
            Mix_PauseMusic();
        }
    }
#endif
    (void)0;
}

/*----------------------------------------------------------------------------*/
void
soundMusicPlay(Bool fade)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    if (music_paused)
    {
        soundMusicResume();
    }
    else if ((music != NULL) && (!music_playing))
    {
        if (fade && (music_fadein != 0))
        {
            if (Mix_FadeInMusic(music, -1, music_fadein) == 0)
            {
                music_playing = TRUE;
            }
        }
        else
        {
            if (Mix_PlayMusic(music, -1) == 0)
            {
                music_playing = TRUE;
            }
        }
    }
#else
    (void)fade;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundMusicStop(Bool fade)
{
#ifdef USE_SOUND
    if ((!nosound) && (music != NULL) && (music_playing))
    {
        if (fade && (music_fadeout != 0) && (!music_paused))
        {
            while(!Mix_FadeOutMusic(music_fadeout) && Mix_PlayingMusic())
            {
                SDL_Delay(100);
            }
        }
        else
        {
            Mix_HaltMusic();
        }
        music_playing = FALSE;
        music_paused = FALSE;
    }
#else
    (void)fade;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundMusicPause()
{
#ifdef USE_SOUND
    if ((!nosound) && (music != NULL) && (music_playing) && (!music_paused))
    {
        Mix_PauseMusic();
        music_paused = TRUE;
    }
#else
    (void)0;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundMusicResume()
{
#ifdef USE_SOUND
    if ((!nosound) && (music != NULL) && (music_playing) && (music_paused))
    {
        Mix_ResumeMusic();
        music_paused = FALSE;
    }
#else
    (void)0;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundSetMaxChannels(int _nbchannels)
{
#ifdef USE_SOUND
    SoundCallback cb;

    if (nosound)
    {
        return;
    }

    if (_nbchannels <= 0)
    {
        shellPrint(LEVEL_ERROR, "Tried to allocate 0 sound channel, ignored.");
        return;
    }
    if (nbchannels != _nbchannels)
    {
        int i;

        for (i = _nbchannels; i < nbchannels; i++)
        {
            /*stop reserved channels*/
            if (channels[i] == CHANNEL_RESERVED)
            {
                Mix_HaltChannel(i);
                channels[i] = CHANNEL_FREE;
                if ((cb = callbacks[i]) != NULL)
                {
                    callbacks[i] = NULL;
                    cb(i, SOUNDEVENT_RELEASE);
                }
            }
        }
        Mix_AllocateChannels(_nbchannels);
        channels = REALLOC(channels, sizeof(ChannelState) * _nbchannels);
        callbacks = REALLOC(callbacks, sizeof(SoundCallback) * _nbchannels);
        for (i = nbchannels; i < _nbchannels; i++)
        {
            callbacks[i] = NULL;
        }
        for (i = nbchannels; i < _nbchannels; i++)
        {
            /*set new channels' state*/
            channels[i] = CHANNEL_FREE;
        }
        nbchannels = _nbchannels;
    }
#else
    (void)_nbchannels;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
SoundChannel
soundReserveChannel()
{
#ifdef USE_SOUND
    int i;

    if (nosound)
    {
        return - 1;
    }

    /*TODO: optimize this*/

    /*search a free channel*/
    i = 0;
    while ((i < nbchannels) && (channels[i] != CHANNEL_FREE))
    {
        i++;
    }
    if (i == nbchannels)
    {
        /*no free channel found, try to steal a common playing channel*/
        i = 0;
        while ((i < nbchannels) && (channels[i] != CHANNEL_BUSY))
        {
            i++;
        }
        if (i == nbchannels)
        {
            /*all channels are reserved*/
            return -1;
        }
        else
        {
            i--;
            /*stop a playing common channel and steal it*/
            Mix_HaltChannel(i);
            ASSERT(channels[i] == CHANNEL_FREE, return -1);
            channels[i] = CHANNEL_RESERVED;
            return i;
        }
    }
    else
    {
        channels[i] = CHANNEL_RESERVED;
        return i;
    }
#else
    return -1;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundReleaseChannel(SoundChannel channel)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);

    if (channel >= 0)
    {
        ASSERT(channels[channel] == CHANNEL_RESERVED, return);

        Mix_HaltChannel(channel);
        channels[channel] = CHANNEL_FREE;
    }
#else
    (void)channel;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundReleaseAllChannels()
{
#ifdef USE_SOUND
    int i;
    SoundCallback cb;

    if (nosound)
    {
        return;
    }

    /*free reserved channels*/
    for (i = 0; i < nbchannels; i++)
    {
        if (channels[i] == CHANNEL_RESERVED)
        {
            Mix_HaltChannel(i);
            channels[i] = CHANNEL_FREE;
            if ((cb = callbacks[i]) != NULL)
            {
                callbacks[i] = NULL;
                cb(i, SOUNDEVENT_RELEASE);
            }
        }
    }
#else
    (void)0;
#endif  /*USE_SOUND*/
}

/*----------------------------------------------------------------------------*/
void
soundSetCamera(Sound3DCoord posx, Sound3DCoord posy, Sound3DCoord posz, Sound3DCoord angh, Sound3DCoord angv)
{
#ifdef USE_SOUND
    camposx = posx;
    camposy = posy;
    camposz = posz;
    camangh = angh;
    camangv = angv;
#else
    (void)posx;
    (void)posy;
    (void)posz;
    (void)angh;
    (void)angv;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundSetCallback(SoundChannel channel, SoundCallback cb)
{
#ifdef USE_SOUND
    if ((!nosound) && (channel >= 0))
    {
        ASSERT(channel < nbchannels, return);
        ASSERT(cb != NULL, return);
        ASSERT(channels[channel] == CHANNEL_RESERVED, return);
        ASSERT(callbacks[channel] == NULL, return);

        callbacks[channel] = cb;
    }
#else
    (void)channel;
    (void)cb;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundUnsetCallback(SoundChannel channel)
{
#ifdef USE_SOUND
    if (!nosound)
    {
        ASSERT(channel < nbchannels, return);
        ASSERT(channels[channel] == CHANNEL_RESERVED, return);
        ASSERT(callbacks[channel] != NULL, return);

        callbacks[channel] = NULL;
    }
#else
    (void)channel;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundStopChannel(SoundChannel channel)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);
    if (channel >= 0)
    {
        Mix_HaltChannel(channel);
        /*SDL_mixer callbacks will change the channel state and throw events*/
    }
#else
    (void)channel;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundFadeChannel(SoundChannel channel, unsigned int fadetime)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);
    if (channel >= 0)
    {
        Mix_FadeOutChannel(channel, fadetime);
        /*SDL_mixer callbacks will change the channel state and throw events*/
    }
#else
    (void)channel;
    (void)fadetime;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundStopAll()
{
#ifdef USE_SOUND
    int i;

    if (nosound)
    {
        return;
    }

    for (i = 0; i < nbchannels; i++)
    {
        Mix_HaltChannel(i);
        /*SDL_mixer callbacks will change the channel state and throw events*/
    }
#else
    (void)0;
#endif
}

/*----------------------------------------------------------------------------*/
SoundSample
soundGetSample(String name)
{
#ifdef USE_SOUND
    PtrArrayIterator it;
    pv_SoundNamedSample s;

    if (nosound)
    {
        return SoundSample_NULL;
    }

    s.name = name;

    it = PtrArray_findSorted(samples, &s);
    if (it == NULL)
    {
        return SoundSample_NULL;
    }
    else
    {
        return ((SoundNamedSample)(*it))->sample;
    }
#else
    (void)name;
    return NULL;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundSetSampleVolume(SoundSample sample, SoundVolume volume)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    Mix_VolumeChunk((Mix_Chunk*)sample, volume);
#else
    (void)sample;
    (void)volume;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundSetChannelVolume(SoundChannel channel, SoundVolume volume)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);
    if (channel >= 0)
    {
        ASSERT(channels[channel] == CHANNEL_RESERVED, return);
        Mix_Volume(channel, volume);
    }
#else
    (void)channel;
    (void)volume;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundSetChannel3DPos(SoundChannel channel, Sound3DCoord posx, Sound3DCoord posy, Sound3DCoord posz)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);
    if (channel >= 0)
    {
        float angh, angv, dist;

        ASSERT(channels[channel] == CHANNEL_RESERVED, return);
        Mix_UnregisterAllEffects(channel);
        angle3d(posx - camposx, posy - camposy, posz - camposz, &angh, &angv);
        dist = dist3d(posx, posy, posz, camposx, camposy, camposz);
        dist = MIN(dist, 255.0);
        Mix_SetPosition(channel, (Sint16)(RAD2DEG(camangh - angh)), (Uint8)dist);
    }
#else
    (void)channel;
    (void)posx;
    (void)posy;
    (void)posz;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundSetChannel2DPos(SoundChannel channel, SoundStereo stereo, SoundDistance distance)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);
    if (channel >= 0)
    {
        ASSERT(channels[channel] == CHANNEL_RESERVED, return);
        Mix_UnregisterAllEffects(channel);
        Mix_SetPanning(channel, 255 - stereo, stereo);
        Mix_SetDistance(channel, distance);
    }
#else
    (void)channel;
    (void)stereo;
    (void)distance;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundPlaySampleOnChannel(SoundSample sample, SoundChannel channel, int nbloops)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    ASSERT(channel < nbchannels, return);

    if ((channel >= 0) && (nbloops != 0))
    {
        ASSERT(channels[channel] == CHANNEL_RESERVED, return);
        if (nbloops < -1)
        {
            nbloops = -1;
        }
        else
        {
            nbloops--;
            /*because for SDL_Mixer, 1 means 2 loops*/
        }
        Mix_PlayChannel(channel, (Mix_Chunk*)sample, nbloops);
    }
#else
    (void)sample;
    (void)channel;
    (void)nbloops;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundPlaySample2D(SoundSample sample, int nbloops, SoundVolume volume, SoundStereo stereo, SoundDistance distance)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    if (nbloops > 0)
    {
        /*find a free channel*/
        int i = 0;
        while ((i < nbchannels) && (channels[i] != CHANNEL_FREE))
        {
            i++;
        }
        if (i != nbchannels)
        {
            channels[i] = CHANNEL_BUSY;

            /*set its parameters*/
            Mix_UnregisterAllEffects(i);
            Mix_SetPanning(i, 255 - stereo, stereo);
            Mix_SetDistance(i, distance);
            Mix_Volume(i, volume);

            /*play the sample on it*/
            Mix_PlayChannel(i, (Mix_Chunk*)sample, nbloops - 1);
        }
    }
#else
    (void)sample;
    (void)nbloops;
    (void)volume;
    (void)stereo;
    (void)distance;
#endif
}

/*----------------------------------------------------------------------------*/
void
soundPlaySample3D(SoundSample sample, int nbloops, SoundVolume volume, Sound3DCoord posx, Sound3DCoord posy, Sound3DCoord posz)
{
#ifdef USE_SOUND
    if (nosound)
    {
        return;
    }

    if (nbloops > 0)
    {
        float angh, angv, dist;

        /*find a free channel*/
        int i = 0;
        while ((i < nbchannels) && (channels[i] != CHANNEL_FREE))
        {
            i++;
        }
        if (i != nbchannels)
        {
            channels[i] = CHANNEL_BUSY;

            /*set its parameters*/
            Mix_UnregisterAllEffects(i);
            angle3d(posx - camposx, posy - camposy, posz - camposz, &angh, &angv);
            dist = dist3d(posx, posy, posz, camposx, camposy, camposz);
            dist = MIN(dist, 255.0);
            Mix_SetPosition(i, (Sint16)(RAD2DEG(camangh - angh)), (Uint8)dist);
            Mix_Volume(i, volume);

            /*play the sample on it*/
            Mix_PlayChannel(i, (Mix_Chunk*)sample, nbloops - 1);
        }
    }
#else
    (void)sample;
    (void)nbloops;
    (void)volume;
    (void)posx;
    (void)posy;
    (void)posz;
#endif
}

/*----------------------------------------------------------------------------*/
SoundVolume
soundVolume(Uint8 vol)
{
#ifdef USE_SOUND
    return ((Uint32)vol * MIX_MAX_VOLUME / 255);
#else
    (void)vol;
    return 0;
#endif
}
