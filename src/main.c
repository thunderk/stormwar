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
 *   Kernel loader and external events handler                                *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                 Includes                                   *
 ******************************************************************************/
#include "SDL.h"
#include "SDL_thread.h"

#include "main.h"
#include "kernel.h"
#include "core/comp.h"
#include "core/shell.h"
#include "core/string.h"
#include "system/mem.h"
#include "sound/sound.h"

#include <stdlib.h>
#include <stdio.h>

/******************************************************************************
 *                              Global variables                              *
 ******************************************************************************/
String global_datapath;
Uint32 global_mainthread;
Uint32 debugcount;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
parseArgs(int argc, char* argv[])
{
    int i;
    
    i = 1;
    while (i < argc)
    {
        if ((i < argc) && ((strcmp(argv[i], "--help") == 0) || (strcmp(argv[i], "-h") == 0)))
        {
            printf("Usage: %s [options...]\n\
  Options:\n\
-h  --help                  Display this help and quit.\n\
-sl --shell-log   STRING    File used to log shell messages.\n\
-sh --shell-level STRING    Set the limit of graphical shell messages:\n\
                              silent, normal, verbose, debug or harddebug.\n\
-ln --language    STRING    Set the language (example: fr, es...).\n\
-ns --nosound               Prevent sound initialization.\n",
                    argv[0]);
            exit(0);
        }
        else if ((i < argc) && ((strcmp(argv[i], "--shell-level") == 0) || (strcmp(argv[i], "-sh") == 0)))
        {
            i++;
            if (i == argc)
            {
                printf("Missing parameter for %s option.\n", argv[i - 1]);
                break;
            }
            if (strcmp(argv[i], "silent") == 0)
            {
                shellSetLevel(LEVEL_USER);
            }
            else if (strcmp(argv[i], "normal") == 0)
            {
                shellSetLevel(LEVEL_ERROR);
            }
            else if (strcmp(argv[i], "verbose") == 0)
            {
                shellSetLevel(LEVEL_INFO);
            }
            else if (strcmp(argv[i], "debug") == 0)
            {
                shellSetLevel(LEVEL_DEBUG);
            }
            else if (strcmp(argv[i], "harddebug") == 0)
            {
                shellSetLevel(LEVEL_HARDDEBUG);
            }
            else
            {
                printf("Unknown shell level: %s\n", argv[i]);
            }
        }
        else if ((i < argc) && ((strcmp(argv[i], "--shell-log") == 0) || (strcmp(argv[i], "-sl") == 0)))
        {
            i++;
            if (i == argc)
            {
                printf("Missing parameter for %s option.\n", argv[i - 1]);
                break;
            }
            shellStartLogging(argv[i]);
        }
        else if ((i < argc) && ((strcmp(argv[i], "--nosound") == 0) || (strcmp(argv[i], "-ns") == 0)))
        {
            soundDisable();
        }
        else if ((i < argc) && ((strcmp(argv[i], "--language") == 0) || (strcmp(argv[i], "-ln") == 0)))
        {
            i++;
            if (i == argc)
            {
                printf("Missing parameter for %s option.\n", argv[i - 1]);
                break;
            }
            kernelSetLanguage(argv[i]);
        }
        else
        {
            printf("Unrecognized argument: %s\n", argv[i]);
        }
        i++;
    }
}

/******************************************************************************
 *############################################################################*
 *#                              Main functions                              #*
 *############################################################################*
 ******************************************************************************/
int
main(int argc, char* argv[])
{
    global_mainthread = SDL_ThreadID();
    debugcount = 0;
    
    parseArgs(argc, argv);
    
    memInit();
    
    /*SDL initialization*/
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER
#ifdef USE_AUDIO
        | SDL_INIT_AUDIO
#endif
#if !defined(NO_EVENT_QUEUE)
        | SDL_INIT_EVENTTHREAD
#endif
        ))
    {
        printf("SDL: %s\n", SDL_GetError());
        error("main", "main", "Can't initialize SDL system.");
    }
    
    /*kernel launching*/
    kernelInit();

    /*the kernel has ended*/
    kernelUninit();
    
    /*SDL exit*/
    SDL_Quit();
    
    memUninit();
    
    return EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/
void
error(char * obj, char * funct, char * desc)
{
    printf("Error in '%s' at function '%s' :\n   %s\n", obj, funct, desc);
    compErrorMessage(PACKAGE_NAME, desc);
    criticalExit();
}

/*----------------------------------------------------------------------------*/
void
criticalExit()
{
    printf("Critical exit.\n");
    exit(EXIT_FAILURE);
}
