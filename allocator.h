/*----------------------------------------------------------
 *				HTBLA-Leonding
 * ---------------------------------------------------------
 * Title:			Dynamic memory allocator.
 * Author:			S. Schraml
 * ----------------------------------------------------------
 * Description:
 * Declaration of memory allocation functions.
 * ----------------------------------------------------------
 */

#ifndef ___ALLOCATOR__H
#define ___ALLOCATOR__H

#include <stdlib.h>
#include <stdbool.h>

/**
 * Allocates a memory block of the given size in bytes
 * in a similar way as `malloc(size)`.
 * 
 * Note: use `sizeof(X)` function to determines the size
 * of a specfic type or value.
 * 
 * @param size The number of bytes to allocate.
 * @return The pointer to the allocated memory block or 0,
 * if no memory could be allocated.
 */
void* alloc_mem(size_t size);

/**
 * Release the addressed memory block that was allocated via function `alloc_mem(...)`.
 * 
 * @param p_mem The pointer to the memory to free.
 */
void free_mem(void* p_mem);

/* ==================================================== */
/* functions used for testing only! */

void mem_reset_calls();
void mem_reset_stat();
bool mem_is_allocated(void* p);
void mem_block_allocs(bool block);
int mem_get_alloc_call_cnt();
int mem_get_free_call_cnt();
int mem_get_allocated_block_cnt();


#endif
