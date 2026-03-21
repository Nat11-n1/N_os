#include "malloc.h"
#include "kernel\drivers\memory\Virtual_memory_manageur.h"
#include "kernel\drivers\memory\Physical_memory_manager.h"
#include "kernel\lib\types.h"

void *heap_start = 0;
void *heap_end = 0;
struct block_t{
    uint64_t size;
    int free;
    struct block_t * next;
    struct block_t * prev;
};

void *malloc(uint64_t size){
    if (heap_start == 0){
        uint64_t *Physical_Page = PMM_alloc_page();
        VMM_map_page(0x400000,Physical_Page);
        heap_start = 0x400000;
        heap_end = 0x400000;
    }
    struct block_t *current = heap_start;
    struct block_t *last = NULL;
    while (current != NULL){
        if (current->free ==1 && current->size >= size){
            current->free = 0;
            return (void *)(current + 1);
        }else{
            last = current;
            current = current->next;
        }
    }
    uint64_t Physical_page = PMM_alloc_page();
    VMM_map_page(heap_end,Physical_page);
    struct block_t *new_block = heap_end;
    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;
    new_block->prev = last;
    if (last != NULL){
        last->next = new_block;
    }
    heap_end += sizeof(struct block_t) + size;
    return (void *)(new_block + 1);
}

void free_malloc(void *ptr){
    struct block_t * block = ptr - 1;
    block->free = 1;
    if (block->next != NULL){
        if (block->next->free == 1){
            block->size += sizeof(struct block_t) + block->next->size;
            block->next = block->next->next;
            if (block->next != NULL){
                block->next->prev = block;
            }
        }
    }
}