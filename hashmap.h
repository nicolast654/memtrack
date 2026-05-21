#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

#define HASHMAP_SIZE 65536

typedef struct oject {
    void *address; // allocated address
    size_t size; // allocated size
} object_t;

object_t hashmap_get(void *address);
void hashmap_set(void *address, size_t size);
void hashmap_delete(void *address);

extern object_t hashmap[HASHMAP_SIZE];
#endif
