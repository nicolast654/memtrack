#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

void* (*malloc_ptr)(size_t) = NULL; // function pointer that returns a void* and takes size_t
int initialization = 0;
char init_data[4096]; // temp buffer that dlsym uses

void *malloc(size_t);

/*
 * Function that gets called as soon as shared object is loaded.
 * It gets a pointer to the real libc malloc function.printf
 * initialization is being set to 1 before dlsym and 0 after bc dlsym uses malloc and needs the data block.
 */
__attribute__((constructor))
void init() {
    initialization = 1;
    malloc_ptr = dlsym(RTLD_NEXT, "malloc");
    initialization = 0;
}

void *malloc(size_t size) {
    if (initialization) {
        return init_data;
    }
    void *output = (*malloc_ptr)(size);
    return output;
}
