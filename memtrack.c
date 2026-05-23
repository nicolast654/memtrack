#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <unistd.h>

#include "hashmap.h"
#include "utils.h"

void* (*malloc_ptr)(size_t) = NULL; // function pointer that returns a void* and takes size_t
void (*free_ptr)(void *) = NULL;
void* (*calloc_ptr)(size_t, size_t) = NULL;
void* (*realloc_ptr)(void *, size_t) = NULL;

int initialization = 0;
char init_data[8192]; // temp buffer that startup functions use

size_t offset = 0; // if many startup functions call malloc, each will have their own region

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


    /* since man page says: backtrace() don't call malloc() explicitly, but they are part of
     * libgcc, which gets loaded dynamically when first used.  Dynamic loading usually triggers
     * a call to malloc(3). If you need certain calls to these two functions to not allocate memory
     * (in signal handlers,  for example), you need to make sure libgcc is loaded beforehand. */
    void *b[16];
    int _ = backtrace(b, 1);
    initialization = 0;
}

void *malloc(size_t size) {
    if (initialization) {
        offset += size;
        return init_data + offset - size;
    }
    void *output = (*malloc_ptr)(size);

    void *bt[MAX_BACKTRACE] = {};
    int bt_count = backtrace(bt, MAX_BACKTRACE);

    hashmap_set(output, size, bt_count, bt);
    return output;
}

void *calloc(size_t nmemb, size_t size) {
    if (initialization) {
        offset += size * nmemb;
        return init_data + offset - size * nmemb;
    }
    void *output = (*calloc_ptr)(nmemb, size);

    void *bt[MAX_BACKTRACE] = {};
    int bt_count = backtrace(bt, MAX_BACKTRACE);

    hashmap_set(output, nmemb * size, bt_count, bt);
    return output;
}

void *realloc(void *ptr, size_t size) {
    if (initialization) {
        offset += size;
        return init_data + offset - size;
    }
    void *output = (*realloc_ptr)(ptr, size);

    void *bt[MAX_BACKTRACE] = {};
    int bt_count = backtrace(bt, MAX_BACKTRACE);

    if (ptr != NULL) {
        hashmap_delete(ptr);
    }
    if (size != 0) {
        hashmap_set(output, size, bt_count, bt);
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
            write(2, "LEAK DETECTED:\n", 15);
            char **trace= backtrace_symbols(hashmap[i].backtrace, hashmap[i].backtrace_count);
            // print every item of trace
            for (int j = 0; j < hashmap[i].backtrace_count; j++) {
                write_str(2, trace[j]);
                write(2, "\n", 2);
            }
            free(trace);
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

