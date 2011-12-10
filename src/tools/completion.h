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
 *   Completion tool                                                          *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_COMPLETION_H_
#define _SW_COMPLETION_H_ 1

/*!
 * \file
 * \brief Completion tool.
 *
 * The completion tool is used to complete words, given a list of possibilities.
 * Completion is performed with 2 objects:
 *  \li A CompletionList that holds all possibilities.
 *  \li A CompletionResults, that contains all possible results for a completion query.
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"
#include "core/types.h"
#include "core/ptrarray.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*!
 * \brief Abstract type for a completion list.
 */
typedef PtrArray CompletionList;

typedef struct pv_CompletionResults pv_CompletionResults;

typedef pv_CompletionResults* CompletionResults;

/******************************************************************************
 *############################################################################*
 *#                        CompletionResults functions                       #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new completion results object.
 *
 * \return Newly allocated completion results object.
 */
CompletionResults CompletionResults_new(void);

/*!
 * \brief Destroy a completion results object.
 *
 * \param results - The results object.
 */
void CompletionResults_del(CompletionResults results);

/*!
 * \brief Get the next result in a completion results object.
 *
 * This function can be called several times to get all the possibilities.
 * \param results - The results object.
 * \param cycle - If TRUE and if there are results, cycle through these results, never returning NULL.
 *                If FALSE, NULL will be returned after the last result.
 * \return The result, NULL if there are no results.
 */
String CompletionResults_getNextResult(CompletionResults results, Bool cycle);

/******************************************************************************
 *############################################################################*
 *#                          CompletionList functions                        #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new completion list.
 *
 * \return Newly allocated completion list.
 */
CompletionList CompletionList_new(void);

/*!
 * \brief Destroy a completion list.
 *
 * \param list - The list.
 */
void CompletionList_del(CompletionList list);

/*!
 * \brief Add an element to a completion list.
 *
 * If \a elem already exists in the list, it will not be added.
 * \param list - The list.
 * \param elem - Element to add, will be copied.
 */
void CompletionList_add(CompletionList list, String elem);

/*!
 * \brief Perform a completion query.
 *
 * \param list - The completion list.
 * \param prefix - Prefix used for the query.
 * \param results - An allocated results object. Will be filled with all results that match the prefix query.
 */
void CompletionList_query(CompletionList list, const char* prefix, CompletionResults results);

#endif
