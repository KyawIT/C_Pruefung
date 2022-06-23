/*----------------------------------------------------------
 *				HTBLA-Leonding / Class: <your class>
 * ---------------------------------------------------------
 * Exercise Number: S02
 * Title:			Doubly Linked List implementation
 * Author:			Kyaw
 * ----------------------------------------------------------
 * Description:
 * Implementation of a doubly linked list.
 * ----------------------------------------------------------
 */

/*
  Implementation notes:

  1) The 'ListData' struct of this linked list SHALL have
     - a pointer to the head node, 
     - a pointer to the tail node
     - and the size of the list 
    as members!

  2) List list, node, and iterator allocation: 
     Use functions `mem_alloc(…)` and `mem_free(…)`
     declared in `allocator.h`. DO NOT use `malloc(…)` and `free(…)` directly
     as unit tests will fail.

  3) Use 'limits.h' to get maximum and minimum values for numeric types, if needed.

  4) Implement 'list_iterator.h' in this file as well.

  5) Avoid code duplication wherever (reasonably) possible.
     This is valid for implementation of similar functions as well
     as for reoccurring code patterns, such as list iteration.
     Nevertheless, aim for efficiency, e.g. `remove_all` shall traverse 
     the list only once and not use `remove` as defined, because
     the later would start at the beginning of the list for each iteration.
*/

#include "doubly_linked_list_with_iterator.h"
#include "allocator.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "list_iterator.h"

/** The type of list nodes */
typedef struct IntNodeData* IntNode;
/** The implementation of list node data */
struct IntNodeData{
    int data;
    struct IntNode* next;
    struct IntNode* prev;
};
/** The implementation of list data */
struct IntListData {
    IntNode head;
};
/** The implementation of list iterator data */


/* ===================================================================== */
/* private list functions */

/* abstract away and generalize also memory allocation for list nodes */
static IntNode list_obtain_node(int value) {
    IntNode node = alloc_mem(sizeof (struct IntNodeData));
    if(node != 0){
        node->data =value;
        node->next =0;
        node->prev=0;
    }
    return node;
}

static void list_release_node(IntNode* node) {
    free_mem(*node);
    *node = 0;
}

/* optional: implement a function for printing  the content of the list - may be useful for debugging */
void list_dump(char* prefix, IntList list) {
    if (list_is_valid(list)) {
        printf("\n%s: X->", prefix);
        IntNode cur = list->head;
        while (cur != 0) {
            printf("[%d]->", cur->data);
            cur = cur->next;
        }
        printf("@\n");
    } else {
        printf("List is invalid!\n");
    }
}

/* ===================================================================== */
IntList list_obtain(){
    IntList list = alloc_mem(sizeof(struct IntListData));
    if(list != 0){
        list->head = 0;
    }
    return list;
}

void list_release(IntList* p_list) {
    if (p_list != 0 && *p_list != 0) {
        list_clear(*p_list);
        free_mem(*p_list);
        *p_list = 0;
    }
}

bool list_is_valid(IntList list) {
    return list != 0;
}

bool list_is_empty(IntList list) {
    return !list_is_valid(list) || list->head == 0;
}

void list_clear(IntList list) {
    if (list_is_valid(list)) {
        IntNode cur = list->head;
        while (cur != 0) {
            IntNode node = cur;
            cur = cur->next;
            list_release_node(&node);
        }
        list->head = 0;
    }
}

int list_get_size(IntList list) {
    int count = 0;
    if (list_is_valid(list)) {
        IntNode cur = list->head;
        while (cur != 0) {
            count++;
            cur = cur->next;
        }
    }
    return count;
}

void list_insert(IntList list, int value) {
    list_insert_at(list, UINT_MAX, value);
}

void list_insert_at(IntList list, unsigned int index, int value) {
    if (list_is_valid(list)) {
        IntNode node = list_obtain_node(value);
        if (node != 0) {
            /* search the node before index until end of list */
            if (index != 0 && list->head != 0) {
                IntNode cur = list->head;
                for (unsigned int step = 0; cur->next != 0 && step < index - 1; step++) {
                    cur = cur->next;
                }
                node->next = cur->next;
                cur->next = node;
            } else { /* empty list or insert at beginning*/
                node->next = list->head;
                list->head = node;
            }
        }
    } /* list_is_valid(list) */
}

static IntNode list_get_at_or_last(IntNode start, unsigned int index, bool acceptLastAsDefault) {
    IntNode cur = start;
    /* search the node at index until end of list */
    unsigned int step = 0;
    for (;cur != 0 && cur->next != 0 && step < index; step++) {
        cur = cur->next;
    }
    return (acceptLastAsDefault || step == index) ? cur : 0;
}

int list_get_at(IntList list, unsigned int index) {
    int value = 0;
    if (list_is_valid(list)) {
        IntNode node = list_get_at_or_last(list->head, index, false);
        if (node != 0) {
            value = node->data;
        }
    }
    return value;
}
static IntNode list_remove_next(IntList list, IntNode start, int value) {
    IntNode cur = start != 0 ? start : (list_is_valid(list) ? list->head : 0);
    IntNode prev = 0;
    IntNode next_start = 0;
    /* search the node with ‘value’ */
    while (cur != 0 && cur->data != value) {
        prev = cur;
        cur = cur->next;
    }
    if (cur != 0) {/* found a node to remove */
        next_start = prev;
        if (prev != 0) { /* it’s not the head because start node is either head or a previous node */
            prev->next = cur->next;
        } else { /* remove the head */
            list->head = cur->next;
            next_start = list->head;
        }
        list_release_node(&cur);
    }
    return next_start;
}

void list_remove(IntList list, int value) {
    list_remove_next(list, 0, value);
}

void list_remove_all(IntList list, int value) {
    IntNode next_start = list_remove_next(list, 0, value);
    while (next_start != 0) {
        next_start = list_remove_next(list, next_start, value);
    }
}

int list_remove_at(IntList list, unsigned int index) {
    int removed_value = 0;
    if (list_is_valid(list) && list->head != 0) {
        IntNode node = 0;
        if (index != 0) {
            /* search the node at index */
            IntNode prev = list_get_at_or_last(list->head, index - 1, false);
            if (prev != 0 && prev->next != 0) {
                node = prev->next;
                prev->next = prev->next;
            }
        } else { /* remove the head node */
            node = list->head;
            list->head = node->next;
        }
        if (node != 0) {
            removed_value = node->data;
            list_release_node(&node);
        }
    } /* list_is_valid(list) */
    return removed_value;
}

bool list_contains(IntList list, int value) {
    IntNode cur = 0;
    if (list_is_valid(list)) {
        cur = list->head;
        while (cur != 0 && cur->data != value) {
            cur = cur->next;
        }
    }
    return (cur != 0);
}

void list_append (IntList list, IntList list_to_append) {
    if (list_is_valid(list) && list_to_append != 0 && list_to_append->head != 0) {
        IntNode last = list_get_at_or_last(list->head, UINT_MAX, true);
        if (last != 0) {
            last->next = list_to_append->head;
        } else { /* empty list */
            list->head = list_to_append->head;
        }
        list_to_append->head = 0;
    } /* lists != 0 */
}

IntListIterator list_it_obtain(IntList list) {
    return 0;
}

void list_it_release(IntListIterator* p_it) {
}

bool list_it_is_valid(IntListIterator it) {
    return true;
}

bool list_it_next(IntListIterator it) {
    return true;
}

bool list_it_previous(IntListIterator it) {
    return true;
}

int list_it_get(IntListIterator it) {
    return 0;
}

void list_it_set(IntListIterator it, int value) {
}