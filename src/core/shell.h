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
 *   Global game shell                                                        *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_SHELL_H_
#define _SW_CORE_SHELL_H_ 1

/*!
 * \file
 * \brief This is the main shell.
 *
 * Shell is used to print information and to execute commands (through shell functions
 * declared by the core). Shell information can be logged to a file. There are several
 * levels of information.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"
#include "core/macros.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Level of importance used in a shell.
 */
typedef enum
{
    LEVEL_USER = 0,             /*!< Messages for the user */
    LEVEL_ERROR = 1,            /*!< Error messages        */
    LEVEL_INFO = 2,             /*!< Game information      */
    LEVEL_DEBUG = 3,            /*!< Normal debug mode     */
    LEVEL_HARDDEBUG = 4,        /*!< Intensive debug       */
    LEVEL_ERRORNOSTACK = 5,     /*!< Error message without stack printing */
    LEVEL_ERRORSTACK = 6,       /*!< Message to push into the error stack */
    LEVEL_NB = 7                /*!< Number of levels      */
} ShellLevel;

/*!
 * \brief Type for a print callback.
 */
typedef void (*ShellCallbackPrint) (ShellLevel level, const char* st);

/******************************************************************************
 *############################################################################*
 *#                              Shell functions                             #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Set the callback that will be called for output printing.
 *
 * \param printcb - Callback function.
 */
void shellSetPrintCallback(ShellCallbackPrint printcb);

/*!
 * \brief Start to log the shell output in a file.
 *
 * \param file - File where the log will be appended.
 */
void shellStartLogging(char* file);

/*!
 * \brief Stop logging to a file.
 *
 * This is done automatically at uninit.
 */
void shellStopLogging(void);

/*!
 * \brief Set the limit of displaying messages.
 *
 * Messages over this limit will be ignored.
 * \param level - New shell limit.
 */
void shellSetLevel(ShellLevel level);

/*!
 * \brief Remove the last message pushed in the error stack.
 *
 * The error stack is used to make error messages more easy (to indicate the location of an error).
 */
void shellPopErrorStack(void);

/*!
 * \brief Print information in the shell
 *
 * \param message - Message to print in the shell
 * \param level   - Level of importance of the message
 */
void shellPrint(ShellLevel level, const char* message);

/*!
 * \brief Print formatted information in the shell
 *
 * The format string is in the standard printf format.
 * \param level  - Level of importance of the message
 * \param format - Message format
 * \param ...    - Format arguments
 */
void shellPrintf(ShellLevel level, const char* format, ...) CHECK_ARGS_PRINTF(2, 3);
 
/*!
 * \brief Parse a function call and call this function.
 *
 * \param com - The string containing the function call.
 * \param retvar - An initialized variable to fill with the value returned by the function call.
 * \return TRUE if there was an error.
 */
Bool shellExecFunction(String com, Var retvar);

/*!
 * \brief Execute a command
 *
 * The command is parsed, checked and executed.
 * \param command - The command string
 */
void shellExec(String command);

#endif
