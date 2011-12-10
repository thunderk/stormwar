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

#ifndef _SW_CORE_CORE_H_
#define _SW_CORE_CORE_H_ 1

/*!
 * \file
 * \brief This is the core engine.
 *
 * The core engine is a fondamental part of the program.
 * It allows the declaration of virtual modules (handled at run time).<br>
 * This engine can manage, for each module :
 *  \li Core events catching
 *  \li Datas loading from the data tree
 *  \li Shell functions (that can be called through the builtin shell)
 *  \li Preferences (that will be saved to a file)
 *  \li Resources (that are shared between the modules)
 *  \li Threads (with slots for other modules)
 *
 * A nice initialization would be:
 * \code
 *  coreInit();
 *  // this will prepare the core to receive modules information.
 *
 *  // ...
 *  // call here init functions of modules to let them plug into the core.
 *
 *  coreStart();
 *  // this will load preferences then start the threads.
 * \endcode
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"

#include "core/var.h"
#include "tools/completion.h"

/******************************************************************************
 *                                 Constants                                  *
 ******************************************************************************/
/*!
 * \brief Invalid identifier (an id which is not used by anything valid in the core).
 */
#define CORE_INVALID_ID -1

/******************************************************************************
 *                                   Types                                    *
 ******************************************************************************/
/*!
 * \brief Enumeration of core events.
 */
typedef enum
{
    CORE_READY,             /*!< Core is ready to start the thread, preferences have been loaded. */
    CORE_DATALOADING,       /*!< Core will start loading datas just after this event. */
    CORE_DATALOADED,        /*!< Core has finished data loading. */
    CORE_PAUSE,             /*!< Core will be put in pause mode after this event. */
    CORE_RESUME             /*!< Core will resume from pause mode after this event. */
} CoreEvent;

/*!
 * \brief Callback for core events.
 *
 * This will be called only by the main thread.
 * \param event - The event.
 */
typedef void (*CoreEventCallback) (CoreEvent event);

/*!
 * \brief Callback for shell function calls.
 *
 * This function will be called when the shell need to execute a function declared by a module.
 * It can be called from any thread.
 * \param func - The function called, with parameters filled. The receiver can set the return variable \a ret.
 */
typedef void (*CoreShellCallback) (ShellFunction* func);

/*!
 * \brief Callback for data loading.
 *
 * This function will be called when datas are loaded from the data tree.
 * This will be called only by main thread.
 * \param datas - The datas variable. It is a VAR_ARRAY named after the module name.
 */
typedef void (*CoreDatasCallback) (Var datas);

/*!
 * \brief Callback for preferences.
 *
 * This function will be called when the preferences are loaded from a file.
 * This will be called only by main thread.
 * \param prefs - The preferences variable. It is a VAR_ARRAY. The owner can add, delete and modify
 *                any fields. The array will be saved in the preferences file as is.
 */
typedef void (*CorePrefsCallback) (Var prefs);

/*!
 * \brief Callback for a shared resource modification.
 * 
 * This function will be called when a resource is watched and has been changed.
 * This can be called by any thread; never use \ref temp_corevar in a callback.
 * \param id - Resource id.
 * \param value - Resource value, will not be valid anymore after the callback returned. \readonly
 */
typedef void (*CoreResourceCallback) (CoreID id, Var value);

/*!
 * \brief Callback for periodical threads.
 *
 * This function will be called periodically by the thread where it is inserted.
 * \param threadid - Thread identifier.
 * \param duration - Delay between the last call and this one.
 */
typedef void (*CoreThreadCallback) (volatile CoreID threadid, CoreTime duration);

/******************************************************************************
 *                             Global variables                               *
 ******************************************************************************/
/*!
 * \brief Temporary variable.
 *
 * This variable is always initialized, not checked and not used by the core engine.
 * It can be useful to pass a parameter to a core function quickly. Pay attention to
 * multi-threading (this variable isn't thread-safe).
 *
 * Mainly for use like this:
 * \code
 *      Var_setInt(temp_corevar, 50);
 *      coreSetResourceValue(mod_id, resource_id, temp_corevar);
 * \endcode
 * It is only a convenience to avoid Var_new and Var_del calls and must be used safely.
 */
extern Var temp_corevar;

/******************************************************************************
 *############################################################################*
 *#                                 Functions                                #*
 *############################################################################*
 ******************************************************************************/
/******************************************************************************
 *                               General purpose                              *
 ******************************************************************************/
/*!
 * \brief Initialize the core engine.
 */
void coreInit(void);

/*!
 * \brief Destroy the core engine.
 */
void coreUninit(void);

/*!
 * \brief Start the core engine's main thread.
 *
 * This must be called from the main thread, generally just after coreInit().
 * This function will return only after a call to coreStop() has been done.
 */
void coreStart(void);

/*!
 * \brief Stop the core engine's main thread.
 *
 * This function will return immediately but some time may be needed to wait
 * for the other threads to end.
 * The coreStart() function will return after this call as soon as possible.
 * This can be called by any thread, before coreUninit() call.
 */
void coreStop(void);

/*!
 * \brief Throw CORE_PAUSE event to the modules.
 */
void corePause(void);

/*!
 * \brief Throw CORE_RESUME event to the modules.
 */
void coreResume(void);

/*!
 * \brief Launch data loading process.
 *
 * This must be done after all modules are plugged into the core.
 * In case TRUE is returned (failure), the last data tree loaded is left unchanged.
 * \param modname - Name of the mod to load.
 * \return TRUE if there was an error.
 */
Bool coreLoadData(String modname);

/*!
 * \brief Find a data file in the mod folder.
 *
 * This doesn't check if the file exists, only return its potential path.
 * \param path - Relative path to the data file, will be prepended with the mod's path.
 */
void coreFindData(String path);

/******************************************************************************
 *                                    Module                                  *
 ******************************************************************************/
/*!
 * \brief Declare a virtual module.
 *
 * \param name - Module name.
 * \param event_cb - Core event callback, can be NULL if the module doesn't need to catch such events.
 * \param datas_cb - Datas callback, can be NULL if the module doesn't require datas from the data tree.
 * \param shell_cb - Shell function callback, can be NULL if no shell function will be declared.
 * \param prefs_cb - Preferences callback, NULL if no preferences are required by this module.
 * \param res_cb - Resources callback, can be NULL if no watchers will be added by this module.
 * \param thread_cb - Thread callback, can be NULL if no slot will be inserted in threads by this module.
 * \return The module identifier, CORE_INVALID_ID on error.
 */
CoreID coreDeclareModule(const char* name, CoreEventCallback event_cb, CoreDatasCallback datas_cb, CoreShellCallback shell_cb, CorePrefsCallback prefs_cb, CoreResourceCallback res_cb, CoreThreadCallback thread_cb);

/******************************************************************************
 *                                 Shell function                             *
 ******************************************************************************/
/*!
 * \brief Declare a shell function inside a module.
 *
 * \param module - The owner module.
 * \param name - Function name.
 * \param rettype - Type of the returned value.
 * \param nbparams - Number of parameters.
 * \param ... - Type of the parameters (a list of \a nbparams VarType).
 * \return The function identifier, CORE_INVALID_ID on error.
 */
CoreID coreDeclareShellFunction(CoreID module, const char* name, VarType rettype, int nbparams, ...);

/******************************************************************************
 *                                   Ressources                               *
 ******************************************************************************/
/*!
 * \brief Create a shared resource.
 *
 * \param owner - The owner module.
 * \param name - Resource name.
 * \param type - Resource type.
 * \param world_writable - Specifies if other module than the owner can modify this resource.
 * \return The resource identifier, CORE_INVALID_ID on error.
 */
CoreID coreCreateResource(CoreID owner, const char* name, VarType type, Bool world_writable);

/*!
 * \brief Add a watcher to look after a resource changes.
 *
 * The res_cb provided in module declaration will be called whenever the watched resource has changed.<br>
 * If the resource hasn't yet been created, the returned identifier will still be valid.
 * \param module - The module that require this.
 * \param name - Resource name.
 * \return The watched resource identifier, CORE_INVALID_ID on error.
 */
CoreID coreAddResourceWatcher(CoreID module, const char* name);

/*!
 * \brief Get a resource identifier by its name.
 *
 * \param name - Resource name.
 * \return The resource identifier, CORE_INVALID_ID if the resource was not found.
 */
CoreID coreGetResourceID(const char* name);

/*!
 * \brief Set a resource value.
 *
 * This will call the callbacks of all watchers.
 * \param module - Module that wants to set the resource, can be CORE_INVALID_ID if the resource is world writable.
 * \param resource - The resource to set.
 * \param value - New value (will be copied).
 */
void coreSetResourceValue(CoreID module, CoreID resource, Var value);

/*!
 * \brief Ask a resource sending by callback.
 *
 * \param module - The module.
 * \param resource - The resource required.
 */
void coreAskResourceSending(CoreID module, CoreID resource);

/******************************************************************************
 *                              Completion list                               *
 ******************************************************************************/
/*!
 * \brief Retrieve a list that can be used for completion.
 *
 * This returns a sorted array of strings. These strings are the names of things declared in the core
 * (modules, shell functions, resources...).
 * All names are declared only once.
 * \return A PtrArray that contains pointers of 'String' type. This pointer will remain valid. \readonly
 */
CompletionList coreGetCompletionList();

/******************************************************************************
 *                                  Threads                                   *
 ******************************************************************************/
/*!
 * \brief Create a new thread.
 *
 * By default, the timer will be set to 50 milliseconds.
 * The returned value must be stored in a \a volatile variable to be used safely.
 * \param owner - Module that will own the thread.
 * \param name - Name of the thread.
 * \param public - Is the thread public?
 * \param id - A pointer to fill with the thread identifier, CORE_INVALID_ID if there was an error.
 */
void coreCreateThread(CoreID owner, const char* name, Bool public, volatile CoreID* id);

/*!
 * \brief Try to end a thread softly.
 *
 * If the thread doesn't end after a certain time, it will be killed.
 * Only the owner can term a thread direclty.
 * \param module - The module that asks this.
 * \param thread - The thread to term.
 */
void coreTermThread(CoreID module, CoreID thread);

/*!
 * \brief Kill a thread badly.
 *
 * Only the owner can kill a thread.
 * \param module - The module that asks the kill.
 * \param thread - The thread to kill.
 */
void coreKillThread(CoreID module, CoreID thread);

/*!
 * \brief Pause a thread.
 *
 * Only the owner can pause a thread.
 * \param module - The module that asks the pause.
 * \param thread - The thread to pause.
 */
void corePauseThread(CoreID module, CoreID thread);

/*!
 * \brief Resume a paused thread.
 *
 * Only the owner can resume a thread.
 * \param module - The module that asks the resume.
 * \param thread - The thread to resume.
 */
void coreResumeThread(CoreID module, CoreID thread);

/*!
 * \brief Set the timer of a thread.
 *
 * The timer is the minimum delay between two calls to the callback.
 * This can only be done by the owner, except for the main thread, which can be changed by anyone.
 * \param module - The module that asks the change.
 * \param thread - The thread to change.
 * \param time - Release time in milliseconds, 0 for no release at all (callback will be called as soon as possible).
 */
void coreSetThreadTimer(CoreID module, CoreID thread, CoreTime time);

/*!
 * \brief Require a thread slot.
 *
 * The slot allows the module's thread callback to be called periodically (at the timer of the thread).
 * An owner already have a slot allocated at thread creation if the thread_cb was not NULL.
 * This can only be done for public threads.
 * \param module - The module that requires the slot.
 * \param thread - A public thread.
 * \return The thread id (the one passed in parameter) or CORE_INVALID_ID if there was an error.
 */
CoreID coreRequireThreadSlot(CoreID module, CoreID thread);

/*!
 * \brief Retrieve a thread ID by its name.
 *
 * \param name - The thread name (NULL or "main" will return the main thread id).
 * \return The thread id required, CORE_INVALID_ID if the required thread wasn't found.
 */
CoreID coreGetThreadID(const char* name);

#endif
