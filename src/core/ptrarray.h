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
 *   Dynamic array of pointers                                                *
 *                                                                            *
  ***************************************************************************/

#ifndef _SW_CORE_PTRARRAY_H_
#define _SW_CORE_PTRARRAY_H_ 1

/*!
 * \file
 * \brief Pointer arrays.
 *
 * Pointer arrays are used to store pointers. Their advantages over a standard array are:
 *  \li Automatic allocation (with growing and shrinking).
 *  \li Automatic calls to custom delete function (pointers will be passed to this function when removed).
 *  \li Pointer sorting by the mean of a custom (or a default) compare function.
 *  \li Dichotomic searches in sorted arrays.
 *
 * For this description, we will take the example of an array of pointers of 'String' type (see string.h file).
 * To create the array, you can do:
 *  \code
 *  PtrArray array_default = PtrArray_new();
 *      //or
 *  PtrArray array = PtrArray_newFull(10, 5, (PtrFunc)String_del, (PtrCmpFunc)String_cmp);
 *  \endcode
 *
 * The first array will just contains pointers; strings will not be freed on removing and sorting is disabled.
 * The second array is better controlled. 10 and 5 are memory management parameters (see the PtrArray_newFull
 * detailled description). \a String_del is the delete function that will be called on pointer removing.
 * \a String_cmp is the function that will be used for sorting and sorted searches. These two functions can be
 * NULL if you don't want respectively freeing or sorting functionalities.
 *
 * You can add, remove or sort strings in the array by using dedicated functions.
 *
 * There are three ways to go through an array. Suppose we want to pass the strings to a function like:
 *  \code
 *  void foo(String s);
 *  \endcode
 * First, you can use positions, starting at 0:
 *  \code
 *  PtrArrayPos i;
 *  for (i = 0; i < PtrArray_SIZE(array); i++)
 *  {
 *      foo(PtrArray_TYPEDELEM(array, i, String));
 *  }
 *  \endcode
 * Second, you can use iterators:
 *  \code
 *  PtrArrayIterator it;
 *  for (it = PtrArray_START(array); it != PtrArray_STOP(array); it++)
 *  {
 *      foo((String)*it);
 *  }
 *  \endcode
 * Third, you can use the foreach commodity:
 * \code
 * PtrArray_foreach(array, foo);
 * \endcode
 *
 * \todo Try to replace sorted search with std functions.
 * \todo Avoid macros use (make object really private and move the declaration to types.h).
 */

/******************************************************************************
 *                                  Includes                                  *
 ******************************************************************************/
#include "main.h"

#include "core/types.h"

/******************************************************************************
 *                                  Typedefs                                  *
 ******************************************************************************/
/*! \brief Any function that can be called for a pointer. */
typedef void (*PtrFunc)(Ptr ptr);

/*! \brief Any function with data that can be called for a pointer. */
typedef void (*PtrFuncWithData)(Ptr ptr, void* data);

/*!
 * \brief Compare function applied on pointers.
 *
 * Beware that the parameters are not the pointers themselves but pointers on them.
 * Might return 0 if **ptr1==**ptr2, a positive value if **ptr1>**ptr2 or a negative value if **ptr1<**ptr2.
 */
typedef int (*PtrCmpFunc)(Ptr* ptr1, Ptr* ptr2);

/*! \brief Type for array's index and size. */
typedef Uint16 PtrArrayPos;

/*!
 * \brief Type for an array iterator.
 *
 * Pay attention: an iterator does not remain valid after an array's size modification.
 */
typedef Ptr* PtrArrayIterator;

/*! \brief Private structure for PtrArray. */
typedef struct
{
    Ptr* array;             /*!< The array. */
    PtrArrayPos len;        /*!< Current length (number of pointers in the array). */
    PtrArrayPos alloclen;   /*!< Really allocated size (in number of pointers). */
    PtrArrayPos allocrange; /*!< Number of allocations to do at the same time (in anticipation). */
    PtrFunc delfunc;        /*!< Delete function for the pointers of this array. */
    PtrCmpFunc cmpfunc;     /*!< Compare function for the pointers of this array. */
} pv_PtrArray;

/*!
 * \brief Type for pointer array.
 *
 * A pointer array handle dynamic allocation and some basic operations to store pointers.
 */
typedef pv_PtrArray* PtrArray;

/******************************************************************************
 *############################################################################*
 *#                            PtrArray Functions                            #*
 *############################################################################*
 ******************************************************************************/
/*!
 * \brief Create a new default pointer array.
 * 
 * \return The newly allocated array.
 */
PtrArray PtrArray_new(void);

/*!
 * \brief Create a fake array by copying another.
 *
 * Only the pointers will be copied (not the values pointed).
 * Such an array have no delete function (to not destroy values pointed).
 * \param base - Array copied.
 * \return The newly allocated array.
 */
PtrArray PtrArray_newFakeCopy(PtrArray base);

/*!
 * \brief Create a fully controlled pointer array.
 *
 * \param startsize - Initial size of the array (allocated size, number of pointers remains 0).
 * \param sizebloc - Number of elements to pre-allocate at the same time when needed (this avoid frequent reallocations).
 * \param delfunc - Function called when a pointer is removed from the array, can be NULL.
 * \param cmpfunc - Function used to compare two pointers (used for sorting). If NULL, a default pointer comparison will be used.
 * \return The newly allocated array.
 */
PtrArray PtrArray_newFull(PtrArrayPos startsize, PtrArrayPos sizebloc, PtrFunc delfunc, PtrCmpFunc cmpfunc);

/*!
 * \brief Delete all pointers and free all memory used by the array.
 *
 * If a delfunc was provided, it will be called for each pointer.
 * \param array - The array.
 */
void PtrArray_del(PtrArray array);

/*!
 * \brief Get the size of the array.
 *
 * Elements in an array are known to be between positions 0 and size-1 included.
 * \param _array_ - (PtrArray) The array.
 * \return (PtrArrayPos) Number of pointers in the array.
 */
#define PtrArray_SIZE(_array_) ((_array_)->len)

/*!
 * \brief Get an iterator on the first array element.
 * 
 * \param _array_ - (PtrArray) The array.
 * \return (PtrArrayIterator) Iterator on the first array element.
 */
#define PtrArray_START(_array_) ((_array_)->array)

/*!
 * \brief Get an iterator on the first element that is outside the array (to the "right").
 *
 * The "stop" element is virtual and must not be used. It is just useful to know when to
 * stop in an array iteration.
 * \param _array_ - (PtrArray) The array.
 * \return (PtrArrayIterator) Iterator on the "stop" element.
 */
#define PtrArray_STOP(_array_) ((_array_)->array + (_array_)->len)

/*!
 * \brief Get an element in the array, given its position.
 *
 * The first position is 0.
 * \param _array_ - (PtrArray) The array.
 * \param _pos_ - (PtrArrayPos) Wanted index.
 * \return (Ptr) The pos-th pointer in the array.
 */
#define PtrArray_ELEM(_array_,_pos_) ((_array_)->array[_pos_])

/*!
 * \brief Get an element in the array, given its position and cast the pointer type.
 *
 * \param _array_ - (PtrArray) The array.
 * \param _pos_ - (PtrArrayPos) Wanted index.
 * \param _type_ - Type to cast the pointer.
 * \return (_type_) The pos-th pointer in the array cast to the specified type.
 */
#define PtrArray_TYPEDELEM(_array_,_pos_,_type_) ((_type_)((_array_)->array[_pos_]))

/*!
 * \brief Remove all pointers in an array.
 *
 * If a delfunc was provided, it will be called for each pointer.
 * \param array - The array.
 */
void PtrArray_clear(PtrArray array);

/*!
 * \brief Add a pointer to the end of an array.
 *
 * \param array - The array.
 * \param ptr - Pointer to add.
 */
void PtrArray_append(PtrArray array, Ptr ptr);

/*!
 * \brief Add a pointer to the beginning of an array.
 *
 * All array elements will be move forward to make room (preserving order).
 * \param array - The array.
 * \param ptr - Pointer to add.
 */
void PtrArray_prepend(PtrArray array, Ptr ptr);

/*!
 * \brief Add a pointer to the beginning of an array.
 *
 * Replaced first element will be moved at the array's end (order can be lost).
 * \param array - The array.
 * \param ptr - Pointer to add.
 */
void PtrArray_prependFast(PtrArray array, Ptr ptr);

/*!
 * \brief Insert a pointer to a given position in the array.
 *
 * If pos is outside the array range, it will be either appended or prepended.
 * Elements after the insert position will be moved forward (preserving order).
 * \param array - The array.
 * \param ptr - Pointer to add.
 * \param pos - Position where to add the pointer.
 */
void PtrArray_insert(PtrArray array, Ptr ptr, PtrArrayPos pos);

/*!
 * \brief Insert quickly a pointer to a given position in the array.
 *
 * If pos is outside the array range, it will be either appended or prepended.
 * Replaced element will be moved at the end of the array (order not preserved).
 * \param array - The array.
 * \param ptr - Pointer to add.
 * \param pos - Position where to add the pointer.
 */
void PtrArray_insertFast(PtrArray array, Ptr ptr, PtrArrayPos pos);

/*!
 * \brief Insert a pointer to a sorted array.
 *
 * This will use the cmpfunc provided (or the default one).
 * The array is supposed to be sorted (no check performed).
 * \param array - The array.
 * \param ptr - Pointer to add.
 */
void PtrArray_insertSorted(PtrArray array, Ptr ptr);

/*!
 * \brief Remove a pointer from the array.
 *
 * If a delfunc was provided, it will be called for the removed pointer.
 * All elements after the removed one will be moved down (order is preserved).
 * If the pointer isn't found, it will be silently ignored.
 * \param array - The array.
 * \param ptr - The pointer to remove.
 */
void PtrArray_remove(PtrArray array, Ptr ptr);

/*!
 * \brief Remove quickly a pointer from the array.
 *
 * If a delfunc was provided, it will be called for the removed pointer.
 * The last element of the array will replace the removed one (order may be lost).
 * If the pointer isn't found, it will be silently ignored.
 * \param array - The array.
 * \param ptr - The pointer to remove.
 */
void PtrArray_removeFast(PtrArray array, Ptr ptr);

/*!
 * \brief Remove a pointer from the array, given an iterator on it.
 *
 * If a delfunc was provided, it will be called for the removed pointer.
 * All elements after the removed one will be moved down (order is preserved).
 * \param array - The array.
 * \param it - Iterator pointing on the element to remove, can be NULL.
 */
void PtrArray_removeIt(PtrArray array, PtrArrayIterator it);

/*!
 * \brief Remove quickly a pointer from the array, given an iterator on it.
 *
 * If a delfunc was provided, it will be called for the removed pointer.
 * The last element of the array will replace the removed one (order may be lost).
 * \param array - The array.
 * \param it - Iterator pointing on the element to remove.
 */
void PtrArray_removeItFast(PtrArray array, PtrArrayIterator it);

/*!
 * \brief Remove a range of pointers in the array.
 *
 * If a delfunc was provided, it will be called for the removed pointers.
 * All elements after the removed ones will be moved down (order is preserved).
 * \param array - The array.
 * \param start - Start position of the range.
 * \param end - End position of the range.
 */
void PtrArray_removeRange(PtrArray array, PtrArrayPos start, PtrArrayPos end);

/*!
 * \brief Remove a range of pointers in the array.
 *
 * If a delfunc was provided, it will be called for the removed pointers.
 * The last elements of the array will replace the removed ones (order may be lost).
 * \param array - The array.
 * \param start - Start position of the range.
 * \param end - End position of the range.
 */
void PtrArray_removeRangeFast(PtrArray array, PtrArrayPos start, PtrArrayPos end);

#define PtrArray_removePos(_array_,_pos_) PtrArray_removeRange(_array_,_pos_,_pos_)

#define PtrArray_removePosFast(_array_,_pos_) PtrArray_removeRangeFast(_array_,_pos_,_pos_)

/*!
 * \brief Sort an array.
 *
 * This will use the cmpfunc provided (or the default one).
 * \param array - The array.
 */
void PtrArray_sort(PtrArray array);

/*!
 * \brief Sort a part of an array.
 *
 * This will use the cmpfunc provided (or the default one).
 * \param array - The array.
 * \param start - Start position of the range to sort.
 * \param end - End position of the range to sort.
 */
void PtrArray_sortRange(PtrArray array, PtrArrayPos start, PtrArrayPos end);

/*!
 * \brief Find an element equal to the given one in an array.
 *
 * This will use the cmpfunc provided (or the default one).
 * \param array - The array.
 * \param base - Pointer to compare array's elements with.
 * \return An iterator pointing on the element found, NULL if not found.
 */
PtrArrayIterator PtrArray_find(PtrArray array, Ptr base);

/*!
 * \brief Find an element equal to the given one in an sorted array.
 *
 * This will use the cmpfunc provided (or the default one).
 * The array is supposed to be sorted (no check performed).
 * \param array - The array.
 * \param base - Pointer to compare array's elements with.
 * \return An iterator pointing on the element found, NULL if not found.
 */
PtrArrayIterator PtrArray_findSorted(PtrArray array, Ptr base);

/*!
 * \brief Call a function for each pointer of an array.
 *
 * The given function is called for each element, following their order in the array.
 * \param array - The array.
 * \param func - The function to call.
 */
void PtrArray_foreach(PtrArray array, PtrFunc func);

/*!
 * \brief Call a function with data for each pointer of an array.
 *
 * The given function is called for each element, following their order in the array.
 * \param array - The array.
 * \param func - The function to call.
 * \param data - Data to pass to \a func .
 */
void PtrArray_foreachWithData(PtrArray array, PtrFuncWithData func, void* data);

#endif
