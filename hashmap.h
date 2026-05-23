#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

#define HASHMAP_SIZE 65536
#define MAX_BACKTRACE 16

typedef struct oject {
    void *address; // allocated address
    size_t size; // allocated size
    int backtrace_count; // actual number of elements in the backtrace array
    void *backtrace[MAX_BACKTRACE]; // backtrace array
} object_t;

object_t hashmap_get(void *address);
void hashmap_set(void *address, size_t size, int backtrace_count, void *backtrace[MAX_BACKTRACE]);
void hashmap_delete(void *address);

extern object_t hashmap[HASHMAP_SIZE];
#endif
