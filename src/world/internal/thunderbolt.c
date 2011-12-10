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
 *   Thunderbolts (global lighting, lightning effect and thunder SFX).        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "world/internal/thunderbolt.h"

#include "world/internal/skybox.h"

#include "core/core.h"

#include "tools/fonct.h"
#include "tools/varvalidator.h"
#include "sound/sound.h"

#include "graphics/graphics.h"
#include "graphics/light.h"

/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/
#define MAX_BOLTS 5

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef enum
{
    BOLT_ONE = 0,
    BOLT_TWOQUICK = 1,
    BOLT_TWO = 2,
    BOLT_TWO_AND_ONE = 3
} BoltType;

typedef struct
{
    BoltType type;
    int step;               /* -2=SFX, -1=waiting, 0=lightDel, 1=first bolt, 2=first bolt end, 3=second bolt... */
    Light li;
    Uint32 thundertime;     /*time in milliseconds before thunder comes*/
    SoundDistance dist;     /*sound distance*/
    Gl3DCoord posx;
    Gl3DCoord posy;
    Gl3DCoord posz;
} Thunderbolt;

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Gl3DCoord worldwidth;
static Gl3DCoord worldheight;
static Thunderbolt bolts[MAX_BOLTS];
static Uint16 nbbolts;
static Uint16 rnd_factor;
static SoundChannel sndchan;
static SoundSample sndsample;

/*current thunder sound*/
static Bool curplaying;
static Uint8 curdist;

/*core things*/
static CoreID MOD_ID = CORE_INVALID_ID;
static Bool _pause = FALSE;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
soundCallback(SoundChannel channel, SoundEvent event)
{
    ASSERT(channel == sndchan, return);
    (void)channel;

    if (event == SOUNDEVENT_RELEASE)
    {
        sndchan = soundReserveChannel();
        soundSetCallback(sndchan, soundCallback);
    }
    else if (event == SOUNDEVENT_SAMPLEEND)
    {
        ASSERT(curplaying == TRUE, return);
        curplaying = FALSE;
    }
}

/*----------------------------------------------------------------------------*/
static void
threadCallback(CoreID thread, CoreTime steptime)
{
    Bool l;
    unsigned int i;
    float angv, angh;
    GlColorRGB col;

    (void)thread;
    if (_pause)
    {
        return;
    }

    /*generate new bolt?*/
    if ((nbbolts < MAX_BOLTS) && (rnd(0, rnd_factor) == 0))
    {
        bolts[nbbolts].type = (BoltType)rnd(0, 4);
        bolts[nbbolts].step = 1;
        bolts[nbbolts].li = lightAdd(FALSE);
        bolts[nbbolts].dist = rnd(5, 251);
        col.r = col.g = col.b = 255 - bolts[nbbolts].dist;
        lightSetColor(bolts[nbbolts].li, col, col, col);
        angh = ((float)rnd(0, 500)) * M_2PI / 500.0;
        angv = ((float)rnd(0, 200)) * M_PI / 200.0 - M_PI_2;
        lightSetPos(bolts[nbbolts].li, 100.0 * cos(angh) * cos(angv), -100.0 * sin(angv), -100.0 * sin(angh) * cos(angv));
        lightSetDirection(bolts[nbbolts].li, angh, angv);
        bolts[nbbolts].posx = cos(angh) * cos(angv);
        bolts[nbbolts].posy = sin(angv);
        bolts[nbbolts].posz = sin(angh) * cos(angv);
        lightToggle(bolts[nbbolts].li, TRUE);
        bolts[nbbolts].thundertime = bolts[nbbolts].dist * 10;
        nbbolts++;
    }

    l = FALSE;
    for (i = 0; i < nbbolts; i++)
    {
        switch (bolts[i].step)
        {
            case -2:
                if ((!curplaying) || (bolts[i].dist < curdist + 20))
                {
                    soundSetChannel3DPos(sndchan, bolts[i].posx * (Gl3DCoord)bolts[i].dist, bolts[i].posy * (Gl3DCoord)bolts[i].dist, bolts[i].posz * (Gl3DCoord)bolts[i].dist);
                    soundPlaySampleOnChannel(sndsample, sndchan, 1);
                    curplaying = TRUE;
                    curdist = bolts[i].dist;
                }
                if (--nbbolts != 0)
                {
                    bolts[i] = bolts[nbbolts];
                }
                break;
            case -1:
                if (bolts[i].thundertime <= steptime)
                {
                    bolts[i].step = -2;
                }
                else
                {
                    bolts[i].thundertime -= steptime;
                }
                break;
            case 0:
                lightDel(bolts[i].li);
                bolts[i].step = -1;
                break;
            case 1:
                lightToggle(bolts[i].li, TRUE);
                l = TRUE;
                bolts[i].step++;
                break;
            case 2:
                lightToggle(bolts[i].li, FALSE);
                l = FALSE;
                if (bolts[i].type == BOLT_ONE)
                {
                    bolts[i].step = 0;
                }
                else
                {
                    bolts[i].step++;
                }
                break;
            case 3:
                if (bolts[i].type != BOLT_TWO)
                {
                    lightToggle(bolts[i].li, TRUE);
                    l = TRUE;
                }
                else
                {
                    lightToggle(bolts[i].li, FALSE);
                    l = FALSE;
                }
                bolts[i].step++;
                break;
            case 4:
                lightToggle(bolts[i].li, FALSE);
                l = FALSE;
                if (bolts[i].type == BOLT_TWOQUICK)
                {
                    bolts[i].step = 0;
                }
                else
                {
                    bolts[i].step++;
                }
                break;
            case 5:
                if (bolts[i].type == BOLT_TWO)
                {
                    lightToggle(bolts[i].li, TRUE);
                    l = TRUE;
                }
                bolts[i].step++;
                break;
            case 6:
                lightToggle(bolts[i].li, FALSE);
                l = FALSE;
                if (bolts[i].type == BOLT_TWO)
                {
                    bolts[i].step = 0;
                }
                else
                {
                    bolts[i].step++;
                }
                break;
            case 7:
                lightToggle(bolts[i].li, TRUE);
                l = TRUE;
                bolts[i].step++;
                break;
            case 8:
                lightToggle(bolts[i].li, FALSE);
                l = FALSE;
                bolts[i].step = 0;
                break;
        }
    }
    
    skyboxSetLightning(l);
}

/*----------------------------------------------------------------------------*/
static void
coreCallback(CoreEvent event)
{
    if (event == CORE_PAUSE)
    {
        _pause = TRUE;
    }
    else if (event == CORE_RESUME)
    {
        _pause = FALSE;
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
thunderboltInit()
{
    nbbolts = 0;
    worldwidth = 10.0;
    worldheight = 10.0;
    rnd_factor = 100;

    curplaying = FALSE;
    _pause = FALSE;

    sndchan = soundReserveChannel();
    soundSetCallback(sndchan, soundCallback);
    sndsample = SoundSample_NULL;

    MOD_ID = coreDeclareModule("thunder", coreCallback, NULL, NULL, NULL, NULL, threadCallback);
    coreRequireThreadSlot(MOD_ID, coreGetThreadID(NULL));
}

/*----------------------------------------------------------------------------*/
void
thunderboltUninit()
{
    soundUnsetCallback(sndchan);
    soundReleaseChannel(sndchan);

    /*TODO: delete lights*/
    shellPrint(LEVEL_INFO, "Thunderbolts module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
thunderboltSet(Var vbolt)
{
    VarValidator valid;

    valid = VarValidator_new();
    VarValidator_declareIntVar(valid, "frequency", 880);
    VarValidator_declareStringVar(valid, "sound", "");
    VarValidator_validate(valid, vbolt);
    VarValidator_del(valid);

    rnd_factor = Var_getValueInt(Var_getArrayElemByCName(vbolt, "frequency"));
    rnd_factor = MAX(rnd_factor, 0);
    rnd_factor = 1000 - MIN(rnd_factor, 1000);
    sndsample = soundGetSample(Var_getValueString(Var_getArrayElemByCName(vbolt, "sound")));
}

/*----------------------------------------------------------------------------*/
void
thunderboltWorldSizeChanged(WorldCoord w, WorldCoord h)
{
    /*TODO: clear bolts?*/
    worldwidth = (Gl3DCoord)w;
    worldheight = (Gl3DCoord)h;
}
