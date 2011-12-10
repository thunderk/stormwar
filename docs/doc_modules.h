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
 *   Documentation : Mod reference                                            *
 *                                                                            *
  ***************************************************************************/

/*!
 * \file
 * \brief Documentation : \ref doc_modules
 */

/*!
 * \page doc_modules Source modules
 *
 *  \section CORE Core engine
 *      \subsection core Module "core"
 *          \par Shell functions
 * \code
 *  void lsmod()
 *      // Print a list of all declared modules.
 *  void lsfunc(string)
 *      // Print a list of shell functions for the module named param_1
 *  void lsres()
 *      // Print a list of all declared ressources.
 *  void echores(string)
 *      // Print the value of the ressource named param_1
 *  void pause()
 *      // Send a pause signal to all modules.
 *  void resume()
 *      // Send a resume signal to all modules.
 * \endcode
 *          \par Threads
 *              \li "main" - The main thread (public).
 *          \par Ressources
 *              None
 *
 * <hr>
 *      \subsection shell Module "shell"
 *          \par Shell functions
 * \code
 *  void quit()
 *      // Quit the program.
 *  void print(string)
 *      // Print the string param_1
 *  string cat(string, string)
 *      // Append param_2 at the end of param_1 and return the result.
 *  void setlevel(int)
 *      // Set the shell level.
 * \endcode
 *          \par Threads
 *              None
 *          \par Ressources
 *              None
 *
 * <hr>
 *      \subsection i18n Module "i18n"
 *          Nothing
 *
 * <hr>
 * <hr>
 * \section GRAPH Graphical engine
 *  TODO
 */
