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
 *   Game kernel                                                              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "kernel.h"

#include "test.h"
#include <stdlib.h>

#include "main.h"
#include "tools/fonct.h"
#include "tools/tools.h"

#include "core/string.h"
#include "core/core.h"

#include "world/world.h"
#include "world/env.h"
#include "world/ground.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

#include "gui/guidialog.h"

#include "sound/sound.h"

#include "game/game.h"

#include "gui/gui.h"
#include "gui/worldmap.h"

#ifdef __W32
#include <windows.h>
/*this inclusion must be the last one because it may conflict with some types declared in the core*/
#endif

/******************************************************************************
 *                                 Variables                                  *
 ******************************************************************************/
static Bool inited = FALSE;
static Bool paused = FALSE;

static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID FUNC_VERSION = CORE_INVALID_ID;
static CoreID FUNC_VERSIONFULL = CORE_INVALID_ID;

static char* language = NULL;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
shellCallback(ShellFunction* func)
{
    if (func->id == FUNC_VERSION)
    {
        String s = String_new(VERSION);
        Var_setString(func->ret, s);
        String_del(s);
    }
    else if (func->id == FUNC_VERSIONFULL)
    {
        String s = String_new(PACKAGE_NAME" "VERSION" ");
        Var_setString(func->ret, s);
        String_del(s);
    }
}

/*----------------------------------------------------------------------------*/
static void
coreCallback(CoreEvent event)
{
    if (event == CORE_READY)
    {
        /*core is ready to start a new game, will load the title*/
        kernelTitle();
    }
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
kernelInit()
{
    SDL_version compile_version;
    const SDL_version *link_version;

    inited = FALSE;
    paused = FALSE;

    /*initialize the core*/
    coreInit();

    /*register into the core*/
    MOD_ID = coreDeclareModule("kernel", coreCallback, NULL, shellCallback, NULL, NULL, NULL);
    FUNC_VERSION = coreDeclareShellFunction(MOD_ID, "version", VAR_STRING, 0);
    FUNC_VERSIONFULL = coreDeclareShellFunction(MOD_ID, "versionfull", VAR_STRING, 0);

    /*infos*/
    SDL_VERSION(&compile_version);
    shellPrintf(LEVEL_INFO, " -> compiled with SDL version: %d.%d.%d",
        compile_version.major,
        compile_version.minor,
        compile_version.patch);
    link_version = SDL_Linked_Version();
    shellPrintf(LEVEL_INFO, " -> running with SDL version:  %d.%d.%d",
        link_version->major,
        link_version->minor,
        link_version->patch);

    rnd_init();

    /*tools*/
    toolsInit();

    /*then the engines*/
    graphicsInit();
    soundInit();
    gameInit();

    /*then the things that depends upon the engines*/
    guiInit();
    worldInit();
    groundInit();
    envInit();

    inited = TRUE;

    /*Start the core main thread, that will return when the game is finished and ready to uninit.*/
    coreStart();
}

/*----------------------------------------------------------------------------*/
void
kernelUninit()
{
    envUninit();
    groundUninit();
    worldUninit();
    guiUninit();

    gameUninit();
    soundUninit();
    graphicsUninit();

    toolsUninit();

    coreUninit();
}

/*----------------------------------------------------------------------------*/
void
kernelSetLanguage(char* lang)
{
    language = lang;
}

/*----------------------------------------------------------------------------*/
void
kernelTitle()
{
    unsigned int i;
    unsigned int j;
    String s;
    static unsigned short tmp[] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,1,0,0,0,0,0,0,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,1,1,0,1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,1,0,0,0,0,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,0,1,1,1,0,
    0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,1,0,1,1,0,1,1,1,1,1,0,0,1,1,1,1,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,0,1,1,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,1,1,1,0,0,0,1,1,1,1,0,1,1,0,
    0,0,0,0,1,1,1,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,0,1,1,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,0,1,0,1,1,1,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,1,1,1,0,1,0,0,0,1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,1,1,0,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,1,1,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,0,1,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,0,1,1,1,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,1,0,0,1,1,1,0,0,0,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,1,1,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,1,1,0,0,0,0,0,
    0,0,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,1,1,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,1,1,1,0,1,1,1,1,1,1,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    /*set the mod*/
    s = String_new("default");
    if (coreLoadData(s))
    {
        error("kernel", "kernelTitle", "Mod 'default' not found.");
        String_del(s);
        return;
    }
    String_del(s);

    /*MOTD*/
    shellPrint(LEVEL_USER, "");
    shellPrintf(LEVEL_USER, "StormWar %s", PACKAGE_VERSION);
    shellPrint(LEVEL_USER, "---------------");
    shellPrint(LEVEL_USER, "(c) 2004 Lemaire Michael");
    shellPrint(LEVEL_USER, "         <stormwar@free.fr>");
    shellPrint(LEVEL_USER, _("Released under GPLv2 license,"));
    shellPrint(LEVEL_USER, _("see COPYING file for details."));
    shellPrint(LEVEL_USER, "");

    /*start the music*/
    soundMusicPlay(TRUE);

    /*clear the game*/
    gameNew();

    /*create the title world*/
    groundClear();
    worldSetSize(105, 18);
    for (i = 0; i < 105; i++)
    {
        for (j = 0; j < 18; j++)
        {
            groundSetState(i, j, (tmp[j * 105 + i] == 1));
        }
    }

    cameraSetPos(48.0, 0.0, 9.0);
    cameraSetAngle(1.9, 0.8);
    cameraSetZoom(100.0);

    GuiDialog_popupMsg(_("This game isn't playable yet. For testing purpose only.\n\nTo start a new game, use the game menu on the top.\n\nCamera control:\n - Scroll: Screen edges in fullscreen, map clicking, ALT + mouse move\n - Rotate: Middle click or CTRL + mouse move\n - Zoom: Mouse wheel, SHIFT + mouse move"));

    testFunction(); /*for some intensive testing, should be empty in releases.*/
}

/*----------------------------------------------------------------------------*/
void
kernelNewGame(WorldCoord sizex, WorldCoord sizey, String mod, String name)
{
    /*set the mod*/
    if (coreLoadData(mod))
    {
        String s;

        shellPrintf(LEVEL_ERROR, "Requested mod '%s' not found.", String_get(mod));
        /*TODO: this is unpretty*/
        s = String_new("showmsg(\"&ERROR : Requested mod not found.\")");
        shellExec(s);
        String_del(s);
        return;
    }

    /*MOTD*/
    shellPrint(LEVEL_USER, "");
    shellPrintf(LEVEL_USER, "StormWar %s", PACKAGE_VERSION);
    shellPrint(LEVEL_USER, "---------------");
    shellPrint(LEVEL_USER, "(c) 2004 Lemaire Michael");
    shellPrint(LEVEL_USER, "         <stormwar@free.fr>");
    shellPrint(LEVEL_USER, _("Released under GPLv2 license,"));
    shellPrint(LEVEL_USER, _("see COPYING file for details."));
    shellPrint(LEVEL_USER, "");

    /*start the music*/
    soundMusicPlay(TRUE);

    /*prepare the new world*/
    worldSetSize(sizex, sizey);

    /*then set the new game*/
    gameNew();
    gameAddLocalPlayer(name);
}
