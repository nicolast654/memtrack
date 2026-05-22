#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#include "hashmap.h"
#include "utils.h"

void* (*malloc_ptr)(size_t) = NULL; // function pointer that returns a void* and takes size_t
void (*free_ptr)(void *) = NULL;

int initialization = 0;
char init_data[4096]; // temp buffer that dlsym uses

void *malloc(size_t);

/*
 * Function that gets called as soon as shared object is loaded.
 * It gets a pointer to the real libc malloc function. printf
 * initialization is being set to 1 before dlsym and 0 after bc dlsym uses malloc and needs the data block.
 */
__attribute__((constructor))
void init() {
    initialization = 1;
    malloc_ptr = dlsym(RTLD_NEXT, "malloc");
    free_ptr = dlsym(RTLD_NEXT, "free");
    initialization = 0;
}

void *malloc(size_t size) {
    if (initialization) {
        return init_data;
    }
    void *output = (*malloc_ptr)(size);
    hashmap_set(output, size);
    return output;
}

void free(void *ptr) {
    if (initialization) {
        return; // small mem leak, but needed
    }
    hashmap_delete(ptr);
    (*free_ptr)(ptr);
}


__attribute__((destructor))
void print_leaks() {
    int leaks = 0;
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (hashmap[i].address != NULL) {
            leaks++;
        }
    }
    char leaks_str[6] = {}; // assuming we will never have more than 999999 leaks
    write_int_to_buffer(leaks, leaks_str);
    for (int i = 0; leaks_str[i] != '\0'; ++i) {
        write(2, &leaks_str[i], 1);
    }
    write(2, " memory leaks detected!\n", 24);
}

