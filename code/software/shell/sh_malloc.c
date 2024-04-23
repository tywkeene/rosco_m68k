#include <stdio.h>
#include <stdalign.h>
#include <stdbool.h>
#include <kernelapi.h>

#include <kmain.h>
#include <sh_malloc.h>
#include <sh_util.h>

extern IKernel *k_api;

#define block_pointer(x) &(*x)+1

typedef struct {
    uint16_t size : 16;
    uint16_t used : 1;
} __attribute__((packed,aligned(4))) sh_block_header_t;

typedef struct {
    uint32_t *start_addr;
    uint16_t count;
} __attribute__((aligned(4))) sh_malloc_header_t;

uint32_t bytes_free = 0;
static sh_malloc_header_t *sh_malloc_headers;

void sh_init_malloc_heap(void) {
    sh_malloc_headers = (sh_malloc_header_t *) k_api->alloc_sys_object(sizeof(sh_malloc_header_t *));
    sh_malloc_headers->start_addr = (uint32_t *) k_api->mem_alloc(_SH_HEAP_TOTAL_SIZE);
    bytes_free = _SH_HEAP_TOTAL_SIZE;

    printf("-----------------------------------------\n");
    printf("  headers addr      : 0x%p\n", (uint32_t *)sh_malloc_headers);
    printf("  heap start addr   : 0x%p\n", (uint32_t *)sh_malloc_headers->start_addr);
    printf("  total heap size   : %s\n", friendly_size(_SH_HEAP_TOTAL_SIZE));
    printf("  managed blocks    : %d\n", _SH_BLOCK_COUNT);
    printf("  block header size : %d bytes\n", sizeof(sh_block_header_t));
    printf("  bytes free        : %d bytes (%s)\n", bytes_free, friendly_size(bytes_free));
    printf("-----------------------------------------\n");
}

static inline sh_block_header_t *next_free_block(size_t need){
    sh_block_header_t *p = (sh_block_header_t *) sh_malloc_headers->start_addr;
    for(unsigned int i = 0; i < _SH_BLOCK_COUNT; i++) {
        printf("allocating block of size %d bytes\n", need);
        if (!(i % 1000))
            printf("searching blocks %d/%d\n", i, _SH_BLOCK_COUNT);

        if (p->used == 0 && (p->size >= need || p->size == 0)){
            p->used = 1;
            p->size = need;
            return p;
        }
        uint32_t prev_size = p->size;
        p += sizeof(sh_block_header_t *) + prev_size;
    }
    return NULL;
}

void *sh_malloc(size_t size) {
    if (!size)
        return NULL;
    sh_block_header_t *blk = next_free_block(size);
    if (blk == NULL){
        sh_panic("sh_malloc() -- OOM");
    }
    bytes_free -= size;

    printf("found block : 0x%p\n",(uint32_t) &blk);
    printf("pointer     : 0x%p\n",(uint32_t) block_pointer(blk));
    return block_pointer(blk);
}

void *sh_calloc(unsigned int n, size_t size) {
    size_t need = (size * n);
    sh_block_header_t *blk = next_free_block(need);

    char *p = (char *) block_pointer(blk);
    for(unsigned int i = 0; i < need; i++)
        *p = 0;
    return p;
}

void sh_free(void *ptr) {
    sh_block_header_t *blk = (sh_block_header_t *) block_pointer((sh_block_header_t *)ptr);
    if (!blk->used)
        return;
    blk->used = 0;
    bytes_free += blk->size;
}
