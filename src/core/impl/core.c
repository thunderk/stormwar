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
 *   Core engine                                                              *
 *                                                                            *
  ***************************************************************************/

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/core.h"

#include <SDL_thread.h>

#include "tools/fonct.h"

#include "core/impl/impl.h"
#include "core/impl/shellfunction.h"

#include "core/string.h"
#include "core/var.h"
#include "core/ptrarray.h"
#include "tools/varvalidator.h"

/******************************************************************************
 *                                   Types                                    *
 ******************************************************************************/
typedef struct
{
    String name;
    CoreID shellfuncts_nb;
    ShellFunction* shellfuncts;
    PtrArray shellfuncts_sorted;
    CoreEventCallback event_cb;
    CoreShellCallback shell_cb;
    CoreDatasCallback datas_cb;
    CorePrefsCallback prefs_cb;
    CoreResourceCallback res_cb;
    CoreThreadCallback thread_cb;
} CoreModule;

typedef struct
{
    Var resource;
    CoreID id;
    CoreID owner;
    Uint16 watchers_nb;
    CoreID* watchers;
    Bool world_writable;
} CoreResource;

typedef enum
{
    THREAD_HERE,            /* Control is here (in the core). */
    THREAD_RUNNING,         /* Running, control is elsewhere... */
    THREAD_WILLPAUSE,       /* Running but will pause when the control returns. */
    THREAD_PAUSED,          /* Control is here and paused. */
    THREAD_WILLTERM,        /* Running but will term when the control returns. */
    THREAD_DEAD             /* The thread won't do anything anymore. */
} ThreadState;

typedef enum
{
    STATE_INVALID,
    STATE_RUNNING,
    STATE_WILLPAUSE,
    STATE_PAUSED,
    STATE_WILLRESUME
} CoreState;

#define THREAD_MAIN 0
#define THREAD_MAXNB 10

typedef struct
{
    CoreID id;
    CoreID owner;
    String name;
    Bool public;
    SDL_Thread* sdlthread;
    CoreTime lasthere;      /* Last time the thread was here. */
    CoreTime timer;         /* Minimum time between two 'frames'. */
    Uint16 slots_nb;
    CoreID* slots;
    ThreadState state;
    SDL_mutex* lock;        /* To protect state, times, and the slots. */
} CoreThread;

/******************************************************************************
 *                             Shared variables                               *
 ******************************************************************************/
Var temp_corevar;
String STRING_NULL;

/******************************************************************************
 *                             Static variables                               *
 ******************************************************************************/
static volatile CoreState _state = STATE_INVALID;

/*virtual modules*/
static CoreID _modules_nb;
static CoreModule* _modules;
static PtrArray _modules_sorted;

/*preferences*/
static Var _prefs;
static String _prefsfile;

/*datas*/
static String _datapath;
static String _modpath;
static Var _modlist;

/*resources*/
static CoreID _resources_nb;
static CoreResource* _resources;
static PtrArray _resources_sorted;

/*completion list*/
static CompletionList _complist;

/*threads*/
static volatile CoreID _threads_nb;
static volatile CoreThread* _threads;

/*internal core things*/
static CoreID MOD_ID = CORE_INVALID_ID;
static CoreID FUNC_LSMOD = CORE_INVALID_ID;
static CoreID FUNC_LSFUNC = CORE_INVALID_ID;
static CoreID FUNC_LSRES = CORE_INVALID_ID;
static CoreID FUNC_ECHORES = CORE_INVALID_ID;
static CoreID FUNC_PAUSE = CORE_INVALID_ID;
static CoreID FUNC_RESUME = CORE_INVALID_ID;

/******************************************************************************
 *############################################################################*
 *#                            Private functions                             #*
 *############################################################################*
 ******************************************************************************/
static void
CoreModule_free(CoreModule* module)
{
    CoreID i;
    
    String_del(module->name);
    PtrArray_del(module->shellfuncts_sorted);
    if (module->shellfuncts_nb != 0)
    {
        for (i = 0; i < module->shellfuncts_nb; i++)
        {
            ShellFunction_del(module->shellfuncts + i);
        }
        FREE(module->shellfuncts);
    }
}

/*----------------------------------------------------------------------------*/
static int
CoreModule_cmp(CoreModule** mod1, CoreModule** mod2)
{
    return String_cmp(&((*mod1)->name), &((*mod2)->name));
}

/*----------------------------------------------------------------------------*/
static void
CoreResource_free(CoreResource* resource)
{
    Var_del(resource->resource);
    if (resource->watchers_nb != 0)
    {
        FREE(resource->watchers);
    }
}

/*----------------------------------------------------------------------------*/
static int
CoreResource_cmp(CoreResource** res1, CoreResource** res2)
{
    return Var_nameCmp(&((*res1)->resource), &((*res2)->resource));
}

/*----------------------------------------------------------------------------*/
static Bool
fileExists(String filepath)
{
    FILE* f = fopen(String_get(filepath), "r");
    if (f == NULL)
    {
        return FALSE;
    }
    else
    {
        fclose(f);
        return TRUE;
    }
}

/*----------------------------------------------------------------------------*/
static void
findData()
{
    /*find the data file to fill _datapath*/
    String filename;
    String s;
    String path;
    String pwd;
    
    filename = String_new(PACKAGE_NAME"_data");
    
#ifdef __W32
    pwd = String_new(".");
#else
    pwd = String_new(getenv("PWD"));
#endif

    path = String_new(DATA_DIR);
    if (String_get(path)[String_getLength(path) - 1] != '/')
    {
        String_appendChar(path, '/');
    }
    
    s = String_newByCopy(path);
    String_appendString(s, filename);
    
    if (!fileExists(s))
    {
        String_del(path);
        String_del(s);
        path = String_new("/usr/local/share/"PACKAGE_NAME"/");
        s = String_newByCopy(path);
        String_appendString(s, filename);
        if (!fileExists(s))
        {
            String_del(path);
            String_del(s);
            path = String_new("/usr/share/"PACKAGE_NAME"/");
            s = String_newByCopy(path);
            String_appendString(s, filename);
            if (!fileExists(s))
            {
                String_del(path);
                String_del(s);
                path = String_newByCopy(pwd);
                if (String_get(path)[String_getLength(path) - 1] != '/')
                {
                    String_appendChar(path, '/');
                }
                s = String_newByCopy(path);
                String_appendString(s, filename);
                if (!fileExists(s))
                {
                    String_del(path);
                    String_del(s);
                    path = String_newByCopy(pwd);
                    if (String_get(path)[String_getLength(path) - 1] != '/')
                    {
                        String_appendChar(path, '/');
                    }
                    String_append(path, "data/");
                    s = String_newByCopy(path);
                    String_appendString(s, filename);
                    if (!fileExists(s))
                    {
                        error("core", "findData", "Data reference file '"PACKAGE_NAME"_data' not found.");
                    }
                }
            }
        }
    }

    String_del(pwd);
    String_del(filename);
    _datapath = String_newByCopy(path);
    shellPrintf(LEVEL_INFO, " -> Datas found in: %s", String_get(_datapath));
    String_del(path);
    
    /*load the mod list*/
    _modlist = Var_new();
    if (Var_readFromFile(_modlist, s) || (Var_getType(_modlist) != VAR_ARRAY))
    {
        shellPrintf(LEVEL_ERROR, "Can't open global data file '%s'.", String_get(s));
        error("core", "findData", "Can't parse global data file.");
    }
    String_del(s);
}

/*----------------------------------------------------------------------------*/
static void
bindPreferences(String prefsfile)
{
    VarValidator valid;
    CoreID i;
    String s;
    
    ASSERT(_prefs != NULL, return);
    
    if (_prefsfile == NULL)
    {
        _prefsfile = String_newByCopy(prefsfile);
        
        /*read from the file*/
        if (Var_readFromFile(_prefs, _prefsfile))
        {
            shellPrintf(LEVEL_INFO, "Can't open preferences file '%s', using default instead.", String_get(_prefsfile));
        }
        
        /*check modules presence*/
        valid = VarValidator_new();
        for (i = 0; i < _modules_nb; i++)
        {
            if (_modules[i].prefs_cb != NULL)
            {
                VarValidator_declareArrayVar(valid, String_get(_modules[i].name));
            }
        }
        VarValidator_declareStringVar(valid, "versionnumber", "current");
        VarValidator_validate(valid, _prefs);
        VarValidator_del(valid);
        
        /*if the version doesn't match, clear preferences*/
        s = String_new(VERSION);
        if (!String_equal(s, Var_getValueString(Var_getArrayElemByCName(_prefs, "versionnumber"))))
        {
            shellPrint(LEVEL_INFO, "Preferences version doesn't match current one, clearing.");
            for (i = 0; i < _modules_nb; i++)
            {
                if (_modules[i].prefs_cb != NULL)
                {
                    Var_setArray(Var_getArrayElemByName(_prefs, _modules[i].name));
                }
            }
        }
        Var_setString(Var_getArrayElemByCName(_prefs, "versionnumber"), s);
        String_del(s);
        
        /*raise callbacks*/
        for (i = 0; i < _modules_nb; i++)
        {
            if (_modules[i].prefs_cb != NULL)
            {
                Var v;
                
                v = Var_getArrayElemByName(_prefs, _modules[i].name);
                ASSERT(v != NULL, continue);
                _modules[i].prefs_cb(v);
            }
        }
        shellPrintf(LEVEL_INFO, "Preferences loaded from file '%s'.", String_get(_prefsfile));
    }
}

/*----------------------------------------------------------------------------*/
static int
threadProcessor(void* data)
{
    CoreThread* thread;
    CoreTime curtime;
    CoreTime duration;
    Uint16 i;
    
    thread = (CoreThread*)data;
    
    /*TODO: thread-safe things*/
    
    curtime = getTicks();
    while (thread->state != THREAD_WILLTERM)
    {
        if (thread->state == THREAD_WILLPAUSE)
        {
            thread->state = THREAD_PAUSED;
        }
        
        /*time regulation*/
        duration = getTicks() - curtime;
        if (duration < thread->timer)
        {
            SDL_Delay(thread->timer - duration);
            duration = thread->timer;
        }
        curtime = getTicks();
        
        /*call callbacks*/
        if (thread->id == THREAD_MAIN)
        {
            if (_state == STATE_WILLPAUSE)
            {
                for (i = 0; i < _modules_nb; i++)
                {
                    if (_modules[i].event_cb != NULL)
                    {
                        _modules[i].event_cb(CORE_PAUSE);
                    }
                }
                _state = STATE_PAUSED;
            }
            else if (_state == STATE_WILLRESUME)
            {
                for (i = 0; i < _modules_nb; i++)
                {
                    if (_modules[i].event_cb != NULL)
                    {
                        _modules[i].event_cb(CORE_RESUME);
                    }
                }
                _state = STATE_RUNNING;
            }
        }
        else
        {
            if (thread->state != THREAD_PAUSED)
            {
                /*TODO: put in THREAD_RUNNING state, don't overwrite THREAD_WILLTERM on return*/
                _modules[thread->owner].thread_cb(thread->id, duration);
            }
        }
        if (thread->state != THREAD_PAUSED)
        {
            for (i = 0; i < thread->slots_nb; i++)
            {
                /* FIXME: should compute the new duration for each slot */
                _modules[thread->slots[i]].thread_cb(thread->id, duration);
            }
        }
    }
    thread->state = THREAD_DEAD;
    
    return 0;
}

/*----------------------------------------------------------------------------*/
static CoreModule*
findModule(const char* name)
{
    CoreModule m;
    PtrArrayIterator i;

    m.name = String_new(name);
    i = PtrArray_findSorted(_modules_sorted, &m);
    String_del(m.name);
    
    if (i == NULL)
    {
        return NULL;
    }
    else
    {
        return (CoreModule*)(*i);
    }
}

/*----------------------------------------------------------------------------*/
static ShellFunction*
findFunction(CoreModule* module, const char* name)
{
    ShellFunction f;
    PtrArrayIterator i;
    
    f.name = String_new(name);
    i = PtrArray_findSorted(module->shellfuncts_sorted, &f);
    String_del(f.name);
    
    if (i == NULL)
    {
        return NULL;
    }
    else
    {
        return (ShellFunction*)(*i);
    }
}

/*----------------------------------------------------------------------------*/
static void
printfuncname(Ptr func)
{
    String s;
    
    s = String_new("");
    ShellFunction_getPrototype((ShellFunction*)func, s);
    shellPrintf(LEVEL_USER, " %s", String_get(s));
    String_del(s);
}

/*----------------------------------------------------------------------------*/
static void
shellCallback(ShellFunction* func)
{
    CoreID i;
    
    if (func->id == FUNC_LSMOD)
    {
        shellPrint(LEVEL_USER, _("Declared modules:"));
        for (i = 0; i < _modules_nb; i++)
        {
            shellPrintf(LEVEL_USER, " %s", String_get(_modules[i].name));
        }
    }
    else if (func->id == FUNC_LSFUNC)
    {
        CoreModule* mod;
        
        ASSERT(func->nbparam == 1, return);
        ASSERT(Var_getType(func->params[0]) == VAR_STRING, return);
        
        if ((mod = findModule(String_get(Var_getValueString(func->params[0])))) == NULL)
        {
            shellPrintf(LEVEL_ERROR, _("Module %s not found."), String_get(Var_getValueString(func->params[0])));
        }
        else
        {
            shellPrintf(LEVEL_USER, _("Declared functions for %s module:"), String_get(Var_getValueString(func->params[0])));
            PtrArray_foreach(mod->shellfuncts_sorted, printfuncname);
        }
    }
    else if (func->id == FUNC_LSRES)
    {
        shellPrint(LEVEL_USER, _("Declared resources:"));
        for (i = 0; i < _resources_nb; i++)
        {
            shellPrintf(LEVEL_USER, " %s", String_get(Var_getName(_resources[i].resource)));
        }
    }
    else if (func->id == FUNC_ECHORES)
    {
        ASSERT(func->nbparam == 1, return);
        ASSERT(Var_getType(func->params[0]) == VAR_STRING, return);
        
        i = coreGetResourceID(String_get(Var_getValueString(func->params[0])));
        if (i == CORE_INVALID_ID)
        {
            shellPrintf(LEVEL_ERROR, _("Ressource '%s' not found."), String_get(Var_getValueString(func->params[0])));
        }
        else
        {
            shellPrintf(LEVEL_USER, "%s", Var_gets(_resources[i].resource));
        }
    }
    else if (func->id == FUNC_PAUSE)
    {
        corePause();
    }
    else if (func->id == FUNC_RESUME)
    {
        coreResume();
    }
    Var_setVoid(func->ret);
}

/******************************************************************************
 *############################################################################*
 *#                            Internal functions                            #*
 *############################################################################*
 ******************************************************************************/
ShellFunction*
coreFindShellFunction(String modname, String funcname, CoreShellCallback* r_callback)
{
    CoreModule* module;
    ShellFunction* func;
    
    if (modname == NULL)
    {
        CoreID i;
        
        /*do a full search*/
        func = NULL;
        i = 0;
        do
        {
            func = findFunction(_modules + i, String_get(funcname));
            i++;
        } while (i < _modules_nb && func == NULL);
        if (func == NULL)
        {
            shellPrintf(LEVEL_ERROR, _("Function '%s' not found in any module."), String_get(funcname));
            return NULL;
        }
        module = _modules + i - 1;
    }
    else
    {
        module = findModule(String_get(modname));
        if (module == NULL)
        {
            shellPrintf(LEVEL_ERROR, _("Module '%s' not found."), String_get(modname));
            return NULL;
        }
        func = findFunction(module, String_get(funcname));
        if (func == NULL)
        {
            shellPrintf(LEVEL_ERROR, _("Function '%s' not found in module '%s'."), String_get(funcname), String_get(modname));
            return NULL;
        }
    }
    
    *r_callback = module->shell_cb;
    return func;
}

/******************************************************************************
 *############################################################################*
 *#                             Public functions                             #*
 *############################################################################*
 ******************************************************************************/
void
coreInit()
{
    /*constants and temporaries*/
    temp_corevar = Var_new();
    STRING_NULL = String_new("");
    
    /*core references*/
    _modules_nb = 0;
    _modules = NULL;
    _modules_sorted = PtrArray_newFull(5, 5, NULL, (PtrCmpFunc)CoreModule_cmp);
    
    _prefs = Var_new();
    Var_setArray(_prefs);
    _prefsfile = NULL;
    
    _datapath = NULL;   /*will be inited by findData*/
    _modpath = String_new("");
    
    _resources_nb = 0;
    _resources = NULL;
    _resources_sorted = PtrArray_newFull(20, 10, NULL, (PtrCmpFunc)CoreResource_cmp);
    
    _complist = CompletionList_new();
    
    _threads_nb = 0;
    _threads = MALLOC(sizeof(CoreThread) * THREAD_MAXNB);
    
    /*here goes the shell because the self-declaration may need it*/
    shellInit();

    /*then find the datas*/
    findData();

    /*then i18n to translate things*/
    i18nInit();
    
    /*self-declaration*/
    MOD_ID = coreDeclareModule("core", NULL, NULL, shellCallback, NULL, NULL, NULL);
    ASSERT_CRITICAL(MOD_ID != CORE_INVALID_ID);
    FUNC_LSMOD = coreDeclareShellFunction(MOD_ID, "lsmod", VAR_VOID, 0);
    FUNC_LSFUNC = coreDeclareShellFunction(MOD_ID, "lsfunc", VAR_VOID, 1, VAR_STRING);
    FUNC_LSRES = coreDeclareShellFunction(MOD_ID, "lsres", VAR_VOID, 0);
    FUNC_ECHORES = coreDeclareShellFunction(MOD_ID, "echores", VAR_VOID, 1, VAR_STRING);
    FUNC_PAUSE = coreDeclareShellFunction(MOD_ID, "pause", VAR_VOID, 0);
    FUNC_RESUME = coreDeclareShellFunction(MOD_ID, "resume", VAR_VOID, 0);

    /*main thread*/
    _threads[THREAD_MAIN].id = THREAD_MAIN;
    _threads[THREAD_MAIN].owner = MOD_ID;
    _threads[THREAD_MAIN].name = String_new("main");
    _threads[THREAD_MAIN].public = TRUE;
    _threads[THREAD_MAIN].sdlthread = NULL;
    _threads[THREAD_MAIN].lasthere = getTicks();
    _threads[THREAD_MAIN].timer = 0;
    _threads[THREAD_MAIN].slots_nb = 0;
    _threads[THREAD_MAIN].slots = NULL;
    _threads[THREAD_MAIN].state = THREAD_HERE;
    _threads[THREAD_MAIN].lock = SDL_CreateMutex();
    _threads_nb = 1;
    
    _state = STATE_RUNNING;
}

/*----------------------------------------------------------------------------*/
void
coreUninit()
{
    CoreID i;
    
    /*threads*/
    for (i = 0; i < _threads_nb; i++)
    {
        /* threads should all be dead at this point */
        ASSERT_WARN(_threads[i].state == THREAD_DEAD);
        if (_threads[i].slots_nb != 0)
        {
            FREE(_threads[i].slots);
        }
        SDL_DestroyMutex(_threads[i].lock);
        String_del(_threads[i].name);
    }
    FREE((void*)_threads);
    
    /*completion list*/
    CompletionList_del(_complist);
    
    /*preferences*/
    if (_prefsfile != NULL)
    {
        if (Var_saveToFile(_prefs, _prefsfile))
        {
            shellPrintf(LEVEL_ERROR, "Can't write preferences file '%s' !", String_get(_prefsfile));
        }
        else
        {
            shellPrintf(LEVEL_INFO, "Preferences saved to file '%s'", String_get(_prefsfile));
        }
        String_del(_prefsfile);
    }
    Var_del(_prefs);
    
    /*modules*/
    if (_modules_nb != 0)
    {
        for (i = 0; i < _modules_nb; i++)
        {
            CoreModule_free(_modules + i);
        }
        FREE(_modules);
    }
    PtrArray_del(_modules_sorted);

    /*resources*/
    if (_resources_nb != 0)
    {
        for (i = 0; i < _resources_nb; i++)
        {
            CoreResource_free(_resources + i);
        }
        FREE(_resources);
    }
    PtrArray_del(_resources_sorted);
    
    /*datas*/
    Var_del(_modlist);
    String_del(_datapath);
    String_del(_modpath);

    /*constants and temporaries*/
    Var_del(temp_corevar);
    String_del(STRING_NULL);
    
    /*internal modules*/
    i18nUninit();
    shellUninit();
}

/*----------------------------------------------------------------------------*/
void
coreStart()
{
    CoreID i;
    Bool loop;
    char* prefpath;
    String prefpathstr;
    
    ASSERT(_state == STATE_RUNNING, return);
    
    /*loading up preferences from file*/
#ifdef __W32
    prefpath = getenv("APPDATA");
    if ((prefpath == NULL) || (prefpath[0] == '\0'))
    {
        /*error("main", "main", _("Incorrect or unset environment variable 'APPDATA'."));*/
        prefpathstr = String_new(PACKAGE_NAME".sav");
    }
    else
    {
        prefpathstr = String_new(prefpath);
        String_append(prefpathstr, "\\"PACKAGE_NAME".sav");
    }
#else
    prefpath = getenv("HOME");
    if ((prefpath == NULL) || (prefpath[0] != '/'))
    {
        error("main", "main", "Incorrect or unset environment variable 'HOME'.");
        prefpathstr = String_new(".");
    }
    else
    {
        prefpathstr = String_new(prefpath);
    }
    String_append(prefpathstr, "/."PACKAGE_NAME);
#endif
    bindPreferences(prefpathstr);
    
    /*throw the CORE_READY event*/
    for (i = 0; i < _modules_nb; i++)
    {
        if (_modules[i].event_cb != NULL)
        {
            _modules[i].event_cb(CORE_READY);
        }
    }
    
    /*start the thread processor for the main thread*/
    /*this will return when the thread is terminated*/
    threadProcessor((void*)(_threads + THREAD_MAIN));
    
    /*wait for other threads to die*/
    do
    {
        loop = FALSE;
        for (i = 0; i < _threads_nb; i++)
        {
            if (_threads[i].state != THREAD_DEAD)
            {
                loop = TRUE;
            }
        }
    } while (loop);
    
    String_del(prefpathstr);
}

/*----------------------------------------------------------------------------*/
void
coreStop()
{
    CoreID i;
    
    ASSERT_CRITICAL(_state != STATE_INVALID);
    
    /*ask all threads to term*/
    for (i = 0; i < _threads_nb; i++)
    {
        coreTermThread(MOD_ID, i);
    }
}

/*----------------------------------------------------------------------------*/
void
corePause()
{
    /*TODO: thread-safe*/
    if (_state == STATE_RUNNING)
    {
        _state = STATE_WILLPAUSE;
    }
}

/*----------------------------------------------------------------------------*/
void
coreResume()
{
    /*TODO: thread-safe*/
    if (_state == STATE_PAUSED)
    {
        _state = STATE_WILLRESUME;
    }
}

/*----------------------------------------------------------------------------*/
Bool
coreLoadData(String modname)
{
    VarValidator valid;
    Var mod;
    CoreID i;
    String s;
    VarArrayPos modpos;
    String _oldpath;
    
    _oldpath = String_newByCopy(_modpath);
    
    /*check if this mod is in the mod list and build the path*/
    /*TODO: don't stop on non string*/
    for (modpos = 0;
         (modpos < Var_getArraySize(_modlist)) &&
          (Var_getType(Var_getArrayElemByPos(_modlist, modpos)) == VAR_STRING) &&
          (!String_equal(Var_getValueString(Var_getArrayElemByPos(_modlist, modpos)), modname));
         modpos++)
    {
    }
    if (modpos < Var_getArraySize(_modlist))
    {
        String_copy(_modpath, _datapath);
        String_appendString(_modpath, modname);
        String_appendChar(_modpath, '/');
    }
    else
    {
        shellPrintf(LEVEL_ERROR, "Mod '%s' not found.", String_get(modname));
        String_copy(_modpath, _oldpath);
        String_del(_oldpath);
        return TRUE;
    }
    
    shellPrintf(LEVEL_INFO, "Start loading mod: %s%s", String_get(_modpath), String_get(modname));
    shellPrintf(LEVEL_ERRORSTACK, "In mod: %s", String_get(modname));
    
    /*load the mod main file in a var*/
    s = String_new("mod_def");
    mod = Var_new();
    if (Var_readFromFile(mod, s))
    {
        String_del(s);
        Var_del(mod);
        shellPrint(LEVEL_ERROR, "Mod's definition file not found.");
        shellPopErrorStack();
        String_copy(_modpath, _oldpath);
        String_del(_oldpath);
        return TRUE;
    }
    String_del(s);
    
    /*build the data validator*/
    valid = VarValidator_new();
    for (i = 0; i < _modules_nb; i++)
    {
        if (_modules[i].datas_cb != NULL)
        {
            VarValidator_declareArrayVar(valid, String_get(_modules[i].name));
        }
    }
    
    /*validate the mod file*/
    VarValidator_validate(valid, mod);
    VarValidator_del(valid);
    
    /*throw CORE_DATALOADING event*/
    for (i = 0; i < _modules_nb; i++)
    {
        if (_modules[i].event_cb != NULL)
        {
            _modules[i].event_cb(CORE_DATALOADING);
        }
    }
    
    /*pass datas to modules*/
    for (i = 0; i < _modules_nb; i++)
    {
        if (_modules[i].datas_cb != NULL)
        {
            shellPrintf(LEVEL_INFO, "Sending datas to '%s' module.", String_get(_modules[i].name));
            _modules[i].datas_cb(Var_getArrayElemByName(mod, _modules[i].name));
        }
    }

    /*throw CORE_DATALOADED event*/
    for (i = 0; i < _modules_nb; i++)
    {
        if (_modules[i].event_cb != NULL)
        {
            _modules[i].event_cb(CORE_DATALOADED);
        }
    }
    
    Var_del(mod);
    String_del(_oldpath);
    shellPopErrorStack();
    return FALSE;
}

/*----------------------------------------------------------------------------*/
void
coreFindData(String path)
{
    String s;
    
    s = String_newByCopy(_modpath);
    String_appendString(s, path);
    String_copy(path, s);
    String_del(s);
}

/*----------------------------------------------------------------------------*/
CoreID
coreDeclareModule(const char* name, CoreEventCallback event_cb, CoreDatasCallback datas_cb, CoreShellCallback shell_cb, CorePrefsCallback prefs_cb, CoreResourceCallback res_cb, CoreThreadCallback thread_cb)
{
    CoreModule* module;
    CoreID i;
    
    /*check if the module already exists*/
    if (findModule(name) != NULL)
    {
        shellPrintf(LEVEL_ERROR, "Core module '%s' already declared.", name);
        return CORE_INVALID_ID;
    }
    else
    {
        shellPrintf(LEVEL_INFO, "Module '%s' declared.", name);
    }
    
    /*allocate*/
    if (_modules_nb++ == 0)
    {
        _modules = MALLOC(sizeof(CoreModule));
    }
    else
    {
        _modules = REALLOC(_modules, sizeof(CoreModule) * _modules_nb);
    }
    
    module = _modules + _modules_nb - 1;
    
    /*fill*/
    module->name = String_new(name);
    module->shellfuncts_nb = 0;
    module->shellfuncts_sorted = PtrArray_newFull(5, 3, NULL, (PtrCmpFunc)ShellFunction_cmp);
    module->event_cb = event_cb;
    module->datas_cb = datas_cb;
    module->shell_cb = shell_cb;
    module->prefs_cb = prefs_cb;
    module->res_cb = res_cb;
    module->thread_cb = thread_cb;
    
    /*build the sorted array again (we must rebuild it all because of the REALLOC)*/
    PtrArray_clear(_modules_sorted);
    for (i = 0; i < _modules_nb; i++)
    {
        PtrArray_insertSorted(_modules_sorted, _modules + i);
    }
    
    CompletionList_add(_complist, module->name);
    
    return _modules_nb - 1;
}

/*----------------------------------------------------------------------------*/
CoreID
coreDeclareShellFunction(CoreID module, const char* name, VarType rettype, int nbparams, ...)
{
    ShellFunction* funct;
    ShellFunction func;
    String funcname;
    int i;
    PtrArrayIterator f;
    va_list ap;
    
    /*checks*/
    if ((module < 0) || (module >= _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "Trying to declare function '%s' for non-existing module %d.", name, module);
        return CORE_INVALID_ID;
    }
    ASSERT(nbparams >= 0, nbparams = 0);
    
    funcname = String_new(name);
    
    func.name = funcname;
    f = PtrArray_findSorted(_modules[module].shellfuncts_sorted, &func);
    if (f != NULL)
    {
        String_del(funcname);
        shellPrintf(LEVEL_ERROR, "Trying to declare an already existing function \"%s\" for %s module.", name, String_get(_modules[module].name));
        return CORE_INVALID_ID;
    }
   
    /*we create the function*/
    if (_modules[module].shellfuncts_nb++ == 0)
    {
        _modules[module].shellfuncts = MALLOC(sizeof(ShellFunction));
    }
    else
    {
        _modules[module].shellfuncts = REALLOC(_modules[module].shellfuncts, sizeof(ShellFunction) * _modules[module].shellfuncts_nb);
    }
    funct = _modules[module].shellfuncts + _modules[module].shellfuncts_nb - 1;
    ShellFunction_new(funct, funcname, _modules[module].shellfuncts_nb - 1, nbparams, rettype);

    /*we add the parameters*/
    va_start(ap, nbparams);
    for (i = 0; i < nbparams; i++)
    {
        ShellFunction_setParamType(funct, i, va_arg(ap, VarType));
    }
    va_end(ap);
    
    /*build the sorted array again (we must rebuild it all because of the REALLOC)*/
    PtrArray_clear(_modules[module].shellfuncts_sorted);
    for (i = 0; i < _modules[module].shellfuncts_nb; i++)
    {
        PtrArray_insertSorted(_modules[module].shellfuncts_sorted, _modules[module].shellfuncts + i);
    }
    
    CompletionList_add(_complist, funcname);
    
    String_del(funcname);
    return funct->id;
}

/*----------------------------------------------------------------------------*/
CoreID
coreCreateResource(CoreID owner, const char* name, VarType type, Bool world_writable)
{
    CoreResource* res;
    CoreID i;
    
    /*checks*/
    if ((owner < 0) || (owner >= _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "Trying to declare resource '%s' for non-existing module %d.", name, owner);
        return CORE_INVALID_ID;
    }

    i = coreGetResourceID(name);
    if (i != CORE_INVALID_ID)
    {
        res = _resources + i;
        /* The resource already exists. If its owner is CORE_INVALID_ID, the resource was pre-declared by adding a watcher. */
        if (res->owner == CORE_INVALID_ID)
        {
            res->owner = owner;
            Var_setType(res->resource, type);
            res->world_writable = world_writable;
            return res->id;
        }
        else
        {
            shellPrintf(LEVEL_ERROR, "Module '%s' tried to declare an already existing resource '%s', declared by module '%s'.", String_get(_modules[owner].name), name, String_get(_modules[res->owner].name));
            return CORE_INVALID_ID;
        }
    }
    
    /*allocate*/
    if (_resources_nb++ == 0)
    {
        _resources = MALLOC(sizeof(CoreResource));
    }
    else
    {
        _resources = REALLOC(_resources, sizeof(CoreResource) * _resources_nb);
    }
    res = _resources + _resources_nb - 1;

    /*fill*/
    res->resource = Var_new();
    Var_setType(res->resource, type);
    Var_setName(res->resource, name);
    res->owner = owner;
    res->world_writable = world_writable;
    res->watchers_nb = 0;
    res->id = _resources_nb - 1;
    
    /*build the sorted array again (we must rebuild it all because of the REALLOC)*/
    PtrArray_clear(_resources_sorted);
    for (i = 0; i < _resources_nb; i++)
    {
        PtrArray_insertSorted(_resources_sorted, _resources + i);
    }
    
    CompletionList_add(_complist, Var_getName(res->resource));
    
    return res->id;
}

/*----------------------------------------------------------------------------*/
CoreID
coreAddResourceWatcher(CoreID module, const char* name)
{
    CoreID resid;
    CoreResource* res;
    CoreID i;

    /*checks*/
    if ((module < 0) || (module >= _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "Trying to add a watcher for non-existing module %d.", module);
        return CORE_INVALID_ID;
    }
    if (_modules[module].res_cb == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' tried to add a module without a callback.", String_get(_modules[module].name));
        return CORE_INVALID_ID;
    }
    
    resid = coreGetResourceID(name);
    if (resid == CORE_INVALID_ID)
    {
        /* The resource isn't declared yet, make a pre-declaration */
        
        /*allocate*/
        if (_resources_nb++ == 0)
        {
            _resources = MALLOC(sizeof(CoreResource));
        }
        else
        {
            _resources = REALLOC(_resources, sizeof(CoreResource) * _resources_nb);
        }
        resid = _resources_nb - 1;
        res = _resources + resid;
    
        /*fill*/
        res->resource = Var_new();
        Var_setName(res->resource, name);
        res->owner = CORE_INVALID_ID;
        res->world_writable = FALSE;        /*in case some module wants to modify this resource*/
        res->watchers_nb = 1;
        res->watchers = MALLOC(sizeof(CoreID));
        res->watchers[0] = module;
        res->id = resid;
        
        /*build the sorted array again (we must rebuild it all because of the REALLOC)*/
        PtrArray_clear(_resources_sorted);
        for (i = 0; i < _resources_nb; i++)
        {
            PtrArray_insertSorted(_resources_sorted, _resources + i);
        }
        
        CompletionList_add(_complist, Var_getName(res->resource));
    }
    else
    {
        /* The resource is already declared or pre-declared */
        res = _resources + resid;
        
        /*check if the module isn't a watcher already*/
        for (i = 0; i < res->watchers_nb; i++)
        {
            if (res->watchers[i] == module)
            {
                shellPrintf(LEVEL_ERROR, "Module '%s' is already watching '%s' resource.", String_get(_modules[module].name), String_get(Var_getName(res->resource)));
                return resid;
            }
        }
        
        /*add the watcher*/
        /*TODO: if the module is the owner, maybe put it in the first place*/
        if (res->watchers_nb++ == 0)
        {
            res->watchers = MALLOC(sizeof(CoreID));
        }
        else
        {
            res->watchers = REALLOC(res->watchers, sizeof(CoreID) * res->watchers_nb);
        }
        res->watchers[res->watchers_nb - 1] = module;
    }
    
    return resid;
}

/*----------------------------------------------------------------------------*/
CoreID
coreGetResourceID(const char* name)
{
    CoreResource resfind;
    PtrArrayIterator it;

    resfind.resource = Var_new();
    Var_setName(resfind.resource, name);
    it = PtrArray_findSorted(_resources_sorted, &resfind);

    if (it == NULL)
    {
        Var_del(resfind.resource);
        return CORE_INVALID_ID;
    }
    else
    {
        Var_del(resfind.resource);
        return ((CoreResource*)(*it))->id;
    }
}

/*----------------------------------------------------------------------------*/
void
coreSetResourceValue(CoreID module, CoreID resource, Var value)
{
    CoreResource* res;
    int i;
    const char* modname;
    
    if ((module < 0) || (module >= _modules_nb))
    {
        modname = "unknown";
    }
    else
    {
        modname = String_get(_modules[module].name);
    }
    
    /*checks*/
    if ((resource < 0) || (resource >= _resources_nb) || (_resources[resource].owner == CORE_INVALID_ID))
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' tried to change the not declared resource %d.", modname, resource);
        return;
    }
    res = _resources + resource;
    if ((module != res->owner) & (!res->world_writable))
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' tried to change the read-only resource '%s'.", modname, String_get(Var_getName(res->resource)));
        return;
    }
    if (Var_getType(value) != Var_getType(res->resource))
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' tried to change the type of resource '%s'.", modname, String_get(Var_getName(res->resource)));
        return;
    }
    
    /*modifying*/
    Var_setFromVar(res->resource, value);
    
    /*raising callbacks*/
    for (i = 0; i < res->watchers_nb; i++)
    {
        _modules[res->watchers[i]].res_cb(resource, value);
    }
}

/*----------------------------------------------------------------------------*/
void
coreAskResourceSending(CoreID module, CoreID resource)
{
    CoreResource* res;
    Var v;
    
    /*checks*/
    if ((resource < 0) || (resource >= _resources_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module %d asked the not declared resource %d.", module, resource);
        return;
    }
    res = _resources + resource;
    if ((module < 0) || (module >= _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "A non-existing module %d tried to ask the resource '%s'.", module, String_get(Var_getName(res->resource)));
        return;
    }
    if (_modules[module].res_cb == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' tried to ask the resource '%s' without a callback set.", String_get(_modules[module].name), String_get(Var_getName(res->resource)));
    }
    
    /*send the value through the callback*/
    v = Var_new();
    Var_setFromVar(v, res->resource);
    _modules[module].res_cb(resource, v);
    Var_del(v);
}

/*----------------------------------------------------------------------------*/
CompletionList
coreGetCompletionList()
{
    return _complist;
}

/*----------------------------------------------------------------------------*/
void
coreCreateThread(CoreID owner, const char* name, Bool public, volatile CoreID* id)
{
    CoreID i;
    String s;
    
    if ((owner < 0) | (owner >= _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "An unknown module tried to declare a thread named '%s'.", name);
        *id = CORE_INVALID_ID;
        return;
    }
    if (_threads_nb == THREAD_MAXNB)
    {
        shellPrint(LEVEL_ERROR, "Max number of core threads reached.");
        *id = CORE_INVALID_ID;
        return;
    }
    if (_modules[owner].thread_cb == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' tried to create a thread with no callback set.", String_get(_modules[owner].name));
        *id = CORE_INVALID_ID;
        return;
    }
    
    /*Check if a thread of this name already exists*/
    s = String_new(name);
    for (i = 0; i < _threads_nb; i++)
    {
        if (String_equal(_threads[i].name, s))
        {
            shellPrintf(LEVEL_ERROR, "Module '%d' tried to declare the already existing thread '%s'.", owner, name);
            String_del(s);
            *id = CORE_INVALID_ID;
            return;
        }
    }
    
    /*Create the thread structure*/
    _threads[_threads_nb].id = _threads_nb;
    _threads[_threads_nb].owner = owner;
    _threads[_threads_nb].name = s;
    _threads[_threads_nb].public = public;
    _threads[_threads_nb].lasthere = getTicks();
    _threads[_threads_nb].timer = 0;
    _threads[_threads_nb].slots_nb = 0;
    _threads[_threads_nb].state = THREAD_HERE;
    _threads[_threads_nb].lock = SDL_CreateMutex();
    
    SDL_mutexP(_threads[_threads_nb].lock);
    *id = _threads_nb;
    _threads_nb++;
    _threads[_threads_nb - 1].sdlthread = SDL_CreateThread(threadProcessor, (void*)(_threads + _threads_nb - 1));
    SDL_mutexV(_threads[_threads_nb - 1].lock);
}

/*----------------------------------------------------------------------------*/
void
coreTermThread(CoreID module, CoreID thread)
{
    /*We place the thread in a THREAD_WILLTERM state.
      If it doesn't return control after a certain time, it will be killed by the safety thread.*/
    
    if ((thread < 0) | (thread >= _threads_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to term the unknown thread '%d'.", module, thread);
    }
    else if ((_threads[thread].owner != module) & (module != MOD_ID))   /*core module is allowed to term anyone*/
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to term the not owned thread '%d'.", module, thread);
    }
    else
    {
        SDL_mutexP(_threads[thread].lock);
        _threads[thread].state = THREAD_WILLTERM;
        SDL_mutexV(_threads[thread].lock);
    }
}

/*----------------------------------------------------------------------------*/
void
coreKillThread(CoreID module, CoreID thread)
{
    if ((thread < 0) | (thread >= _threads_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to kill the unknown thread '%d'.", module, thread);
    }
    else if (_threads[thread].owner != module)
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to kill the not owned thread '%d'.", module, thread);
    }
    else if (thread == THREAD_MAIN)
    {
        shellPrint(LEVEL_ERROR, "Main thread asked to be killed !");
    }
    else
    {
        SDL_mutexP(_threads[thread].lock);
        _threads[thread].state = THREAD_DEAD;
        SDL_mutexV(_threads[thread].lock);
        
        SDL_KillThread(_threads[thread].sdlthread);
        /* memory freeing for the structure will be done at uninit */
    }
}

/*----------------------------------------------------------------------------*/
void
corePauseThread(CoreID module, CoreID thread)
{
    if ((thread < 0) | (thread >= _threads_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to pause the unknown thread '%d'.", module, thread);
    }
    else if (_threads[thread].owner != module)
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to pause the not owned thread '%d'.", module, thread);
    }
    else if (thread == THREAD_MAIN)
    {
        shellPrint(LEVEL_ERROR, "Main thread asked to be paused !");
    }
    else if (_threads[thread].state == THREAD_DEAD)
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to pause the dead thread '%d'.", module, thread);
    }
    else if (_threads[thread].state != THREAD_WILLTERM)
    {
        SDL_mutexP(_threads[thread].lock);
        _threads[thread].state = THREAD_WILLPAUSE;
        SDL_mutexV(_threads[thread].lock);
    }
}

/*----------------------------------------------------------------------------*/
void
coreResumeThread(CoreID module, CoreID thread)
{
    if ((thread < 0) | (thread >= _threads_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to resume the unknown thread '%d'.", module, thread);
    }
    else if (_threads[thread].owner != module)
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to resume the not owned thread '%d'.", module, thread);
    }
    else if (_threads[thread].state == THREAD_PAUSED)
    {
        SDL_mutexP(_threads[thread].lock);
        _threads[thread].state = THREAD_RUNNING;
        SDL_mutexV(_threads[thread].lock);
    }
}

/*----------------------------------------------------------------------------*/
void
coreSetThreadTimer(CoreID module, CoreID thread, CoreTime time)
{
    if ((module < 0) | (module > _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "The unknown module '%d' tried to retime the thread '%d'.", module, thread);
    }
    else if ((thread < 0) | (thread >= _threads_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to retime the unknown thread '%d'.", module, thread);
    }
    else if ((_threads[thread].owner != module) & (thread != THREAD_MAIN))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to retime the not owned thread '%d'.", module, thread);
    }
    else if (_threads[thread].state == THREAD_DEAD)
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to retime the dead thread '%d'.", module, thread);
    }
    else if (_threads[thread].state != THREAD_WILLTERM)
    {
        SDL_mutexP(_threads[thread].lock);
        _threads[thread].timer = time;
        SDL_mutexV(_threads[thread].lock);
    }
}

/*----------------------------------------------------------------------------*/
CoreID
coreRequireThreadSlot(CoreID module, CoreID thread)
{
    if ((module < 0) | (module > _modules_nb))
    {
        shellPrintf(LEVEL_ERROR, "The unknown module '%d' tried to require a slot in thread '%d'.", module, thread);
        return CORE_INVALID_ID;
    }
    else if ((thread < 0) | (thread >= _threads_nb))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to require a slot in the unknown thread '%d'.", module, thread);
        return CORE_INVALID_ID;
    }
    else if ((_threads[thread].owner != module) & (!_threads[thread].public))
    {
        shellPrintf(LEVEL_ERROR, "Module '%d' tried to require a slot in the private thread '%d'.", module, thread);
        return CORE_INVALID_ID;
    }
    else if (_modules[module].thread_cb == NULL)
    {
        shellPrintf(LEVEL_ERROR, "Module '%s' required a thread slot with no callback set.", String_get(_modules[module].name));
        return CORE_INVALID_ID;
    }
    else
    {
        /*TODO: maybe check that this module doesn't already have a slot in this thread*/
        
        SDL_mutexP(_threads[thread].lock);
        if (_threads[thread].slots_nb == 0)
        {
            _threads[thread].slots = MALLOC(sizeof(CoreID));
        }
        else
        {
            _threads[thread].slots = REALLOC(_threads[thread].slots, sizeof(CoreID) * (_threads[thread].slots_nb + 1));
        }
        _threads[thread].slots[_threads[thread].slots_nb++] = module;
        SDL_mutexV(_threads[thread].lock);
        
        return thread;
    }
}

/*----------------------------------------------------------------------------*/
CoreID
coreGetThreadID(const char* name)
{
    CoreID i;
    String s;
    
    if (name == NULL)
    {
        return THREAD_MAIN;
    }
    
    s = String_new(name);
    
    for (i = 0; i < _threads_nb; i++)
    {
        if (String_equal(s, _threads[i].name))
        {
            String_del(s);
            return i;
        }
    }
    
    String_del(s);
    return CORE_INVALID_ID;
}
