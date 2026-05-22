#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

#include "hashmap.h"
#include "utils.h"

void* (*malloc_ptr)(size_t) = NULL; // function pointer that returns a void* and takes size_t
void (*free_ptr)(void *) = NULL;
void* (*calloc_ptr)(size_t, size_t) = NULL;
void* (*realloc_ptr)(void *, size_t) = NULL;

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
    calloc_ptr = dlsym(RTLD_NEXT, "calloc");
    realloc_ptr = dlsym(RTLD_NEXT, "realloc");
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

void *calloc(size_t nmemb, size_t size) {
    if (initialization) {
        return init_data;
    }
    void *output = (*calloc_ptr)(nmemb, size);
    hashmap_set(output, nmemb * size);
    return output;
}

void *realloc(void *ptr, size_t size) {
    if (initialization) {
        return init_data;
    }
    void *output = (*realloc_ptr)(ptr, size);
    if (ptr != NULL) {
        hashmap_delete(ptr);
    }
    if (size != 0) {
        hashmap_set(output, size);
    }
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
    int byte_leaks = 0;
    for (int i = 0; i < HASHMAP_SIZE; i++) {
        if (hashmap[i].address != NULL) {
            leaks++;
            byte_leaks += hashmap[i].size;
        }
    }
    char leaks_str[6] = {}; // assuming we will never have more than 999999 leaks
    char bytes_leaked_str[9] = {};

    write_int_to_buffer(byte_leaks, bytes_leaked_str);
    write_int_to_buffer(leaks, leaks_str);

    write_str(2, bytes_leaked_str);
    write(2, " bytes leaked accross ", 22);
    write_str(2, leaks_str);
    write(2, " memory leaks!\n", 15);
}

