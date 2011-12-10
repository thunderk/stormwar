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

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "tools/anim.h"

#include "tools/varvalidator.h"
#include "tools/fonct.h"
#include "core/string.h"
#include "core/var.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
typedef struct
{
    CoreTime time;
    union
    {
        Int i;
        Float f;
    } value;
} Frame;

typedef struct
{
    Bool integer;       /*TRUE: integer track, FALSE: float track*/
    Uint16 nbframes;
    Frame* frames;
} Track;

struct pv_Anim
{
    String name;
    AnimTrack nbtracks;
    Track* tracks;
    CoreTime lastframe;
};

typedef struct
{
    Bool integer;       /*TRUE: integer track, FALSE: float track*/
    Track* link;        /*link, NULL if linking failed*/
    void* p;            /*value pointer, either (Int*) or (Float*), NULL if not set yet */
} TrackControl;

struct pv_AnimControl
{
    AnimTrack nbtracks;
    TrackControl* tracks;
};

/******************************************************************************
 *############################################################################*
 *#                             Debug functions                              #*
 *############################################################################*
 ******************************************************************************/
#ifdef DEBUG_ANIM
static void
Anim_debug(Anim anim)
{
    int i, j;
    printf("DEBUG Anim %p.\n", (void*)anim);
    for (i = 0; i < anim->nbtracks; i++)
    {
        printf("Track %d :", i);
        for (j = 0; j < anim->tracks[i].nbframes; j++)
        {
            if (anim->tracks[i].integer)
            {
                printf(" %d(%d)", anim->tracks[i].frames[j].time, anim->tracks[i].frames[j].value.i);
            }
            else
            {
                printf(" %d(%f)", anim->tracks[i].frames[j].time, anim->tracks[i].frames[j].value.f);
            }
        }
        printf("\n");
    }
}

/*----------------------------------------------------------------------------*/
static void
AnimControl_debug(AnimControl control)
{
    int i;
    printf("DEBUG AnimControl %p.\n", (void*)control);
    for (i = 0; i < control->nbtracks; i++)
    {
        if (control->tracks[i].link == NULL)
        {
            printf("Track %d not linked.\n", i);
        }
    }
}
#endif

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
Anim
Anim_new(String name, AnimTrack nbtracks)
{
    Anim ret;
    AnimTrack i;
    
    ret = MALLOC(sizeof(pv_Anim));
    if (name == NULL)
    {
        ret->name = String_new(NULL);
    }
    else
    {
        ret->name = String_newByCopy(name);
    }
    ret->nbtracks = nbtracks;
    ret->lastframe = 0;
    if (nbtracks != 0)
    {
        ret->tracks = MALLOC(sizeof(Track) * nbtracks);
        for (i = 0; i < nbtracks; i++)
        {
            ret->tracks[i].integer = TRUE;
            ret->tracks[i].nbframes = 0;
        }
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
Anim_del(Anim anim)
{
    AnimTrack i;
    
    if (anim->nbtracks != 0)
    {
        for (i = 0; i < anim->nbtracks; i++)
        {
            if (anim->tracks[i].nbframes != 0)
            {
                FREE(anim->tracks[i].frames);
            }
        }
        FREE(anim->tracks);
    }
    String_del(anim->name);
    FREE(anim);
}

/*----------------------------------------------------------------------------*/
int
Anim_cmp(Anim* a1, Anim* a2)
{
    return String_cmp(&(*a1)->name, &(*a2)->name);
}

/*----------------------------------------------------------------------------*/
void
Anim_setTrackFromVar(Anim anim, AnimTrack track, Var v)
{
    VarArrayPos i;
    Var ve;
    CoreTime time;
    
    if (Var_getType(v) != VAR_ARRAY)
    {
        /*TODO: error*/
    }
    else
    {
        for (i = 0; i < Var_getArraySize(v) - 1; i += 2)
        {
            ve = Var_getArrayElemByPos(v, i);
            if (Var_getType(ve) != VAR_INT)
            {
                /*TODO: error*/
                continue;
            }
            time = Var_getValueInt(ve);
            ve = Var_getArrayElemByPos(v, i + 1);
            if (Var_getType(ve) == VAR_INT)
            {
                Anim_addIntFrame(anim, time, track, Var_getValueInt(ve));
            }
            else if (Var_getType(ve) == VAR_FLOAT)
            {
                Anim_addFloatFrame(anim, time, track, Var_getValueFloat(ve));
            }
            else
            {
                /*TODO: error*/
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void
Anim_addIntFrame(Anim anim, CoreTime time, AnimTrack tracknb, Int value)
{
    if (tracknb < anim->nbtracks)
    {
        Track* track = anim->tracks + tracknb;
        if (track->nbframes == 0)
        {
            /*the track becomes an integer one*/
            track->integer = TRUE;
            track->frames = MALLOC(sizeof(Frame));
            track->nbframes++;
            track->frames[0].time = time;
            track->frames[0].value.i = value;
            if (time > anim->lastframe)
            {
                anim->lastframe = time;
            }
        }
        else
        {
            if (!track->integer)
            {
                /*the track is already a float one*/
                Anim_addFloatFrame(anim, time, tracknb, (Float)value);
            }
            else
            {
                Uint16 i, j;
                
                i = 0;
                while ((i < track->nbframes) && (track->frames[i].time < time))
                {
                    i++;
                }
                if ((i < track->nbframes) && (track->frames[i].time == time))
                {
                    /*replace existing value*/
                    track->frames[i].value.i = value;
                }
                else
                {
                    /*add the frame*/
                    track->frames = REALLOC(track->frames, sizeof(Frame) * (track->nbframes + 1));
                    for (j = track->nbframes; j > i; j--)
                    {
                        track->frames[j] = track->frames[j - 1];
                    }
                    track->frames[i].time = time;
                    track->frames[i].value.i = value;
                    track->nbframes++;
                    if (time > anim->lastframe)
                    {
                        anim->lastframe = time;
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
void
Anim_addFloatFrame(Anim anim, CoreTime time, AnimTrack tracknb, Float value)
{
    if (tracknb < anim->nbtracks)
    {
        Track* track = anim->tracks + tracknb;
        if (track->nbframes == 0)
        {
            /*the track becomes a float one*/
            track->integer = FALSE;
            track->frames = MALLOC(sizeof(Frame));
            track->nbframes++;
            track->frames[0].time = time;
            track->frames[0].value.f = value;
            if (time > anim->lastframe)
            {
                anim->lastframe = time;
            }
        }
        else
        {
            if (track->integer)
            {
                /*the track is already an integer one*/
                Anim_addIntFrame(anim, time, tracknb, (Int)value);
            }
            else
            {
                Uint16 i, j;
                
                i = 0;
                while ((i < track->nbframes) && (track->frames[i].time < time))
                {
                    i++;
                }
                if ((i < track->nbframes) && (track->frames[i].time == time))
                {
                    /*replace existing value*/
                    track->frames[i].value.f = value;
                }
                else
                {
                    /*add the frame*/
                    track->frames = REALLOC(track->frames, sizeof(Frame) * (track->nbframes + 1));
                    for (j = track->nbframes; j > i; j--)
                    {
                        track->frames[j] = track->frames[j - 1];
                    }
                    track->frames[i].time = time;
                    track->frames[i].value.f = value;
                    track->nbframes++;
                    if (time > anim->lastframe)
                    {
                        anim->lastframe = time;
                    }
                }
            }
        }
    }
}

/*----------------------------------------------------------------------------*/
AnimControl
AnimControl_new(AnimTrack nbtracks)
{
    AnimControl ret;
    AnimTrack i;
    
    ret = MALLOC(sizeof(pv_AnimControl));
    ret->nbtracks = nbtracks;
    if (nbtracks != 0)
    {
        ret->tracks = MALLOC(sizeof(TrackControl) * nbtracks);
        for (i = 0; i < nbtracks; i++)
        {
            /*ret->tracks[i].integer = TRUE;*/
            ret->tracks[i].link = NULL;
            ret->tracks[i].p = NULL;
        }
    }
    
    return ret;
}

/*----------------------------------------------------------------------------*/
void
AnimControl_del(AnimControl control)
{
    if (control->nbtracks != 0)
    {
        FREE(control->tracks);
    }
    FREE(control);
}

/*----------------------------------------------------------------------------*/
void
AnimControl_setIntControl(AnimControl control, AnimTrack track, Int* i)
{
    if (track < control->nbtracks)
    {
        control->tracks[track].integer = TRUE;
        control->tracks[track].link = NULL;
        control->tracks[track].p = (void*)i;
    }
}

/*----------------------------------------------------------------------------*/
void
AnimControl_setFloatControl(AnimControl control, AnimTrack track, Float* f)
{
    if (track < control->nbtracks)
    {
        control->tracks[track].integer = FALSE;
        control->tracks[track].link = NULL;
        control->tracks[track].p = (void*)f;
    }
}

/*----------------------------------------------------------------------------*/
CoreTime
AnimControl_linkToAnim(AnimControl control, Anim anim)
{
    AnimTrack i, n;
    
    if (anim == NULL)
    {
        for (i = 0; i < control->nbtracks; i++)
        {
            control->tracks[i].link = NULL;
        }
        return 0;
    }
    
#ifdef DEBUG_ANIM
    Anim_debug(anim);
#endif
    
    n = MIN(control->nbtracks, anim->nbtracks);
    /*create links*/
    for (i = 0; i < n; i++)
    {
        if ((control->tracks[i].integer == anim->tracks[i].integer)
         && (anim->tracks[i].nbframes != 0)
         && (control->tracks[i].p != NULL))
        {
            control->tracks[i].link = anim->tracks + i;
        }
        else
        {
            control->tracks[i].link = NULL;
        }
    }

#ifdef DEBUG_ANIM
    AnimControl_debug(control);
#endif
    
    return anim->lastframe;
}

/*----------------------------------------------------------------------------*/
void
AnimControl_update(AnimControl control, CoreTime time)
{
    AnimTrack i;
    Track* link;
    Frame* frame;
    Uint16 j;
    
    for (i = 0; i < control->nbtracks; i++)
    {
        link = control->tracks[i].link;
        if (link != NULL)
        {
            /*the track is linked, search for the value*/
            frame = link->frames;
            if (frame->time >= time)
            {
                /*required a value before (or at) the first frame*/
                if (link->integer)
                {
                    *((Int*)control->tracks[i].p) = frame->value.i;
                }
                else
                {
                    *((Float*)control->tracks[i].p) = frame->value.f;
                }
            }
            else
            {
                /*searching*/
                j = 0;
                while ((j < (link->nbframes - 1)) && ((frame + 1)->time < time))
                {
                    j++;
                    frame++;
                }
                
                if (j == link->nbframes - 1)
                {
                    /*the time is after the last frame.*/
                    if (link->integer)
                    {
                        *((Int*)control->tracks[i].p) = frame->value.i;
                    }
                    else
                    {
                        *((Float*)control->tracks[i].p) = frame->value.f;
                    }
                }
                else if ((frame + 1)->time == time)
                {
                    /*we found a frame at the right time*/
                    if (link->integer)
                    {
                        *((Int*)control->tracks[i].p) = (frame + 1)->value.i;
                    }
                    else
                    {
                        *((Float*)control->tracks[i].p) = (frame + 1)->value.f;
                    }
                }
                else
                {
                    /*we are between two frames, so interpolate*/
                    if (link->integer)
                    {
                        *((Int*)control->tracks[i].p) = frame->value.i + ((frame + 1)->value.i - frame->value.i) * (Int)(time - frame->time) / (Int)((frame + 1)->time - frame->time);
                    }
                    else
                    {
                        *((Float*)control->tracks[i].p) = frame->value.f + ((frame + 1)->value.f - frame->value.f) * (Float)(time - frame->time) / (Float)((frame + 1)->time - frame->time);
                    }
                }
            }
        }
    }
}
