/*----------------------------------------------------------
 *				HTBLA-Leonding / Class: <your class>
 * ---------------------------------------------------------
 * Exercise Number: S02
 * Title:			List Iterator Interface
 * Author:			S. Schraml
 * ----------------------------------------------------------
 * Description:
 * The declaration of a list iterator abstract data type.
 * ----------------------------------------------------------
 */

#ifndef ___LIST_ITERATOR__H
#define ___LIST_ITERATOR__H

#include <stdbool.h>

#include "list_types.h"

/**
 * Obtains ('creates') and provides a 'new' list iterator instance for the given list.
 * The provided iterator initially points to the head node of the list.
 * 
 * Any iterator obtained via this function MUST be released using
 * function `release_iterator()`.
 * 
 * Note: This function does not make any assumptions 
 * about how list components, esp. nodes, are allocated.
 * 
 * @param list The list for which the iterator is obtained.
 * @return The list iterator instance or 0, if no list iterator could by instantiated.
 */
IntListIterator list_it_obtain(IntList list);

/**
 * Releases a list iterator that was obtained earlier via function `list_it_obtain`.
 * Released list iterators MUST NOT be used anymore.
 * 
 * Note: The implementation of this function does not make any assumptions
 * about the allocation method of list iterator elements, but MUST match the implementation
 * of function `list_it_obtain` as its inverse function.
 * 
 * @param p_it The pointer to the list iterator to release. The value of the pointer
 * is set to 0, if the list iterator was successfully released, otherwise it is left untouched.
 */
void list_it_release(IntListIterator* p_it);

/**
 * Determines whether or not the given list iterator is valid.
 * 
 * @param it The list iterator to evaluate.
 * @return `True` if the list iterator is valid, false otherwise.
 */
bool list_it_is_valid(IntListIterator it);

/**
 * Proceeds the list iterator to the next list element, if possible.
 * 
 * @param it The list iterator to evaluate.
 * @return `True` if the list iterator could proceed to the next list node, `false` otherwise.
 */
bool list_it_next(IntListIterator it);

/**
 * Proceeds the list iterator to the previous list element, if possible.
 * 
 * @param it The list iterator to evaluate.
 * @return `True` if the list iterator could proceed to the previous list node, `false` otherwise.
 */
bool list_it_previous(IntListIterator it);

/**
 * Provides the value of the node the list iterator currently points to.
 * 
 * @param it The list iterator to evaluate.
 * @return The value of the current list node under the iterator.
 */
int list_it_get(IntListIterator it);

/**
 * Applies the given value to the node the list iterator currently points to.
 * 
 * @param it The list iterator to evaluate.
 * @param value The value to set to the current list node under the iterator.
 */
void list_it_set(IntListIterator it, int value);

/**
 * Inserts the given value after the node under the iterator and proceeds the iterator to the
 * inserted node.
 * 
 * @param it The list iterator to evaluate.
 * @param value The value to insert.
 */
// void list_it_insert(IntListIterator it, int value);

/**
 * Removes the node under the iterator and proceeds the iterator to the 'next' node.
 * 
 * @param it The list iterator to evaluate.
 */
// void list_it_remove(IntListIterator it);

#endif
