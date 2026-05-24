#include "hashmap.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

object_t hashmap[HASHMAP_SIZE] = {0};

int hash(void *address) {
    uintptr_t addr_int = (uintptr_t)address;
    return ((addr_int * 5 + 9) % 65537) % HASHMAP_SIZE;
}

object_t hashmap_get(void *address) {
    int index = hash(address);
    object_t output = hashmap[index];
    while (output.address != address) {
        index = (index + 1) % HASHMAP_SIZE;
        if ((output.address == NULL && output.size == 0) || index == hash(address)) {
            write(2, "Error: hashmap tried to get an address that's not in it\n", 57);
            exit(1);
        }
        output = hashmap[index];
    }
    return output;
}

void hashmap_set(void *address, size_t size, int backtrace_count, void *backtrace[MAX_BACKTRACE]) {
    int index = hash(address);
    object_t *slot = &(hashmap[index]);
    while (slot->address != NULL) {
        index = (index + 1) % HASHMAP_SIZE;
        slot = &(hashmap[index]);
        if (index == hash(address)) {
            write(2, "Error: hashmap full!\n", 20);
            exit(1);
        }
    }
    slot->address = address;
    slot->size = size;
    slot->backtrace_count = backtrace_count;
    memcpy(slot->backtrace, backtrace, backtrace_count * sizeof(void *)); // can't just assign since lifespan of backtrace is gonna end with malloc (the one that calls hashmap_set)
}

void hashmap_delete(void *address) {
    int index = hash(address);
    object_t *slot = &hashmap[index];
    while (slot->address != address) {
        index = (index + 1) % HASHMAP_SIZE;
        slot = &(hashmap[index]);
        if (index == hash(address)) {
            write(2, "Error: hashmap tries to delete inexistant object!\n", 51);
            exit(1);
        }
    }
    slot->address = NULL;
    slot->size = (size_t) -1;
}
