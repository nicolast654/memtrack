#include <dlfcn.h>
#include <stdio.h>
#include <unistd.h>

void* (*malloc_ptr)(size_t) = NULL; // function pointer that returns a void* and takes size_t
int initialization = 0;
char init_data[4096]; // temp buffer that dlsym uses

void *malloc(size_t);

__attribute__((constructor))
void init() {
    write(1, "Before init\n", 13);
    initialization = 1;
    malloc_ptr = dlsym(RTLD_NEXT, "malloc");
    if (malloc_ptr) {
        write(1, "malloc ptr is set\n", 19);
    } else {
        write(1, "malloc ptr is nul\n", 19);
    }
    initialization = 0;
}

void *malloc(size_t size) {
    if (initialization) {
        write(1, "init\n", 6);
        return init_data;
    }
    write(1, "Before malloc\n", 15);
    void *output = (*malloc_ptr)(size);
    write(1, "After malloc\n", 14);
    return output;
}
