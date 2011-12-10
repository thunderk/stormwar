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
 *   Screen management                                                        *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "graphics/impl/glscreen.h"

#include "tools/fonct.h"
#include "graphics/graphics.h"
#include "core/core.h"
#include "SDL.h"
#include "graphics/impl/opengl.h"

/******************************************************************************
 *                              Global variables                              *
 ******************************************************************************/
static SDL_Surface* global_screen;

/******************************************************************************
 *                              Shared variables                              *
 ******************************************************************************/
Gl2DSize global_screenwidth;
Gl2DSize global_screenheight;

/******************************************************************************
 *                              Static variables                              *
 ******************************************************************************/
static Bool _fullscreen;

/* Core things */
static CoreID MOD_ID = 0;
static CoreID FUNC_VIDEOMODE = 0;

/******************************************************************************
 *############################################################################*
 *#                             Private functions                            #*
 *############################################################################*
 ******************************************************************************/
static void
shellCallback(ShellFunction* funct)
{
    if (funct->id == FUNC_VIDEOMODE)
    {
        graphicsSetVideoMode(Var_getValueInt(funct->params[0]), Var_getValueInt(funct->params[1]), Var_getValueInt(funct->params[2]));
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Screen functions                             #*
 *############################################################################*
 ******************************************************************************/
void
glscreenInit()
{
    char buf[50];
    SDL_VideoInfo* info;
    SDL_Rect** modes;
    
    _fullscreen = FALSE;
    global_screenwidth = 800;
    global_screenheight = 600;

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 0);
    
    /*global_screen = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL | SDL_RESIZABLE);*/
    glscreenSetVideo(&global_screenwidth, &global_screenheight, &_fullscreen);

    if(global_screen == NULL)
    {
        printf("%s\n", SDL_GetError());
        error("glscreen", "glscreenInit", "Can't initialize screen surface.");
    }
    
    /*declare to core*/
    MOD_ID = coreDeclareModule("screen", NULL, NULL, shellCallback, NULL, NULL, NULL);
    FUNC_VIDEOMODE = coreDeclareShellFunction(MOD_ID, "setvideomode", VAR_VOID, 3, VAR_INT, VAR_INT, VAR_INT);

    /*driver info*/
    if (SDL_VideoDriverName(buf, 50) != NULL)
    {
        shellPrintf(LEVEL_INFO, " -> SDL graphical driver:                         %s", buf);
    }
    
    /*video modes available*/
    modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_OPENGL);
    if (modes == (SDL_Rect**)0)
    {
        shellPrintf(LEVEL_ERROR, "No video modes listed, going on anyway.");
    }
    else if (modes != (SDL_Rect**)-1)
    {
        int i;
        Uint16 oldw = 0, oldh = 0;  /*for unique displaying*/
        
        shellPrint(LEVEL_INFO, " -> Video modes available in fullscreen:");
        for (i = 0; modes[i]; ++i)
        {
            if ((oldw != modes[i]->w) | (oldh != modes[i]->h))
            {
                oldw = modes[i]->w;
                oldh = modes[i]->h;
                shellPrintf(LEVEL_INFO, "     %d*%d", oldw, oldh);
            }
        }
    }
    
    /*video capabilities info*/
    if ((info = (SDL_VideoInfo*)SDL_GetVideoInfo()) != NULL)
    {
        shellPrintf(LEVEL_INFO, " -> Window manager available:                     %s", (info->wm_available) ? "yes" : "no");
        shellPrintf(LEVEL_INFO, " -> Hardware surfaces support:                    %s", (info->hw_available) ? "yes" : "no");
        if (info->hw_available)
        {
            shellPrintf(LEVEL_INFO, " -> Hardware blits accelerated:                   %s", (info->blit_hw) ? "yes" : "no");
            shellPrintf(LEVEL_INFO, " -> Hardware alpha blits accelerated:             %s", (info->blit_hw_A) ? "yes" : "no");
            shellPrintf(LEVEL_INFO, " -> Software to hardware blits accelerated:       %s", (info->blit_sw) ? "yes" : "no");
            shellPrintf(LEVEL_INFO, " -> Software to hardware alpha blits accelerated: %s", (info->blit_sw_A) ? "yes" : "no");
            shellPrintf(LEVEL_INFO, " -> Color fills accelerated:                      %s", (info->blit_fill) ? "yes" : "no");
            shellPrintf(LEVEL_INFO, " -> Video memory available in kB:                 %d", info->video_mem);
        }
    }
}

/*----------------------------------------------------------------------------*/
void
glscreenUninit()
{
    global_screen = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL);
    shellPrint(LEVEL_INFO, "Screen module unloaded.");
}

/*----------------------------------------------------------------------------*/
void
glscreenSetVideo(Gl2DSize* width, Gl2DSize* height, Bool* fullscreen)
{
    Uint32 flags = SDL_OPENGL | SDL_RESIZABLE;
    Bool er = FALSE;

    /*TODO: see about other than 32bpp*/
    /*TODO: check if a mode is available before switching to it*/
    
    /*we do the change in 3 times to be soft with the driver*/
    global_screen = SDL_SetVideoMode(global_screenwidth, global_screenheight, 32, flags);
    if(global_screen == NULL)
    {
        er = TRUE;
    }
    else
    {
        SDL_Delay(100);
        global_screen = SDL_SetVideoMode(*width, *height, 32, flags);
        if(global_screen == NULL)
        {
            er = TRUE;
        }
        else if (*fullscreen)
        {
            flags = SDL_OPENGL | SDL_FULLSCREEN;
            SDL_Delay(100);
            global_screen = SDL_SetVideoMode(*width, *height, 32, flags);
            if(global_screen == NULL)
            {
                er = TRUE;
            }
        }
    }
    
    if (er)
    {
        shellPrintf(LEVEL_ERROR, "Unable to initialize screen surface with this format: %d*%d %s.", *width, *height, (*fullscreen) ? "fullscreen" : "windowed");
        shellPrint(LEVEL_ERROR, "Trying with standard format: 800*600 windowed.");
        *width = 800;
        *height = 600;
        *fullscreen = FALSE;
        global_screen = SDL_SetVideoMode(800, 600, 32, SDL_OPENGL | SDL_RESIZABLE);
        if (global_screen == NULL)
        {
            error("glscreen", "glscreenSetVideo", "Unable to initialize screen surface.");
            /*TODO: try the first mode available in the modes list*/
        }
    }

    _fullscreen = *fullscreen;
    global_screenwidth = *width;
    global_screenheight = *height;

    SDL_WM_SetCaption("StormWar "PACKAGE_VERSION, "StormWar");
}
