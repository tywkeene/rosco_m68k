#ifndef SH_MALLOC_H
#define SH_MALLOC_H

#include <stdint.h>
#include <stddef.h>

#define _SH_HEAP_TOTAL_SIZE 128000 // 128kb
#define _SH_HEAP_BLOCK_SIZE 4
#define _SH_BLOCK_COUNT     ( _SH_HEAP_TOTAL_SIZE / _SH_HEAP_BLOCK_SIZE )

#define _SH_HEADER_SIZE     (sizeof(sh_malloc_header_t) + (_SH_BLOCK_COUNT * sizeof(sh_block_t)))

void sh_init_malloc_heap(void);
void *sh_malloc(size_t size);
void *sh_calloc(unsigned int n, size_t size);
void sh_free(void *ptr);

#endif
