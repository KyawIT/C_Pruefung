/*----------------------------------------------------------
 *				HTBLA-Leonding
 * ---------------------------------------------------------
 * Title:			Allocator for lists and list elements.
 * Author:			S. Schraml
 * ----------------------------------------------------------
 * Description:
 * Implementation of memory allocation functions.
 * ----------------------------------------------------------
 */

#include "allocator.h"
#include <stdio.h>
#include <string.h>
#include "shortcut.h"

/* ===================================================================== */
/* Private memory allocation functions */
#define MAX_BLOCK_CNT 64
struct MemAllocStat {
    bool block_alloc_calls;
    int alloc_call_cnt;
    int free_call_cnt;
    int mem_block_cnt;
    void* mem_blocks[MAX_BLOCK_CNT];
};
static struct MemAllocStat mem_stat = {false, 0, 0, 0, {0}};

void mem_reset_calls() {
    mem_stat.block_alloc_calls = false;
    mem_stat.alloc_call_cnt = 0;
    mem_stat.free_call_cnt = 0;
}

void mem_reset_stat() {
    mem_reset_calls();
    memset(mem_stat.mem_blocks, 0, MAX_BLOCK_CNT * sizeof(void*));
    mem_stat.mem_block_cnt = 0;
}

static int mem_get_alloc_idx(void* p) {
    if (p == 0) {
        return MAX_BLOCK_CNT;
    }
    
    int idx = 0;
    for (; idx < MAX_BLOCK_CNT && p != mem_stat.mem_blocks[idx]; idx++);
    return idx;
}

bool mem_is_allocated(void* p) {
    return mem_get_alloc_idx(p) < MAX_BLOCK_CNT;
}

void mem_block_allocs(bool block) {
    mem_stat.block_alloc_calls = block;
}

int mem_get_alloc_call_cnt() {
    return mem_stat.alloc_call_cnt;
}

int mem_get_free_call_cnt() {
    return mem_stat.free_call_cnt;
}

int mem_get_allocated_block_cnt() {
    return mem_stat.mem_block_cnt;
}

/* ===================================================================== */

void* alloc_mem(size_t size) {
    void* p = 0;
    mem_stat.alloc_call_cnt++;
    if (!mem_stat.block_alloc_calls) {
        p = malloc(size);
        if (p != 0) {
            int idx = 0;
            for (; idx < MAX_BLOCK_CNT && mem_stat.mem_blocks[idx] != 0; idx++);
            if (idx < MAX_BLOCK_CNT) {
                mem_stat.mem_blocks[idx] = p;
                mem_stat.mem_block_cnt++;
            } else {
                free(p);
                p = 0;
            }
        }
    }
    return p;
}

void free_mem(void* p_mem) {
    mem_stat.free_call_cnt++;
    if(p_mem == 0) {
		printf("\tFailure: Freeing 0 (NULL)\n");
    } else {
        int mem_block_idx = mem_get_alloc_idx(p_mem);
        if(mem_block_idx >= MAX_BLOCK_CNT) {
            printf("\tFailure: Freeing memory that is not allocated [%p]\n", p_mem);
        } else {
            mem_stat.mem_blocks[mem_block_idx] = 0;
            mem_stat.mem_block_cnt--;
        }
    }
    free(p_mem);
}

